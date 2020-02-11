/*
 *
 * CosmosLikeTransactionDatabaseHelper
 *
 * Created by El Khalil Bellakrid on 06/01/2019.
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


#include <cosmos/database/CosmosLikeTransactionDatabaseHelper.hpp>

#include <core/database/SociOption.hpp>
#include <core/database/SociDate.hpp>
#include <core/database/SociNumber.hpp>
#include <core/crypto/SHA256.hpp>
#include <core/wallet/BlockDatabaseHelper.hpp>

#include <cosmos/database/SociCosmosAmount.hpp>
//#include <cosmos/CosmosLikeCurrencies.hpp>
#include <cosmos/CosmosLikeConstants.hpp>

#include <core/api/enum_from_string.hpp>

#include <boost/lexical_cast.hpp>


namespace ledger {
    namespace core {

        bool CosmosLikeTransactionDatabaseHelper::getTransactionByHash(soci::session &sql,
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

        bool CosmosLikeTransactionDatabaseHelper::inflateTransaction(soci::session &sql,
                                                                     const soci::row &row,
                                                                     cosmos::Transaction &tx) {

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

        bool CosmosLikeTransactionDatabaseHelper::transactionExists(soci::session &sql, const std::string &cosmosTxUid) {
            int32_t count = 0;
            sql << "SELECT COUNT(*) FROM cosmos_transactions WHERE transaction_uid = :cosmosTxUid", soci::use(cosmosTxUid), soci::into(count);
            return count == 1;
        }

        std::string CosmosLikeTransactionDatabaseHelper::createCosmosTransactionUid(const std::string &accountUid, const std::string &txHash) {
            auto result = SHA256::stringToHexHash(fmt::format("uid:{}+{}", accountUid, txHash));
            return result;
        }

        std::string CosmosLikeTransactionDatabaseHelper::createCosmosMessageUid(const std::string &txUid, uint64_t msgIndex) {
            auto result = SHA256::stringToHexHash(fmt::format("uid:{}+{}", txUid, msgIndex));
            return result;
        }

        static void insertMessage(soci::session& sql, const std::string& txUid, uint64_t index,
                const cosmos::Message& msg, const cosmos::MessageLog& log) {
            auto uid = CosmosLikeTransactionDatabaseHelper::createCosmosMessageUid(txUid, index);
            switch (cosmos::stringToMsgType(msg.type.c_str())) {
                case api::CosmosLikeMsgType::MSGSEND:
                    {
                        const auto& m = boost::get<cosmos::MsgSend>(msg.content);
                        std::string coins = soci::coinsToString(m.amount);
                        sql << "INSERT INTO cosmos_messages (uid,"
                               "transaction_uid, message_type, log,"
                               "success, msg_index, from_address, to_address, amount) "
                               "VALUES (:uid, :tuid, :mt, :log, :success, :mi, :fa, :ta, :amount)",
                               soci::use(uid), soci::use(txUid), soci::use(msg.type), soci::use(log.log),
                               soci::use(log.success ? 1 : 0), soci::use(log.messageIndex),
                               soci::use(m.fromAddress), soci::use(m.toAddress), soci::use(coins);
                    }
                    break;
                case api::CosmosLikeMsgType::MSGDELEGATE:
                    {
                        const auto& m = boost::get<cosmos::MsgDelegate>(msg.content);
                        sql << "INSERT INTO cosmos_messages (uid,"
                               "transaction_uid, message_type, log,"
                               "success, msg_index, delegator_address, validator_address, amount) "
                               "VALUES (:uid, :tuid, :mt, :log, :success, :mi, :fa, :ta, :amount)",
                                soci::use(uid), soci::use(txUid), soci::use(msg.type), soci::use(log.log),
                                soci::use(log.success ? 1 : 0), soci::use(log.messageIndex),
                                soci::use(m.delegatorAddress), soci::use(m.validatorAddress), soci::use(m.amount);
                    }
                    break;
                case api::CosmosLikeMsgType::MSGUNDELEGATE:
                    {
                        const auto& m = boost::get<cosmos::MsgUndelegate>(msg.content);
                        sql << "INSERT INTO cosmos_messages (uid,"
                               "transaction_uid, message_type, log,"
                               "success, msg_index, delegator_address, validator_address, amount)"
                               "VALUES (:uid, :tuid, :mt, :log, :success, :mi, :fa, :ta, :amount)",
                                soci::use(uid), soci::use(txUid), soci::use(msg.type), soci::use(log.log),
                                soci::use(log.success ? 1 : 0), soci::use(log.messageIndex),
                                soci::use(m.delegatorAddress), soci::use(m.validatorAddress), soci::use(m.amount);
                    }
                    break;
                case api::CosmosLikeMsgType::MSGREDELEGATE:
                    {
                        const auto& m = boost::get<cosmos::MsgRedelegate>(msg.content);
                        sql << "INSERT INTO cosmos_messages (uid,"
                               "transaction_uid, message_type, log,"
                               "success, msg_index, delegator_address, validator_src_address,"
                               "validator_dst_address, amount)"
                               "VALUES (:uid, :tuid, :mt, :log, :success, :mi, :fa, :ta, :amount)",
                                soci::use(uid), soci::use(txUid), soci::use(msg.type), soci::use(log.log),
                                soci::use(log.success ? 1 : 0), soci::use(log.messageIndex),
                                soci::use(m.delegatorAddress), soci::use(m.validatorSourceAddress),
                                soci::use(m.validatorDestinationAddress), soci::use(m.amount);
                    }
                    break;
                case api::CosmosLikeMsgType::MSGSUBMITPROPOSAL:
                    {
                        const auto& m = boost::get<cosmos::MsgSubmitProposal>(msg.content);
                        std::string coins = soci::coinsToString(m.initialDeposit);
                        sql << "INSERT INTO cosmos_messages (uid,"
                               "transaction_uid, message_type, log,"
                               "success, msg_index, proposer, content_type,"
                               "content_title, content_description, amount)"
                               "VALUES (:uid, :tuid, :mt, :log, :success, :mi, :proposer,"
                               ":ctype, :ctitle, :cdescription, :amount)",
                                soci::use(uid), soci::use(txUid), soci::use(msg.type), soci::use(log.log),
                                soci::use(log.success ? 1 : 0), soci::use(log.messageIndex),
                                soci::use(m.proposer), soci::use(m.content.type), soci::use(m.content.title),
                                soci::use(m.content.description), soci::use(coins);
                    }
                    break;
                case api::CosmosLikeMsgType::MSGVOTE:
                    {
                        const auto& m = boost::get<cosmos::MsgVote>(msg.content);
                        sql << "INSERT INTO cosmos_messages (uid,"
                               "transaction_uid, message_type, log,"
                               "success, msg_index, proposal_id, voter,"
                               "vote_option)"
                               "VALUES (:uid, :tuid, :mt, :log, :success, :mi, :pid, :voter, :opt)",
                                soci::use(uid), soci::use(txUid), soci::use(msg.type), soci::use(log.log),
                                soci::use(log.success ? 1 : 0), soci::use(log.messageIndex),
                                soci::use(m.proposalId), soci::use(m.voter),
                                soci::use(api::to_string(m.option));
                    }
                    break;
                case api::CosmosLikeMsgType::MSGDEPOSIT:
                    {
                        const auto& m = boost::get<cosmos::MsgRedelegate>(msg.content);
                        sql << "INSERT INTO cosmos_messages (uid,"
                               "transaction_uid, message_type, log,"
                               "success, msg_index, delegator_address, validator_src_address,"
                               "validator_dst_address, amount)"
                               "VALUES (:uid, :tuid, :mt, :log, :success, :mi, :fa, :ta, :amount)",
                                soci::use(uid), soci::use(txUid), soci::use(msg.type), soci::use(log.log),
                                soci::use(log.success ? 1 : 0), soci::use(log.messageIndex),
                                soci::use(m.delegatorAddress), soci::use(m.validatorSourceAddress),
                                soci::use(m.validatorDestinationAddress), soci::use(m.amount);
                    }
                    break;
                case api::CosmosLikeMsgType::MSGWITHDRAWDELEGATIONREWARD: {
                    const auto &m = boost::get<cosmos::MsgWithdrawDelegationReward>(msg.content);
                    sql << "INSERT INTO cosmos_messages (uid,"
                           "transaction_uid, message_type, log,"
                           "success, msg_index, delegator_address, validator_src_address)"
                           "VALUES (:uid, :tuid, :mt, :log, :success, :mi, :fa, :ta)",
                            soci::use(uid), soci::use(txUid), soci::use(msg.type), soci::use(log.log),
                            soci::use(log.success ? 1 : 0), soci::use(log.messageIndex),
                            soci::use(m.delegatorAddress), soci::use(m.validatorAddress);
                }
                    break;
                case api::CosmosLikeMsgType::UNKNOWN:break;
            }
        }

        // FIXME Test this
        static void insertTransaction(soci::session& sql, const std::string& uid, const cosmos::Transaction& tx) {
            Option<std::string> blockUid;
            if (tx.block.nonEmpty() && !tx.block.getValue().blockHash.empty()) {
                blockUid = BlockDatabaseHelper::createBlockUid(tx.block.getValue());
            }

            auto date = DateUtils::toJSON(tx.timestamp);
            auto fee = soci::coinsToString(tx.fee.amount);
            auto gas = tx.fee.gas.toString();
            auto gasUsed = tx.gasUsed.flatMap<std::string>([] (const BigInt& g) {
                return g.toString();
            });
            sql << "INSERT INTO cosmos_transactions("
                   "transaction_uid, hash, block_uid, time, fee_amount, gas, gas_used, memo"
                   ") VALUES (:uid, :hash, :buid, :time, :fee, :gas, :gas_used, :memo)",
                    soci::use(uid), soci::use(tx.hash), soci::use(blockUid), soci::use(date),
                    soci::use(fee), soci::use(gas), soci::use(gasUsed), soci::use(tx.memo);
        }

        // FIXME Test this
        std::string CosmosLikeTransactionDatabaseHelper::putTransaction(soci::session &sql,
                                                                        const std::string &accountUid,
                                                                        const cosmos::Transaction &tx) {
            auto blockUid = tx.block.map<std::string>([](const cosmos::Block &block) {
                return block.uid;
            });

            auto cosmosTxUid = createCosmosTransactionUid(accountUid, tx.hash);

            if (transactionExists(sql, cosmosTxUid)) {
                // UPDATE (we only update block information and gasUsed)
                if (tx.block.nonEmpty() && tx.block.getValue().blockHash.size() > 0) {
                    auto gasUsed = tx.gasUsed.flatMap<std::string>([] (const BigInt& g) {
                        return g.toString();
                    });
                    sql << "UPDATE cosmos_transactions SET block_uid = :uid, gas_used = :gas_used "
                           "WHERE hash = :tx_hash",
                            soci::use(blockUid), soci::use(gasUsed), soci::use(tx.hash);
                }
            } else {
                // Insert
                if (tx.block.nonEmpty() && !tx.block.getValue().blockHash.empty()) {
                    BlockDatabaseHelper::putBlock(sql, tx.block.getValue());
                }
                // Insert transaction
                insertTransaction(sql, cosmosTxUid, tx);
                // Insert messages
                auto index = 0;
                for (const auto& message : tx.messages) {
                    const auto& log = tx.logs[index];
                    // insertMessage(sql, cosmosTxUid, index, message, log);
                    index += 1;
                }
            }

            return cosmosTxUid;
        }
    }
}
