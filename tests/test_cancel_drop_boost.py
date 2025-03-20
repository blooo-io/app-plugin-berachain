from pathlib import Path
import json
import os

from web3 import Web3
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient
import ledger_app_clients.ethereum.response_parser as ResponseParser
from ledger_app_clients.ethereum.utils import (
    get_selector_from_data,
    recover_transaction,
)
from ragger.navigator import NavInsID

from .utils import get_appname_from_makefile, DERIVATION_PATH


ROOT_SCREENSHOT_PATH = Path(__file__).parent
ABIS_FOLDER = "%s/abis" % (os.path.dirname(__file__))

PLUGIN_NAME = get_appname_from_makefile()


with open(
    "%s/0x656b95e550c07a9ffe548bd4085c72418ceb1dba.abi.json" % (ABIS_FOLDER)
) as file:
    contract = Web3().eth.contract(
        abi=json.load(file),
        # Get address from filename
        address=bytes.fromhex(os.path.basename(file.name).split(".")[0].split("x")[-1]),
    )


def test_cancel_drop_boost(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)
    public_key = "97266c94c490662479a9188b070c86e505df4f167016883af4114c8c1a71429e8f270c0c2f1b74375880f81828b768c0"
    amount = Web3.to_wei(1.5, "ether")
    data = contract.encode_abi(
        "cancelDropBoost",
        [
            bytes.fromhex(public_key),
            amount,
        ],
    )
    print(
        "km-logs ---- [test_cancel_drop_boost.py] (test_cancel_drop_boost) data: %s"
        % data
    )
    # first setup the external plugin
    client.set_external_plugin(
        PLUGIN_NAME,
        contract.address,
        # Extract function selector from the encoded data
        get_selector_from_data(data),
    )

    tx_params = {
        "nonce": 20,
        "maxFeePerGas": Web3.to_wei(145, "gwei"),
        "maxPriorityFeePerGas": Web3.to_wei(1.5, "gwei"),
        "gas": 173290,
        "to": contract.address,
        "value": 0,
        "chainId": 80094,
        "data": data,
    }
    # send the transaction
    with client.sign(DERIVATION_PATH, tx_params):
        # Validate the on-screen request by performing the navigation appropriate for this device
        if firmware.is_nano:
            navigator.navigate_until_text_and_compare(
                NavInsID.RIGHT_CLICK,
                [NavInsID.BOTH_CLICK],
                "Accept",
                ROOT_SCREENSHOT_PATH,
                test_name,
            )
        else:
            navigator.navigate_until_text_and_compare(
                NavInsID.SWIPE_CENTER_TO_LEFT,
                [NavInsID.USE_CASE_REVIEW_CONFIRM, NavInsID.USE_CASE_STATUS_DISMISS],
                "Hold to sign",
                ROOT_SCREENSHOT_PATH,
                test_name,
            )
    # verify signature
    vrs = ResponseParser.signature(client.response().data)
    addr = recover_transaction(tx_params, vrs)
    assert addr == wallet_addr.get()
