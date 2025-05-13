#include "plugin.h"

// EDIT THIS: You need to adapt / remove the static functions (set_send_ui, set_amount_ui ...) to
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

/**
 * Format byte buffer to uppercase hexadecimal string.
 *
 * @param[in]  in
 *   Pointer to input byte buffer.
 * @param[in]  in_len
 *   Length of input byte buffer.
 * @param[out] out
 *   Pointer to output string.
 * @param[in]  out_len
 *   Length of output string.
 *
 * @return number of bytes written if success, -1 otherwise.
 *
 */
static int local_format_hex(const uint8_t *in, size_t in_len, char *out, size_t out_len) {
    if (out_len < 2 * in_len + 1) {
        return -1;
    }

    const char hex[] = "0123456789ABCDEF";
    size_t i = 0;
    int written = 0;

    while (i < in_len && (i * 2 + (2 + 1)) <= out_len) {
        uint8_t high_nibble = (in[i] & 0xF0) >> 4;
        *out = hex[high_nibble];
        out++;

        uint8_t low_nibble = in[i] & 0x0F;
        *out = hex[low_nibble];
        out++;

        i++;
        written += 2;
    }

    *out = '\0';

    return written + 1;
}

static bool set_amount_ui(ethQueryContractUI_t *msg, const context_t *context) {
    uint8_t decimals = context->decimals;
    const char *ticker = (char *) context->ticker;

    // If the token look up failed, use the default network ticker along with the default decimals.
    if (!context->token_found && context->selectorIndex != REDEEM) {
        decimals = WEI_TO_ETHER;
        ticker = msg->network_ticker;
    }

    switch (context->selectorIndex) {
        case MINT:
            strlcpy(msg->title, "Amount to send", msg->titleLength);
            break;
        case REDEEM:
            strlcpy(msg->title, "Amount to send", msg->titleLength);
            ticker = "HONEY";
            decimals = 18;
            break;
        case QUEUE_BOOST:
        case CANCEL_BOOST:
        case QUEUE_DROP_BOOST:
        case CANCEL_DROP_BOOST:
            strlcpy(msg->title, "Amount", msg->titleLength);
            ticker = "BGT";
            decimals = 18;
            break;
        default:
            strlcpy(msg->title, "Amount", msg->titleLength);
            break;
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
        case MINT:
        case REDEEM:
            strlcpy(msg->title, "Beneficiary", msg->titleLength);
            break;
        case DELEGATE:
        case DELEGATE_BY_SIG:
            strlcpy(msg->title, "Delegatee", msg->titleLength);
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
        case REDEEM:
            strlcpy(msg->title, "Address", msg->titleLength);
            break;
        case ACTIVATE_BOOST:
        case DROP_BOOST:
            strlcpy(msg->title, "User Address", msg->titleLength);
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
        case REDEEM:
            strlcpy(msg->title, "Basket Mode", msg->titleLength);
            break;
        default:
            PRINTF("Received an invalid selectorIndex\n");
            break;
    }
    int result = -1;
    if (context->boolean == 0) {
        result = snprintf(msg->msg, msg->msgLength, "%s", "False");
    } else {
        result = snprintf(msg->msg, msg->msgLength, "%s", "True");
    }
    return result >= 0;
}

static bool set_public_key_ui(ethQueryContractUI_t *msg, context_t *context, bool first_chunk) {
    if (first_chunk) {
        strlcpy(msg->title, "Public Key Pt 1", msg->titleLength);
    } else {
        strlcpy(msg->title, "Public Key Pt 2", msg->titleLength);
    }

    if (first_chunk) {
        // Format the first chunk of the public key as a hex string
        msg->msg[0] = '0';
        msg->msg[1] = 'x';
        if (local_format_hex(context->public_key,
                             32,  // First 32 bytes of public key
                             msg->msg + 2,
                             msg->msgLength - 2) != -1) {
            return true;
        }
    } else {
        // Format the second chunk of the public key as a hex string
        if (local_format_hex(context->beneficiary,
                             16,  // Last 16 bytes of public key
                             msg->msg,
                             msg->msgLength) != -1) {
            return true;
        }
    }

    return false;
}

// Set UI for "Warning" screen.
static bool set_warning_ui(ethQueryContractUI_t *msg, context_t *context __attribute__((unused))) {
    strlcpy(msg->title, "WARNING", msg->titleLength);
    strlcpy(msg->msg, "Unknown token", msg->msgLength);
    return true;
}

