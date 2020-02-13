/*
 *
 * CosmosLikeTransactionDatabaseHelper
 *
 * Created by Hakim Aammar on 11/02/2020.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ledger
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


#ifndef LEDGER_CORE_COSMOSLIKEOPERATIONDATABASEHELPER_H
#define LEDGER_CORE_COSMOSLIKEOPERATIONDATABASEHELPER_H


#include <string>

#include <soci.h>

#include <core/operation/OperationDatabaseHelper.hpp>
//#include <cosmos/explorers/CosmosLikeBlockchainExplorer.hpp>

//#include <cosmos/api/CosmosLikeOperation.hpp>

namespace ledger {
    namespace core {
        class CosmosLikeOperationDatabaseHelper : public OperationDatabaseHelper {
        public:

            //static void updateOperation(soci::session& sql, const api::CosmosLikeOperation& operation);
            static void updateOperation(soci::session& sql, const std::string& opUid, const std::string& msgUid);

            // Tx --> DB
            //static void putOperation(soci::session &sql, CosmosLikeOperation &op);

            // DB --> Tx
            //static bool inflateOperation(soci::session &sql, const soci::row &row, CosmosLikeOperation &op);

            //static bool operationExists(soci::session &sql, const std::string &cosmosTxUid);

            // NOTE Replaced by a call to CosmmosLikeOperation::refreshUid() just before putOperation() ?
            //static std::string createCosmosOperationUid(const std::string &txUid, const std::string &msgUid);

        };
    }
}

#endif //LEDGER_CORE_COSMOSLIKEOPERATIONDATABASEHELPER_H
