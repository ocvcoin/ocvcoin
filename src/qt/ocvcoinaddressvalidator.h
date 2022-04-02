// Copyright (c) 2011-2020 The Ocvcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef OCVCOIN_QT_OCVCOINADDRESSVALIDATOR_H
#define OCVCOIN_QT_OCVCOINADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class OcvcoinAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit OcvcoinAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

/** Ocvcoin address widget validator, checks for a valid ocvcoin address.
 */
class OcvcoinAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit OcvcoinAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

#endif // OCVCOIN_QT_OCVCOINADDRESSVALIDATOR_H
