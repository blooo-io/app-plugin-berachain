#include "plugin.h"

void handle_finalize(ethPluginFinalize_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;

    switch (context->selectorIndex) {
        case CREATE_REWARD_VAULT:
        case DELEGATE:
            msg->numScreens = 1;
            break;
        case MINT:
        case REDEEM:
            msg->numScreens = 4;
            break;
        case CANCEL_BOOST:
        case QUEUE_BOOST:
        case ACTIVATE_BOOST:
        case DROP_BOOST:
            msg->numScreens = 3;
            break;
        case DELEGATE_BY_SIG:
            msg->numScreens = 6;
            break;
        default:
            PRINTF("Selector index: %d not supported\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }

    // EDIT THIS: set `tokenLookup1` (and maybe `tokenLookup2`) to point to
    // token addresses you will info for (such as decimals, ticker...).
    msg->tokenLookup1 = context->token_received;

    msg->result = ETH_PLUGIN_RESULT_OK;
}
