# Ocvcoin

an open-source, peer-to-peer digital currency

---

## Downloads

* **OCVCOIN CORE**: [ocvcoin-26.0.1-win64-setup.exe](https://github.com/ocvcoin/ocvcoin/releases/download/v26.0.1/ocvcoin-26.0.1-win64-setup.exe)
  
* **GPU MINER**: [https://github.com/ocvcoin/gpuminer/blob/main/README.md](https://github.com/ocvcoin/gpuminer/blob/main/README.md)

---

## Build From Source

> **Note:** Requires an x86-based 64-bit processor.

### 1. Automatic Build Script for Ubuntu & Debian

This script builds Ocvcoin Core from source and sets it up to run automatically as a system service.

Run the following command:

```bash
wget -qO - https://raw.githubusercontent.com/ocvcoin/ocvcoin/master/UBUNTU_AUTO_BUILD_AND_INSTALL.sh | sudo bash

```

* **To customize your settings, edit the config file:** `/etc/ocvcoin/ocvcoin.conf`
* **To launch the Graphical Interface (GUI), use this command:**
```bash
ocvcoin-qt -listen=0

```



### 2. Automatic Build Script for Windows

If you want to build the `ocvcoin-26.0.1-win64-setup.exe` installer yourself, you can do so using WSL (Windows Subsystem for Linux), Ubuntu, or Debian.

Run this command in your Linux terminal:

```bash
wget -qO - https://raw.githubusercontent.com/ocvcoin/ocvcoin/master/UBUNTU_AUTO_BUILD_AND_PACK_WINDOWS_SETUP.sh | sudo bash

```

---
