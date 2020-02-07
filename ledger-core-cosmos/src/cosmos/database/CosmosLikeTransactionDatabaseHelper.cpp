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
#include <cosmos/CosmosLikeCurrencies.hpp>
#include <cosmos/CosmosLikeConstants.hpp>

using namespace soci;

namespace ledger {
    namespace core {

        bool CosmosLikeTransactionDatabaseHelper::getTransactionByHash(soci::session &sql,
                                                                       const std::string &hash,
                                                                       cosmos::Transaction &tx) {

            rowset<row> rows = (sql.prepare << "SELECT tx.transaction_uid, tx.hash, tx.time, "
                    "tx.memo, tx.gas_used, tx.fee_amount, tx.block_height"
                    // Block
                    "block.height, block.hash, block.time, block.currency_name, "
                    // Message
                    "msg.message_type, msg.log, msg,success, msg.msg_index, msg.from_address,"
                    "msg.to_address, msg.amount_value, msg.delegator_address, msg.validator_address,"
                    "msg.validator_src_address, msg.validator_src_address, msg.content_type, msg.content_title,"
                    "msg.content_description, msg.proposer, msg.voter, msg.proposal_id, msg.vote_option,"
                    "msg.depositor "
                    // End of Message
                    "FROM cosmos_transactions AS tx "
                    "LEFT JOIN blocks AS block ON tx.block_uid = block.uid "
                    "LEFT JOIN cosmos_messages AS msg ON msg.transaction_uid = tx.transaction_uid "
                    "WHERE tx.hash = :hash "
                    "ORDER BY msg.msg_index ASC", use(hash));

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
            tx.timestamp = row.get<std::chrono::system_clock::time_point>(3);
            tx.memo = row.get<Option<std::string>>(6).getValueOr("");
            tx.gasUsed = row.get<Option<std::string>>(8).map<BigInt>([] (const std::string& v) {
                return BigInt::fromString(v);
            });
            tx.fee = row.get<cosmos::Fee>(9);

             //TODO: gas limit and price
            if (row.get_indicator(7) != i_null) {
                CosmosLikeBlockchainExplorer::Block block;
                block.height = get_number<uint64_t>(row, 8);
                block.blockHash = row.get<std::string>(9);
                block.time = row.get<std::chrono::system_clock::time_point>(10);
                block.currencyName = row.get<std::string>(11);
                tx.block = block;
            }

//            CosmosLikeBlockchainExplorerMessage msg;
//            msg.type = row.get<std::string>(12);
//            msg.sender = row.get<std::string>(13);
//            msg.recipient = row.get<std::string>(14);
//            msg.amount = row.get<std::string>(15);
//            msg.fees = row.get<std::string>(16);
//            tx.messages.push_back(msg);
            return true;
        }

        bool CosmosLikeTransactionDatabaseHelper::transactionExists(soci::session &sql,
                                                                    const std::string &cosmosTxUid) {
            int32_t count = 0;
            sql << "SELECT COUNT(*) FROM cosmos_transactions WHERE transaction_uid = :cosmosTxUid", use(cosmosTxUid), into(
                    count);
            return count == 1;
        }

        std::string CosmosLikeTransactionDatabaseHelper::createCosmosTransactionUid(const std::string &accountUid,
                                                                                    const std::string &txHash) {
            auto result = SHA256::stringToHexHash(fmt::format("uid:{}+{}", accountUid, txHash));
            return result;
        }

