What is Ocvcoin?
----------------

It is modified version of bitcoin and it's pow mechanism. Heavy computations are used for resistance to asics. It uses the Open CV library for these computations.

HOW TO INSTALL & MINING VIDEO: [https://youtu.be/z3Eh1TgPu78](https://youtu.be/z3Eh1TgPu78)

Dowload
----------------

(ONLY FOR 64 BIT WINDOWS)

Firstly you need to download, install and run the pow server. (pow server must be running as long as the ocvcoin application is running!)

OCVCOIN'S PROOF-OF-WORK SERVER: [ocvcoin_pow_server-1.0.0.0-win64-setup.exe](https://github.com/ocvcoin/pow_server/releases/download/1.0.0.0/ocvcoin_pow_server-1.0.0.0-win64-setup.exe)

OCVCOIN: [ocvcoin-0.21.1-win64-setup.exe](https://github.com/ocvcoin/ocvcoin/releases/download/v0.21.1/ocvcoin-0.21.1-win64-setup.exe)


How Can I Mining?
----------------

No mining pool or special mining software yet. You can try to mine with the built-in mining tool.

Firstly, if you haven't created one yet, you should create a wallet for yourself by clicking the "Create a new wallet" button.

Then you must press the "Receive" button and press the "create new receiving address" button on the page that opens.

Click the "Copy Address" button in the newly opened window and close the window.

Open the console pressing CTRL + T 

To try to mine one block, type "generatetoaddress 1 TheAddressYouJustCopied" in the command line and enter.



Some Online Services
----------------

Block Explorer: [https://explorer.ocvcoin.com/](https://explorer.ocvcoin.com/)

Simple Wallet: [https://wallet.ocvcoin.com/](https://wallet.ocvcoin.com/)



Build From Source
----------------

(ONLY FOR UBUNTU 18)

```
wget -qO - https://raw.githubusercontent.com/ocvcoin/ocvcoin/master/UBUNTU18_INSTALLER.sh | sudo bash

```


Simple Shell Commands
----------------

Create Wallet:

```
sudo ocvcoin-cli createwallet ""

```

Get Your Wallet Address:

```
sudo ocvcoin-cli getaddressesbylabel ""

```

Simply trying to mine one block:

```
sudo ocvcoin-cli generatetoaddress 1 yourwalletaddress

```
