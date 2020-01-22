/*
 *
 * CosmosLikeOperationQuery
 *
 * Created by Gerry Agbobada 22/01/2020
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

#include <cosmos/CosmosLikeOperationQuery.hpp>

#include <cosmos/database/CosmosLikeTransactionDatabaseHelper.hpp>

namespace ledger {
        namespace core {

                CosmosLikeOperationQuery::CosmosLikeOperationQuery(
                        const std::shared_ptr<api::QueryFilter>& headFilter,
                        const std::shared_ptr<DatabaseSessionPool>& pool,
                        const std::shared_ptr<api::ExecutionContext>& context,
                        const std::shared_ptr<api::ExecutionContext>& mainContext)
                        : OperationQuery(headFilter, pool, context, mainContext) {

                }

                soci::rowset<soci::row> CosmosLikeOperationQuery::performExecute(soci::session &sql) {
                        // TODO : fix this Tezos copy-pasta
                        // return _builder.select("o.account_uid, o.uid, o.wallet_uid, o.type, o.date, o.senders, o.recipients,"
                        //                        "o.amount, o.fees, o.currency_name, o.trust, b.hash, b.height, b.time, orig_op.uid"
                        // )
                        //         .from("operations").to("o")
                        //         .outerJoin("blocks AS b", "o.block_uid = b.uid")
                        //         .outerJoin("tezos_originated_operations AS orig_op", "o.uid = orig_op.uid")
                        //         .execute(sql);
                }

                void CosmosLikeOperationQuery::inflateCompleteTransaction(
                        soci::session &sql, const std::string &accountUid, CosmosLikeOperation& operation) {
                        // TODO : fix this Tezos copy-pasta
                        // CosmosLikeBlockchainExplorerTransaction tx;

                        // operation.setExplorerTransaction(tx);
                        // std::string transactionHash;
                        // sql << "SELECT transaction_hash FROM tezos_operations WHERE uid = :uid"
                        //         , soci::use(operation.getUid())
                        //         , soci::into(transactionHash);
                        // CosmosLikeTransactionDatabaseHelper::getTransactionByHash(
                        //         sql, transactionHash, operation.getUid(), operation.getExplorerTransaction());
                }

                std::shared_ptr<CosmosLikeOperation> CosmosLikeOperationQuery::createOperation(std::shared_ptr<AbstractAccount> &account) {
                        // TODO : fix this Tezos copy-pasta
                        // CosmosLikeBlockchainExplorerTransaction tx;

                        // return std::make_shared<CosmosLikeOperation>(account->getWallet(), tx);
                }
        }
}
