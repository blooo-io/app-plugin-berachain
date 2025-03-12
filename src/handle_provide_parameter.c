#include "plugin.h"

static void handle_beneficiary(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case BENEFICIARY:
            copy_address(context->beneficiary, msg->parameter, sizeof(context->beneficiary));
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_mint(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case ADDRESS:
            copy_address(context->address, msg->parameter, sizeof(context->address));
            context->next_param = MIN_AMOUNT_RECEIVED;
            break;
        case MIN_AMOUNT_RECEIVED:
            copy_parameter(context->amount_received,
                           msg->parameter,
                           sizeof(context->amount_received));
            context->next_param = BENEFICIARY;
            break;
        case BENEFICIARY:
            copy_address(context->beneficiary, msg->parameter, sizeof(context->beneficiary));
            context->next_param = BOOLEAN;
            break;
        case BOOLEAN:
            if (!U2BE_from_parameter(msg->parameter, &context->boolean)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_public_key_and_amount(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case OFFSET:
            context->offset = msg->parameter[0];
            context->next_param = MIN_AMOUNT_RECEIVED;
            break;
        case MIN_AMOUNT_RECEIVED:
            copy_parameter(context->amount_received,
                           msg->parameter,
                           sizeof(context->amount_received));
            context->next_param = LENGTH;
            break;
        case LENGTH:
            context->initialLength = msg->parameter[0];
            context->remainingLength = context->initialLength;
            context->next_param = PUBLIC_KEY;
            break;
        case PUBLIC_KEY:
            if (context->remainingLength == context->initialLength) {
                copy_parameter(context->public_key, msg->parameter, sizeof(context->public_key));
                context->remainingLength -= sizeof(context->public_key);
                context->next_param = PUBLIC_KEY;
            } else {
                // Copy the next 16 bytes of the public key inside address
                copy_parameter(context->address, msg->parameter, 16);
                context->remainingLength -= 16;
                if (context->remainingLength == 0) {
                    context->next_param = NONE;
                }
            }
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(ethPluginProvideParameter_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`) and then
    // the address (here `msg->parameter`).
    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);
    msg->result = ETH_PLUGIN_RESULT_OK;

    // EDIT THIS: adapt the cases and the names of the functions.
    switch (context->selectorIndex) {
        case CREATE_REWARD_VAULT:
        case DELEGATE:
            handle_beneficiary(msg, context);
            break;
        case MINT:
            handle_mint(msg, context);
            break;
        case CANCEL_BOOST:
        case QUEUE_BOOST:
            handle_public_key_and_amount(msg, context);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
