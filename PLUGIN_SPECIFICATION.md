# Technical Specification

## About

This documentation describes the smart contracts and functions supported by the Berachain plugin.

## Smart Contracts

Smart contracts covered by the plugin:

| Network   | Version | Smart Contract       | Address                                      |
| --------- | ------- | -------------------- | -------------------------------------------- |
| Berachain | V1      | BGT Token            | `0x656b95E550C07a9ffe548bd4085c72418Ceb1dba` |
| Berachain | V1      | Honey Factory        | `0xA4aFef880F5cE1f63c9fb48F661E27F8B4216401` |
| Berachain | V1      | Reward Vault         | `0x40C938e57A863cFdBF0a35Bbee35809Ec3EA54c8` |
| Berachain | V1      | Reward Vault Factory | `0x94Ad6Ac84f6C6FbA8b8CCbD71d9f4f101def52a8` |

## Functions

For the smart contracts implemented, the functions covered by the plugin:

| Contract             | Function          | Selector     | Displayed Parameters                                                                                                                                                                                                                                                                                 |
| -------------------- | ----------------- | ------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| BGT Token            | queueBoost        | `0x489fa30a` | <table><tbody> <tr><td><code>bytes pubkey</code></td></tr> <tr><td><code>uint128 amount</code></td></tr> </tbody></table>                                                                                                                                                                            |
| BGT Token            | cancelBoost       | `0xf59f3cbf` | <table><tbody> <tr><td><code>bytes pubkey</code></td></tr> <tr><td><code>uint128 amount</code></td></tr> </tbody></table>                                                                                                                                                                            |
| BGT Token            | activateBoost     | `0x9b05e781` | <table><tbody> <tr><td><code>address user</code></td></tr> <tr><td><code>bytes pubkey</code></td></tr> </tbody></table>                                                                                                                                                                              |
| BGT Token            | queueDropBoost    | `0x2b153ce1` | <table><tbody> <tr><td><code>bytes pubkey</code></td></tr> <tr><td><code>uint128 amount</code></td></tr> </tbody></table>                                                                                                                                                                            |
| BGT Token            | cancelDropBoost   | `0x9c8b0547` | <table><tbody> <tr><td><code>bytes pubkey</code></td></tr> <tr><td><code>uint128 amount</code></td></tr> </tbody></table>                                                                                                                                                                            |
| BGT Token            | dropBoost         | `0x814d2614` | <table><tbody> <tr><td><code>address user</code></td></tr> <tr><td><code>bytes pubkey</code></td></tr> </tbody></table>                                                                                                                                                                              |
| BGT Token            | delegate          | `0x5c19a95c` | <table><tbody> <tr><td><code>address delegatee</code></td></tr> </tbody></table>                                                                                                                                                                                                                     |
| BGT Token            | delegateBySig     | `0xc3cda520` | <table><tbody> <tr><td><code>address delegatee</code></td></tr> <tr><td><code>uint256 nonce</code></td></tr> <tr><td><code>uint256 expiry</code></td></tr> <tr><td><code>uint8 v</code></td></tr> <tr><td><code>bytes32 r</code></td></tr> <tr><td><code>bytes32 s</code></td></tr> </tbody></table> |
| Honey Factory        | mint              | `0x328ebaf7` | <table><tbody> </td></tr> <tr><td><code>uint256 amount</code></td></tr> <tr><td><code>address receiver</code></td></tr> <tr><td><code>bool expectBasketMode</code></td></tr> </tbody></table>                                                                                                        |
| Honey Factory        | redeem            | `0xed596315` | <table><tbody> <tr><td><code>address asset</code></td></tr> <tr><td><code>uint256 honeyAmount</code></td></tr> <tr><td><code>address receiver</code></td></tr> <tr><td><code>bool expectBasketMode</code></td></tr> </tbody></table>                                                                 |
| Reward Vault         | stake             | `0xa694fc3a` | <table><tbody> <tr><td><code>uint256 amount</code></td></tr> </tbody></table>                                                                                                                                                                                                                        |
| Reward Vault Factory | createRewardVault | `0x577ee5c7` | <table><tbody> <tr><td><code>address stakingToken</code></td></tr> </tbody></table>                                                                                                                                                                                                                  |

## Formatting

The C source code is expected to be formatted with `clang-format` 11.0.0 or higher.
