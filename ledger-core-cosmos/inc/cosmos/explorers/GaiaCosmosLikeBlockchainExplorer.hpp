/*
 *
 * GaiaCosmosLikeBlockchainExplorer.hpp
 * ledger-core
 *
 * Created by Pierre Pollastri on 29/11/2019.
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

#pragma once

#include <cosmos/explorers/CosmosLikeBlockchainExplorer.hpp>

#include <core/async/DedicatedContext.hpp>
#include <core/net/HttpClient.hpp>

namespace ledger {
    namespace core {

        class GaiaCosmosLikeBlockchainExplorer :
                public CosmosLikeBlockchainExplorer,
                public DedicatedContext {
        public:
            GaiaCosmosLikeBlockchainExplorer(
                    const std::shared_ptr<api::ExecutionContext> &context,
                    const std::shared_ptr<HttpClient> &http,
                    const api::CosmosLikeNetworkParameters &parameters,
                    const std::shared_ptr<api::DynamicObject> &configuration);

            const std::vector<TransactionFilter> &getTransactionFilters() override;
            FuturePtr<ledger::core::Block> getBlock(uint64_t &blockHeight) override;
            FuturePtr<ledger::core::cosmos::Account> getAccount(const std::string &account) override;
            FuturePtr<ledger::core::Block> getCurrentBlock() override;
            Future<TransactionList>
            getTransactions(const std::string &address, TransactionFilter &filter, int page, int limit) override;

        private:
            std::shared_ptr<HttpClient> _http;
            api::CosmosLikeNetworkParameters _parameters;
        };

    }
}
