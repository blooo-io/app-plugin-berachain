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

static void handle_honey_functions(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case ASSET_ADDRESS:
            copy_address(context->token_received, msg->parameter, sizeof(context->token_received));
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
            context->initial_length = msg->parameter[0];
            context->remaining_length = context->initial_length;
            context->next_param = PUBLIC_KEY;
            break;
        case PUBLIC_KEY:
            if (context->remaining_length == context->initial_length) {
                copy_parameter(context->public_key, msg->parameter, sizeof(context->public_key));
                context->remaining_length -= sizeof(context->public_key);
                context->next_param = PUBLIC_KEY;
            } else {
                // Copy the last 16 bytes of the public key inside the beneficiary variable
                copy_parameter(context->beneficiary, msg->parameter, 16);
                context->remaining_length -= 16;
                if (context->remaining_length == 0) {
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

static void handle_address_and_public_key(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case ADDRESS:
            copy_address(context->address, msg->parameter, sizeof(context->address));
            context->next_param = OFFSET;
            break;
        case OFFSET:
            context->offset = msg->parameter[0];
            context->next_param = LENGTH;
            break;
        case LENGTH:
            context->initial_length = msg->parameter[0];
            context->remaining_length = context->initial_length;
            context->next_param = PUBLIC_KEY;
            break;
        case PUBLIC_KEY:
            if (context->remaining_length == context->initial_length) {
                copy_parameter(context->public_key, msg->parameter, sizeof(context->public_key));
                context->remaining_length -= sizeof(context->public_key);
                context->next_param = PUBLIC_KEY;
            } else {
                // Copy the last 16 bytes of the public key inside the beneficiary variable
                copy_parameter(context->beneficiary, msg->parameter, 16);
                context->remaining_length -= 16;
                if (context->remaining_length == 0) {
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

static void handle_amount(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case MIN_AMOUNT_RECEIVED:
            copy_parameter(context->amount_received,
                           msg->parameter,
                           sizeof(context->amount_received));
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

static void handle_delegate_by_sig(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case BENEFICIARY:
            copy_address(context->beneficiary, msg->parameter, sizeof(context->beneficiary));
            context->next_param = NONCE;
            break;
        case NONCE:
            copy_parameter(context->amount_received,
                           msg->parameter,
                           sizeof(context->amount_received));
            context->next_param = EXPIRY;
            break;
        case EXPIRY:
            copy_parameter(context->public_key, msg->parameter, sizeof(context->public_key));
            context->next_param = V;
            break;
        case V:
            if (!U2BE_from_parameter(msg->parameter, &context->boolean)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = R;
            break;
        case R:
            // Copy the first 20 bytes of R and store it in the address variable
            copy_parameter(context->address, msg->parameter, 20);
            // Copy the remaining 12 bytes of R and store it in the first 12 bytes of the
            // token_received variable
            copy_parameter(context->token_received, msg->parameter + 20, 12);
            context->next_param = S;
            break;
        case S:
            // Copy the first 11 bytes of S and store it in the rest variable
            copy_parameter(context->rest, msg->parameter, 11);
            // Copy the next 11 bytes of S and store it in the ticker variable
            copy_parameter(context->ticker, msg->parameter + 11, 11);
            // Copy the next 8 bytes of S and store it in the last 8 bytes of the
            // token_received variable
            copy_parameter(context->token_received + 12, msg->parameter + 22, 8);
            // Copy the next byte of S and store it in the remaining_length variable
            memcpy(&context->remaining_length, msg->parameter + 30, 1);
            // Copy the next byte of S and store it in the initial_length variable
            memcpy(&context->initial_length, msg->parameter + 31, 1);
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
        case REDEEM:
            handle_honey_functions(msg, context);
            break;
        case CANCEL_BOOST:
        case QUEUE_BOOST:
        case CANCEL_DROP_BOOST:
        case QUEUE_DROP_BOOST:
            handle_public_key_and_amount(msg, context);
            break;
        case ACTIVATE_BOOST:
        case DROP_BOOST:
            handle_address_and_public_key(msg, context);
            break;
        case STAKE:
            handle_amount(msg, context);
            break;
        case DELEGATE_BY_SIG:
            handle_delegate_by_sig(msg, context);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
