/*
 * AlgorandTransactionDatabaseHelper
 *
 * Created by Hakim Aammar on 18/05/2020.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Ledger
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef LEDGER_CORE_ALGORANDTRANSACTIONDATABASEHELPER_H
#define LEDGER_CORE_ALGORANDTRANSACTIONDATABASEHELPER_H

#include <algorand/model/transactions/AlgorandTransaction.hpp>

#include <soci.h>

namespace ledger {
namespace core {
namespace algorand {

    class TransactionDatabaseHelper {

    public:

        static bool transactionExists(soci::session & sql,
                                      const std::string & txUid);

        static bool getTransactionByHash(soci::session & sql,
                                         const std::string & hash,
                                         model::Transaction & tx);

        static std::string createTransactionUid(const std::string & accountUid,
                                                const std::string & txHash);

        static std::string putTransaction(soci::session & sql,
                                          const std::string & accountUid,
                                          const model::Transaction & tx);

    };

} // namespace algorand
} // namespace core
} // namespace ledger

#endif // LEDGER_CORE_ALGORANDTRANSACTIONDATABASEHELPER_H
