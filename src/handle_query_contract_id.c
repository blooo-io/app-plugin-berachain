#include "plugin.h"

// Sets the first screen to display.
void handle_query_contract_id(ethQueryContractID_t *msg) {
    const context_t *context = (const context_t *) msg->pluginContext;
    // msg->name will be the upper sentence displayed on the screen.
    // msg->version will be the lower sentence displayed on the screen.

    // For the first screen, display the plugin name.
    strlcpy(msg->name, APPNAME, msg->nameLength);

    // EDIT THIS: Adapt the cases by modifying the strings you pass to `strlcpy`.
    switch (context->selectorIndex) {
        case CREATE_REWARD_VAULT:
            strlcpy(msg->version, "Create Reward Vault", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case DELEGATE:
            strlcpy(msg->version, "Delegate", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case MINT:
            strlcpy(msg->version, "Mint", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case CANCEL_BOOST:
            strlcpy(msg->version, "Cancel Boost", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case QUEUE_BOOST:
            strlcpy(msg->version, "Queue Boost", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case ACTIVATE_BOOST:
            strlcpy(msg->version, "Activate Boost", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case DROP_BOOST:
            strlcpy(msg->version, "Drop Boost", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        default:
            PRINTF("Selector index: %d not supported\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