static bool set_asset_received_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Asset to receive", msg->titleLength);
    // display the address

    // Prefix the address with `0x`.
    msg->msg[0] = '0';
    msg->msg[1] = 'x';

    // We need a random chainID for legacy reasons with `getEthAddressStringFromBinary`.
    // Setting it to `0` will make it work with every chainID :)
    uint64_t chainid = 0;

    // Get the string representation of the address stored in `context->beneficiary`. Put it in
    // `msg->msg`.
    return getEthAddressStringFromBinary(
        context->token_received,
        msg->msg + 2,  // +2 here because we've already prefixed with '0x'.
        chainid);
}

static bool set_nonce_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Nonce", msg->titleLength);
    // format the nonce as a hex string from the amount_received variable
    return amountToString(context->amount_received,
                          sizeof(context->amount_received),
                          0,
                          "",
                          msg->msg,
                          msg->msgLength);
}

static bool set_expiry_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Expiry", msg->titleLength);
    return amountToString(context->public_key,
                          sizeof(context->public_key),
                          0,
                          "",
                          msg->msg,
                          msg->msgLength);
}

static bool set_v_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "V", msg->titleLength);
    // Convert the uint8 into a string
    return amountToString(&context->boolean,
                          sizeof(context->boolean),
                          0,
                          "",
                          msg->msg,
                          msg->msgLength);
}

static bool set_r_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "R", msg->titleLength);
    uint8_t r[32];
    uint8_t offset = 0;
    // copy the 20 bytes of the address variable to the r variable
    memcpy(r, context->address, 20);
    offset += 20;
    // copy the first 12 bytes of the token_received variable to the r variable
    memcpy(r + offset, context->token_received, 12);

    msg->msg[0] = '0';
    msg->msg[1] = 'x';
    int ret = local_format_hex(r, sizeof(r), msg->msg + 2, msg->msgLength - 2);
    return ret != -1;
}

static bool set_s_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "S", msg->titleLength);
    uint8_t s[32];
    uint8_t offset = 0;
    // copy the 11 bytes of the rest variable to the s variable
    memcpy(s, context->rest, 11);
    offset += 11;
    // copy the 11 bytes of the ticker variable to the s variable
    memcpy(s + offset, context->ticker, 11);
    offset += 11;
    // copy the last 8 bytes of the token_received variable to the s variable
    uint8_t token_received_offset = sizeof(context->token_received) - 8;
    memcpy(s + offset, context->token_received + token_received_offset, 8);
    offset += 8;
    // copy the byte of the remaining_length variable to the s variable
    s[offset++] = context->remaining_length;
    // copy the  byte of the initial_length variable to the s variable
    s[offset] = context->initial_length;

    msg->msg[0] = '0';
    msg->msg[1] = 'x';
    int ret = local_format_hex(s, sizeof(s), msg->msg + 2, msg->msgLength - 2);
    return ret != -1;
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
                    if (context->token_found) {
                        ret = set_amount_ui(msg, context);
                    } else {
                        ret = set_warning_ui(msg, context);
                    }
                    break;
                case 1:
                    if (context->token_found) {
                        ret = set_beneficiary_ui(msg, context);
                    } else {
                        ret = set_amount_ui(msg, context);
                    }
                    break;
                case 2:
                    if (context->token_found) {
                        ret = set_boolean_ui(msg, context);
                    } else {
                        ret = set_beneficiary_ui(msg, context);
                    }
                    break;
                case 3:
                    if (context->token_found) {
                        ret = true;
                    } else {
                        ret = set_boolean_ui(msg, context);
                    }
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    break;
            }
            break;
        case REDEEM:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_asset_received_ui(msg, context);
                    break;
                case 1:
                    ret = set_amount_ui(msg, context);
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
        case CANCEL_BOOST:
        case QUEUE_BOOST:
        case CANCEL_DROP_BOOST:
        case QUEUE_DROP_BOOST:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_public_key_ui(msg, context, true);
                    break;
                case 1:
                    ret = set_public_key_ui(msg, context, false);
                    break;
                case 2:
                    ret = set_amount_ui(msg, context);
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    break;
            }
            break;
        case ACTIVATE_BOOST:
        case DROP_BOOST:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_address_ui(msg, context);
                    break;
                case 1:
                    ret = set_public_key_ui(msg, context, true);
                    break;
                case 2:
                    ret = set_public_key_ui(msg, context, false);
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    break;
            }
            break;
        case DELEGATE_BY_SIG:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_beneficiary_ui(msg, context);
                    break;
                case 1:
                    ret = set_nonce_ui(msg, context);
                    break;
                case 2:
                    ret = set_expiry_ui(msg, context);
                    break;
                case 3:
                    ret = set_v_ui(msg, context);
                    break;
                case 4:
                    ret = set_r_ui(msg, context);
                    break;
                case 5:
                    ret = set_s_ui(msg, context);
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    break;
            }
            break;
        case STAKE:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_amount_ui(msg, context);
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
