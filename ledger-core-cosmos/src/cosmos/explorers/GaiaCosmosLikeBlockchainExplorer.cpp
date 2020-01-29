/*
 *
 * GaiaCosmosLikeBlockchainExplorer.cpp
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

#include <cosmos/explorers/GaiaCosmosLikeBlockchainExplorer.hpp>

#include <core/api/Configuration.hpp>

#include <cosmos/explorers/RpcsParsers.hpp>

namespace ledger {
    namespace core {

        using MsgType = cosmos::MsgType;

        static const std::vector<CosmosLikeBlockchainExplorer::TransactionFilter> GAIA_FILTER {
            "recipient=",
            "sender=",
            "delegator="
        };

        GaiaCosmosLikeBlockchainExplorer::GaiaCosmosLikeBlockchainExplorer(
                const std::shared_ptr<api::ExecutionContext> &context, const std::shared_ptr<HttpClient> &http,
                const api::CosmosLikeNetworkParameters &parameters,
                const std::shared_ptr<api::DynamicObject> &configuration) :
                DedicatedContext(context),
                CosmosLikeBlockchainExplorer(configuration, {api::Configuration::BLOCKCHAIN_EXPLORER_API_ENDPOINT}),
                _http(http), _parameters(parameters) {

        }

        const std::vector<CosmosLikeBlockchainExplorer::TransactionFilter> &
        GaiaCosmosLikeBlockchainExplorer::getTransactionFilters() {
            return GAIA_FILTER;
        }

        FuturePtr<ledger::core::Block> GaiaCosmosLikeBlockchainExplorer::getBlock(uint64_t &blockHeight) {
            return _http->GET(fmt::format("/blocks/{}", blockHeight)).json(true).mapPtr<cosmos::Block>(getContext(), [=] (const HttpRequest::JsonResult& response) {
                auto result = std::make_shared<cosmos::Block>();
                const auto& document = std::get<1>(response)->GetObject();
                rpcs_parsers::parseBlock(document, "cosmos", *result);
                return result;
            });
        }

        FuturePtr<ledger::core::cosmos::Account>
        GaiaCosmosLikeBlockchainExplorer::getAccount(const std::string &account) {
            return _http->GET(fmt::format("/auth/accounts/{}", account)).json(true).mapPtr<cosmos::Account>(getContext(), [=] (const HttpRequest::JsonResult& response) {
                auto result = std::make_shared<cosmos::Account>();
                const auto& document = std::get<1>(response)->GetObject();
                rpcs_parsers::parseAccount(document, *result);
                return result;
            });
        }

        FuturePtr<ledger::core::Block> GaiaCosmosLikeBlockchainExplorer::getCurrentBlock() {
            return _http->GET(fmt::format("/blocks/latest")).json(true)
            .map<std::shared_ptr<Block>>(getContext(),
             [=] (const HttpRequest::JsonResult& response) {
                 auto result = std::make_shared<Block>();
                 const auto& document = std::get<1>(response)->GetObject();
                 rpcs_parsers::parseBlock(document, "cosmos", *result);
                 return result;
             });
        }

        Future<cosmos::TransactionList> GaiaCosmosLikeBlockchainExplorer::getTransactions(const std::string &address,
                                                                                          CosmosLikeBlockchainExplorer::TransactionFilter &filter,
                                                                                              int page, int limit) {
            return _http->GET(fmt::format("/txs?{}{}&page={}&limit={}", filter, address, page, limit)).json(true).map<cosmos::TransactionList>(getContext(), [=] (const HttpRequest::JsonResult& response) {
                cosmos::TransactionList result;
                const auto& document = std::get<1>(response)->GetArray();
                for (const auto& node : document) {
                    auto tx = std::make_shared<cosmos::Transaction>();
                    rpcs_parsers::parseTransaction(node, *tx);
                    result.emplace_back(tx);
                }
                return result;
            });
        }
    }
}
