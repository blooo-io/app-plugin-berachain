[![Ensure compliance with Ledger guidelines](https://github.com/blooo-io/app-plugin-berachain/actions/workflows/guidelines_enforcer.yml/badge.svg?branch=develop)](https://github.com/blooo-io/app-plugin-berachain/actions/workflows/guidelines_enforcer.yml)
[![Compilation & tests](https://github.com/blooo-io/app-plugin-berachain/actions/workflows/build_and_functional_tests.yml/badge.svg?branch=develop)](https://github.com/blooo-io/app-plugin-berachain/actions/workflows/build_and_functional_tests.yml)

# app-plugin-berachain

Plugins are lightweight applications that go hand-in-hand with the Ethereum
Application on a Nano (S plus, X), Stax and Flex devices.

They allow users to safely interact with smart contracts by parsing the
transaction data and displaying its content in a human-readable way. This is
done on a "per contract" basis, meaning a plugin is required for every DApp.

## Quick start guide with VSCode

You can quickly setup a convenient environment to build and test your application by using [Ledger's VSCode developer tools extension](https://marketplace.visualstudio.com/items?itemName=LedgerHQ.ledger-dev-tools) which leverages the [ledger-app-dev-tools](https://github.com/LedgerHQ/ledger-app-builder/pkgs/container/ledger-app-builder%2Fledger-app-dev-tools) docker image.

It will allow you, whether you are developing on macOS, Windows or Linux to quickly **build** your apps, **test** them on **Speculos** and **load** them on any supported device.

- Install and run [Docker](https://www.docker.com/products/docker-desktop/).
- Make sure you have an X11 server running :
  - On Ubuntu Linux, it should be running by default.
  - On macOS, install and launch [XQuartz](https://www.xquartz.org/) (make sure to go to XQuartz > Preferences > Security and check "Allow client connections").
  - On Windows, install and launch [VcXsrv](https://sourceforge.net/projects/vcxsrv/) (make sure to configure it to disable access control).
- Install [VScode](https://code.visualstudio.com/download) and add [Ledger's extension](https://marketplace.visualstudio.com/items?itemName=LedgerHQ.ledger-dev-tools).
- Open a terminal and clone `app-plugin-berachain` with `git clone git@github.com:blooo-io/app-plugin-berachain.git`.
- Open the `app-plugin-berachain` folder with VSCode.
- Use Ledger extension's sidebar menu or open the tasks menu with `ctrl + shift + b` (`command + shift + b` on a Mac) to conveniently execute actions :
  - Build the app for the device model of your choice with `Build`.
  - You can also run functional tests, load the app on a physical device, and more.

:information_source: The terminal tab of VSCode will show you what commands the extension runs behind the scene.

## Documentation

The documentation about the plugin is in [PLUGIN_SPECIFICATON.md](https://github.com/blooo-io/app-plugin-berachain/blob/develop/PLUGIN_SPECIFICATION.md).

## Formatting

The C source code is expected to be formatted with `clang-format` 11.0.0 or higher.
