/*
 *
 * CosmosLikeOperationDatabaseHelper
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


#include <cosmos/database/CosmosLikeOperationDatabaseHelper.hpp>

//#include <core/database/SociOption.hpp>
//#include <core/database/SociDate.hpp>
//#include <core/database/SociNumber.hpp>
//#include <core/crypto/SHA256.hpp>
//#include <core/wallet/BlockDatabaseHelper.hpp>

//#include <cosmos/database/SociCosmosAmount.hpp>
//#include <cosmos/CosmosLikeCurrencies.hpp>
//#include <cosmos/CosmosLikeConstants.hpp>

//#include <core/api/enum_from_string.hpp>

//#include <boost/lexical_cast.hpp>


namespace ledger {
    namespace core {

        //void CosmosLikeOperationDatabaseHelper::updateOperation(soci::session& sql, const CosmosLikeOperation& op)
        void CosmosLikeOperationDatabaseHelper::updateOperation(soci::session& sql, const std::string& opUid, const std::string& msgUid)
        {
            sql << "INSERT INTO cosmos_operations VALUES(:uid, :message_uid)", soci::use(opUid), soci::use(msgUid);
        }

#if 0
    /*
        bool CosmosLikeOperationDatabaseHelper::getOperationByHash(soci::session &sql,
                                                                       const std::string &hash,
                                                                       cosmos::Transaction &tx) {
            soci::rowset<soci::row> rows = (sql.prepare << "SELECT tx.transaction_uid, tx.hash, tx.time, "
                    "tx.fee_amount, tx.gas, tx.gas_used, tx.memo, tx.block_uid, "
                    "block.hash, block.height, block.time, block.currency_name "
                    "FROM cosmos_transactions AS tx "
                    "LEFT JOIN blocks AS block ON tx.block_uid = block.uid "
                    "WHERE tx.hash = :hash ", soci::use(hash));

            for (auto &row : rows) {
                inflateTransaction(sql, row, tx);
                return true;
            }

            return false;
        }
    */
        bool CosmosLikeOperationDatabaseHelper::inflateOperation(soci::session &sql,
                                                                const soci::row &row,
                                                                CosmosLikeOperation &op) {

            // TODO

            tx.uid = row.get<std::string>(0);
            tx.hash = row.get<std::string>(1);
            tx.timestamp = row.get<std::chrono::system_clock::time_point>(2);
            soci::stringToCoins(row.get<std::string>(3), tx.fee.amount);
            tx.fee.gas = row.get<std::string>(4);
            tx.gasUsed = row.get<Option<std::string>>(5).map<BigInt>([] (const std::string& v) {
                return BigInt::fromString(v);
            });
            tx.memo = row.get<Option<std::string>>(6).getValueOr("");

            if (row.get_indicator(7) != soci::i_null) {
                cosmos::Block block;
                block.uid = row.get<std::string>(7);
                block.blockHash = row.get<std::string>(8);
                block.height = soci::get_number<uint64_t>(row, 9);
                block.time = row.get<std::chrono::system_clock::time_point>(10);
                block.currencyName = row.get<std::string>(11);
                tx.block = block;
            }

            soci::rowset<soci::row> msgRows = (sql.prepare <<
                    "SELECT * FROM cosmos_messages WHERE transaction_uid = :uid ORDER BY cosmos_messages.uid",
                    soci::use(tx.uid));

            for (auto &msgRow : msgRows) {
                cosmos::Message msg;
                auto msgType = msgRow.get<std::string>(2);
                msg.type = msgType;
                switch (cosmos::stringToMsgType(msgType.c_str())) {
                    case api::CosmosLikeMsgType::MSGSEND:
                        {
                            msg.content = cosmos::MsgSend();
                            auto &content = boost::get<cosmos::MsgSend>(msg.content);
                            content.fromAddress = msgRow.get<std::string>(6);
                            content.toAddress = msgRow.get<std::string>(7);
                            soci::stringToCoins(msgRow.get<std::string>(8), content.amount);
                        }
                        break;
                    case api::CosmosLikeMsgType::MSGDELEGATE:
                        {
                            msg.content = cosmos::MsgDelegate();
                            auto &content = boost::get<cosmos::MsgDelegate>(msg.content);
                            content.delegatorAddress = msgRow.get<std::string>(9);
                            content.validatorAddress = msgRow.get<std::string>(10);
                            soci::stringToCoin(msgRow.get<std::string>(8), content.amount);
                        }
                        break;
                    case api::CosmosLikeMsgType::MSGUNDELEGATE:
                        {
                            msg.content = cosmos::MsgUndelegate();
                            auto &content = boost::get<cosmos::MsgUndelegate>(msg.content);
                            content.delegatorAddress = msgRow.get<std::string>(9);
                            content.validatorAddress = msgRow.get<std::string>(10);
                            soci::stringToCoin(msgRow.get<std::string>(8), content.amount);
                        }
                        break;
                    case api::CosmosLikeMsgType::MSGREDELEGATE:
                        {
                            msg.content = cosmos::MsgRedelegate();
                            auto &content = boost::get<cosmos::MsgRedelegate>(msg.content);
                            content.delegatorAddress = msgRow.get<std::string>(9);
                            content.validatorSourceAddress = msgRow.get<std::string>(11);
                            content.validatorDestinationAddress = msgRow.get<std::string>(12);
                            soci::stringToCoin(msgRow.get<std::string>(8), content.amount);
                        }
                        break;
                    case api::CosmosLikeMsgType::MSGSUBMITPROPOSAL:
                        {
                            msg.content = cosmos::MsgSubmitProposal();
                            auto &content = boost::get<cosmos::MsgSubmitProposal>(msg.content);
                            content.content.type = msgRow.get<std::string>(13);
                            content.content.title = msgRow.get<std::string>(14);
                            content.content.description = msgRow.get<std::string>(15);
                            content.proposer = msgRow.get<std::string>(16);
                            soci::stringToCoins(msgRow.get<std::string>(8), content.initialDeposit);
                        }
                        break;
                    case api::CosmosLikeMsgType::MSGVOTE:
                        {
                            msg.content = cosmos::MsgVote();
                            auto &content = boost::get<cosmos::MsgVote>(msg.content);
                            content.voter = msgRow.get<std::string>(17);
                            content.proposalId = msgRow.get<std::string>(18);
                            content.option = api::from_string<api::CosmosLikeVoteOption>(msgRow.get<std::string>(19));
                        }
                        break;
                    case api::CosmosLikeMsgType::MSGDEPOSIT:
                        {
                            msg.content = cosmos::MsgDeposit();
                            auto &content = boost::get<cosmos::MsgDeposit>(msg.content);
                            content.depositor = msgRow.get<std::string>(20);
                            content.proposalId = msgRow.get<std::string>(18);
                            soci::stringToCoins(msgRow.get<std::string>(8), content.amount);
                        }
                        break;
                    case api::CosmosLikeMsgType::MSGWITHDRAWDELEGATIONREWARD:
                        {
                            msg.content = cosmos::MsgWithdrawDelegationReward();
                            auto &content = boost::get<cosmos::MsgWithdrawDelegationReward>(msg.content);
                            content.delegatorAddress = msgRow.get<std::string>(9);
                            content.validatorAddress = msgRow.get<std::string>(10);
                        }
                        break;
                    case api::CosmosLikeMsgType::UNKNOWN:
                        break;
                }
                tx.messages.push_back(msg);

                cosmos::MessageLog log;
                log.log = msgRow.get<std::string>(3);
                log.success = soci::get_number<int32_t>(msgRow, 4) != 0;
                log.messageIndex = soci::get_number<int32_t>(msgRow, 5);
                tx.logs.push_back(log);
            }
            return true;
        }

        // FIXME Test this
        bool CosmosLikeOperationDatabaseHelper::putOperation(soci::session &sql, CosmosLikeOperation &op) {

            op.refreshUid();

            if (operationExists(sql, op.uid)) {
                // TODO Update ??
                return false;
            } else {
                // Insert operation
                insertOperation(sql, op);
                return true;
            }
        }

        bool CosmosLikeOperationDatabaseHelper::operationExists(soci::session &sql, const std::string &opUid) {
            int32_t count = 0;
            sql << "SELECT COUNT(*) FROM cosmos_operations WHERE operation_uid = :opUid", soci::use(opUid), soci::into(count);
            return count == 1;
        }

        // FIXME Test this
        static void insertOperation(soci::session& sql, const CosmosLikeOperation& op) {

            OperationDatabaseHelper::putOperation(sql, op);

            sql << "INSERT INTO cosmos_operations(uid, message_uid) "
                   "VALUES (:uid, :msg_uid)",
                    soci::use(op.uid), soci::use(op.msgData.uid);
        }
#endif

    }
}
