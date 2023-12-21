// Copyright (c) 2023 Ocvcoin Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "logprintf.h"

#include <clang-tidy/ClangTidyModule.h>
#include <clang-tidy/ClangTidyModuleRegistry.h>

class OcvcoinModule final : public clang::tidy::ClangTidyModule
{
public:
    void addCheckFactories(clang::tidy::ClangTidyCheckFactories& CheckFactories) override
    {
        CheckFactories.registerCheck<ocvcoin::LogPrintfCheck>("ocvcoin-unterminated-logprintf");
    }
};

static clang::tidy::ClangTidyModuleRegistry::Add<OcvcoinModule>
    X("ocvcoin-module", "Adds ocvcoin checks.");

volatile int OcvcoinModuleAnchorSource = 0;