        std::string CosmosLikeTransactionDatabaseHelper::createCosmosMessageUid(const std::string &txUid,
                                                                                uint64_t msgIndex) {
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
                               use(uid), use(txUid), use(msg.type), use(log.log),
                               use(log.success ? 1 : 0), use(log.messageIndex),
                               use(m.fromAddress), use(m.toAddress), use(coins);
                    }
                    break;
                case api::CosmosLikeMsgType::MSGDELEGATE:
                    {
                        const auto& m = boost::get<cosmos::MsgDelegate>(msg.content);
                        sql << "INSERT INTO cosmos_messages (uid,"
                               "transaction_uid, message_type, log,"
                               "success, msg_index, delegator_address, validator_address, amount) "
                               "VALUES (:uid, :tuid, :mt, :log, :success, :mi, :fa, :ta, :amount)",
                                use(uid), use(txUid), use(msg.type), use(log.log),
                                use(log.success ? 1 : 0), use(log.messageIndex),
                                use(m.delegatorAddress), use(m.validatorAddress), use(m.amount);
                    }
                    break;
                case api::CosmosLikeMsgType::MSGUNDELEGATE:
                    {
                        const auto& m = boost::get<cosmos::MsgUndelegate>(msg.content);
                        sql << "INSERT INTO cosmos_messages (uid,"
                               "transaction_uid, message_type, log,"
                               "success, msg_index, delegator_address, validator_address, amount)"
                               "VALUES (:uid, :tuid, :mt, :log, :success, :mi, :fa, :ta, :amount)",
                                use(uid), use(txUid), use(msg.type), use(log.log),
                                use(log.success ? 1 : 0), use(log.messageIndex),
                                use(m.delegatorAddress), use(m.validatorAddress), use(m.amount);
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
                                use(uid), use(txUid), use(msg.type), use(log.log),
                                use(log.success ? 1 : 0), use(log.messageIndex),
                                use(m.delegatorAddress), use(m.validatorSourceAddress),
                                use(m.validatorDestinationAddress), use(m.amount);
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
                                use(uid), use(txUid), use(msg.type), use(log.log),
                                use(log.success ? 1 : 0), use(log.messageIndex),
                                use(m.proposer), use(m.content.type), use(m.content.title),
                                use(m.content.description), use(coins);
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
                                use(uid), use(txUid), use(msg.type), use(log.log),
                                use(log.success ? 1 : 0), use(log.messageIndex),
                                use(m.proposalId), use(m.voter),
                                use(api::to_string(m.option));
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
                                use(uid), use(txUid), use(msg.type), use(log.log),
                                use(log.success ? 1 : 0), use(log.messageIndex),
                                use(m.delegatorAddress), use(m.validatorSourceAddress),
                                use(m.validatorDestinationAddress), use(m.amount);
                    }
                    break;
                case api::CosmosLikeMsgType::MSGWITHDRAWDELEGATIONREWARD: {
                    const auto &m = boost::get<cosmos::MsgWithdrawDelegationReward>(msg.content);
                    sql << "INSERT INTO cosmos_messages (uid,"
                           "transaction_uid, message_type, log,"
                           "success, msg_index, delegator_address, validator_src_address)"
                           "VALUES (:uid, :tuid, :mt, :log, :success, :mi, :fa, :ta)",
                            use(uid), use(txUid), use(msg.type), use(log.log),
                            use(log.success ? 1 : 0), use(log.messageIndex),
                            use(m.delegatorAddress), use(m.validatorAddress);
                }
                    break;
                case api::CosmosLikeMsgType::UNKNOWN:break;
            }
        }

        static void insertTransaction(soci::session& sql, const std::string& uid, const cosmos::Transaction& tx) {
            Option<std::string> blockUid;
            Option<uint64_t> blockHeight;
            if (tx.block.nonEmpty() && !tx.block.getValue().blockHash.empty()) {
                blockUid = BlockDatabaseHelper::createBlockUid(tx.block.getValue());
            } else if (tx.block.nonEmpty()) {
                blockHeight = tx.block.getValue().height;
            }

            auto date = DateUtils::toJSON(tx.timestamp);
            auto fee = soci::coinsToString(tx.fee.amount);
            auto gas = tx.fee.gas.toString();
            auto gasUsed = tx.gasUsed.flatMap<std::string>([] (const BigInt& g) {
                return g.toString();
            });
            sql << "INSERT INTO cosmos_transactions("
                   "transaction_uid, hash, block_uid, time, fee_amount, gas, gas_used, memo"
                   ") VALUES(:uid, :hash, :buid, :time, :fee, :gas, :gas_used, :memo)",
                    use(uid), use(tx.hash), use(blockUid), use(date), use(fee), use(gas),
                    use(gasUsed), use(tx.memo);
        }

        std::string CosmosLikeTransactionDatabaseHelper::putTransaction(soci::session &sql,
                                                                        const std::string &accountUid,
                                                                        const cosmos::Transaction &tx) {
            auto blockUid = tx.block.map<std::string>([](const CosmosLikeBlockchainExplorer::Block &block) {
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
                            use(blockUid), use(gasUsed), use(tx.hash);
                }
                return cosmosTxUid;
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
                return cosmosTxUid;
            }
        }
    }
}
