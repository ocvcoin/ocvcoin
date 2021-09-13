#!/usr/bin/env bash
# Copyright (c) 2016-2019 The Ocvcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C
TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
BUILDDIR=${BUILDDIR:-$TOPDIR}

BINDIR=${BINDIR:-$BUILDDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

OCVCOIND=${OCVCOIND:-$BINDIR/ocvcoind}
OCVCOINCLI=${OCVCOINCLI:-$BINDIR/ocvcoin-cli}
OCVCOINTX=${OCVCOINTX:-$BINDIR/ocvcoin-tx}
WALLET_TOOL=${WALLET_TOOL:-$BINDIR/ocvcoin-wallet}
OCVCOINQT=${OCVCOINQT:-$BINDIR/qt/ocvcoin-qt}

[ ! -x $OCVCOIND ] && echo "$OCVCOIND not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
read -r -a OCVVER <<< "$($OCVCOINCLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }')"

# Create a footer file with copyright content.
# This gets autodetected fine for ocvcoind if --version-string is not set,
# but has different outcomes for ocvcoin-qt and ocvcoin-cli.
echo "[COPYRIGHT]" > footer.h2m
$OCVCOIND --version | sed -n '1!p' >> footer.h2m

for cmd in $OCVCOIND $OCVCOINCLI $OCVCOINTX $WALLET_TOOL $OCVCOINQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${OCVVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${OCVVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
