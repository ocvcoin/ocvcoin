#!/usr/bin/env bash
# Copyright (c) 2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C
TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
BUILDDIR=${BUILDDIR:-$TOPDIR}
BINDIR=${BINDIR:-$BUILDDIR/src}
OCVCOIND=${OCVCOIND:-$BINDIR/ocvcoind}
SHARE_EXAMPLES_DIR=${SHARE_EXAMPLES_DIR:-$TOPDIR/share/examples}
EXAMPLE_CONF_FILE=${EXAMPLE_CONF_FILE:-$SHARE_EXAMPLES_DIR/ocvcoin.conf}

[ ! -x "$OCVCOIND" ] && echo "$OCVCOIND not found or not executable." && exit 1

DIRTY=""
VERSION_OUTPUT=$($OCVCOIND --version)
if [[ $VERSION_OUTPUT == *"dirty"* ]]; then
  DIRTY="${DIRTY}${OCVCOIND}\n"
fi

if [ -n "$DIRTY" ]
then
  echo -e "WARNING: $OCVCOIND was built from a dirty tree.\n"
  echo -e "To safely generate a ocvcoin.conf file, please commit your changes to $OCVCOIND, rebuild, then run this script again.\n"
fi

echo 'Generating example ocvcoin.conf file in share/examples/'

# create the directory, if it doesn't exist
mkdir -p "${SHARE_EXAMPLES_DIR}"

# create the header text
cat > "${EXAMPLE_CONF_FILE}" << 'EOF'
##
## ocvcoin.conf configuration file.
## Generated by contrib/devtools/gen-ocvcoin-conf.sh.
##
## Lines beginning with # are comments.
## All possible configuration options are provided. To use, copy this file
## to your data directory (default or specified by -datadir), uncomment
## options you would like to change, and save the file.
##


### Options
EOF

# parse the output from ocvcoind --help
# adding newlines is a bit funky to ensure portability for BSD
# see here for more details: https://stackoverflow.com/a/24575385
${OCVCOIND} --help \
    | sed '1,/Print this help message and exit/d' \
    | sed -E 's/^[[:space:]]{2}\-/#/' \
    | sed -E 's/^[[:space:]]{7}/# /' \
    | sed -E '/[=[:space:]]/!s/#.*$/&=1/' \
    | awk '/^#[a-z]/{x=$0;next}{if (NF==0) print x"\n",x="";else print}' \
    | sed 's,\(^[[:upper:]].*\)\:$,\
### \1,' \
    | sed 's/[[:space:]]*$//' >> "${EXAMPLE_CONF_FILE}"

# create the footer text
cat >> "${EXAMPLE_CONF_FILE}" << 'EOF'

# [Sections]
# Most options will apply to all networks. To confine an option to a specific
# network, add it under the relevant section below.
#
# Note: If not specified under a network section, the options addnode, connect,
# port, bind, rpcport, rpcbind, and wallet will only apply to mainnet.

# Options for mainnet
[main]

# Options for testnet
[test]

# Options for signet
[signet]

# Options for regtest
[regtest]
EOF