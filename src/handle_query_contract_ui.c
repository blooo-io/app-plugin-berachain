#include "plugin.h"

// EDIT THIS: You need to adapt / remove the static functions (set_send_ui, set_receive_ui ...) to
// match what you wish to display.

// Set UI for the "Send" screen.
// EDIT THIS: Adapt / remove this function to your needs.
/* static bool set_send_ui(ethQueryContractUI_t *msg) {
    strlcpy(msg->title, "Send", msg->titleLength);

    const uint8_t *eth_amount = msg->pluginSharedRO->txContent->value.value;
    uint8_t eth_amount_size = msg->pluginSharedRO->txContent->value.length;

    // Converts the uint256 number located in `eth_amount` to its string representation and
    // copies this to `msg->msg`.
    return amountToString(eth_amount,
                          eth_amount_size,
                          WEI_TO_ETHER,
                          "ETH",
                          msg->msg,
                          msg->msgLength);
} */

// Set UI for "Receive" screen.
// EDIT THIS: Adapt / remove this function to your needs.
static bool set_receive_ui(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Amount", msg->titleLength);

    uint8_t decimals = context->decimals;
    const char *ticker = context->ticker;

    // If the token look up failed, use the default network ticker along with the default decimals.
    if (!context->token_found) {
        decimals = WEI_TO_ETHER;
        ticker = msg->network_ticker;
    }

    return amountToString(context->amount_received,
                          sizeof(context->amount_received),
                          decimals,
                          ticker,
                          msg->msg,
                          msg->msgLength);
}

// Set UI for "Beneficiary" screen.
static bool set_beneficiary_ui(ethQueryContractUI_t *msg, context_t *context) {
    switch (context->selectorIndex) {
        case CREATE_REWARD_VAULT:
            strlcpy(msg->title, "Address", msg->titleLength);
            break;
        case DELEGATE:
        case MINT:
            strlcpy(msg->title, "Beneficiary", msg->titleLength);
            break;
        default:
            PRINTF("Received an invalid selectorIndex\n");
            break;
    }

    // Prefix the address with `0x`.
    msg->msg[0] = '0';
    msg->msg[1] = 'x';

    // We need a random chainID for legacy reasons with `getEthAddressStringFromBinary`.
    // Setting it to `0` will make it work with every chainID :)
    uint64_t chainid = 0;

    // Get the string representation of the address stored in `context->beneficiary`. Put it in
    // `msg->msg`.
    return getEthAddressStringFromBinary(
        context->beneficiary,
        msg->msg + 2,  // +2 here because we've already prefixed with '0x'.
        chainid);
}

static bool set_address_ui(ethQueryContractUI_t *msg, context_t *context) {
    switch (context->selectorIndex) {
        case MINT:
            strlcpy(msg->title, "Address", msg->titleLength);
            break;
        default:
            PRINTF("Received an invalid selectorIndex\n");
            break;
    }

    // Prefix the address with `0x`.
    msg->msg[0] = '0';
    msg->msg[1] = 'x';

    // We need a random chainID for legacy reasons with `getEthAddressStringFromBinary`.
    // Setting it to `0` will make it work with every chainID :)
    uint64_t chainid = 0;

    return getEthAddressStringFromBinary(
        context->address,
        msg->msg + 2,  // +2 here because we've already prefixed with '0x'.
        chainid);
}

static bool set_boolean_ui(ethQueryContractUI_t *msg, context_t *context) {
    switch (context->selectorIndex) {
        case MINT:
            strlcpy(msg->title, "Basket Mode", msg->titleLength);
            break;
        default:
            PRINTF("Received an invalid selectorIndex\n");
            break;
    }

    if (context->boolean == 0) {
        snprintf(msg->msg, msg->msgLength, "%s", "False");
        return true;
    } else {
        snprintf(msg->msg, msg->msgLength, "%s", "True");
        return true;
    }
    return false;
}

void handle_query_contract_ui(ethQueryContractUI_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    bool ret = false;

    // msg->title is the upper line displayed on the device.
    // msg->msg is the lower line displayed on the device.

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    // EDIT THIS: Adapt the cases for the screens you'd like to display.
    switch (context->selectorIndex) {
        case CREATE_REWARD_VAULT:
        case DELEGATE:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_beneficiary_ui(msg, context);
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    break;
            }
            break;
        case MINT:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_address_ui(msg, context);
                    break;
                case 1:
                    ret = set_receive_ui(msg, context);
                    break;
                case 2:
                    ret = set_beneficiary_ui(msg, context);
                    break;
                case 3:
                    ret = set_boolean_ui(msg, context);
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    break;
            }
            break;
        default:
            PRINTF("Received an invalid selectorIndex\n");
            break;
    }
    msg->result = ret ? ETH_PLUGIN_RESULT_OK : ETH_PLUGIN_RESULT_ERROR;
}
