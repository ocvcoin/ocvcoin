What is Ocvcoin?
----------------

It is a modified version of bitcoin and it's pow mechanism. Heavy computations are used for resistance to asics. It uses the Open CV library for these computations. Simply put, it generates a random image from the block header and applies various filters to it with Open CV. It then accepts the hash of the most recently created image as a block hash.

For more technical details (OCVCOIN'S POW SERVER):
[https://github.com/ocvcoin/pow_server/blob/main/pow_server.py](https://github.com/ocvcoin/pow_server/blob/main/pow_server.py)


AUTOMATIC INSTALLATION FOR UBUNTU 18

```
wget -qO - https://raw.githubusercontent.com/ocvcoin/ocvcoin/master/UBUNTU18_INSTALLER.sh | sudo bash

```


Block Explorer:
[https://explorer.ocvcoin.com/](https://explorer.ocvcoin.com/)



Sample Commands
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
