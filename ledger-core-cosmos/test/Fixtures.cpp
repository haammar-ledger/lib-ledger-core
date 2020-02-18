#include "Fixtures.hpp"

namespace ledger {
        namespace testing {
                namespace cosmos {

                        std::shared_ptr<core::CosmosLikeAccount>
                        createCosmosLikeAccount(const std::shared_ptr<core::AbstractWallet>& wallet,
                                                int32_t index,
                                                const api::AccountCreationInfo &info) {
                                auto i = info;
                                i.index = index;
                                return std::dynamic_pointer_cast<core::CosmosLikeAccount>(::wait(wallet->newAccountWithInfo(i)));
                        }

                        std::shared_ptr<core::CosmosLikeAccount>
                        createCosmosLikeAccount(const std::shared_ptr<core::AbstractWallet>& wallet,
                                                int32_t index,
                                                const api::ExtendedKeyAccountCreationInfo &info) {
                                auto i = info;
                                i.index = index;
                                return std::dynamic_pointer_cast<CosmosLikeAccount>(::wait(wallet->newAccountWithExtendedKeyInfo(i)));
                        }

                        void setupSendMessage(Message& msg, const std::chrono::system_clock::time_point& timeRef) {
                                msg.type = "cosmos-sdk/MsgSend";
                                MsgSend sendMsg;
                                sendMsg.fromAddress = "cosmos155svs6sgxe55rnvs6ghprtqu0mh69kehrn0dqr";
                                sendMsg.toAddress = "cosmos1sd4tl9aljmmezzudugs7zlaya7pg2895tyn79r";
                                sendMsg.amount.emplace_back("900000", "uatom");
                                msg.content = sendMsg;
                        }

                        void setupTransactionWithSingleMessage(Transaction& tx, const Message& msg, const std::chrono::system_clock::time_point& timeRef) {
                                tx.hash = "A1E44688B429AF17322EC33CE62876FA415EFC8D9244A2F51454BD025F416594";
                                api::Block block;
                                block.blockHash = "52B39D45B438C6995CD448B09963954883B0F7A57E9EFC7A95E0A6C5BAC09C00";
                                block.currencyName = "atom";
                                block.height = 744795;
                                block.time = timeRef;
                                tx.block = block;
                                tx.fee.gas = BigInt(30000);
                                tx.fee.amount.emplace_back("30", "uatom");
                                tx.gasUsed = BigInt(26826);
                                tx.timestamp = timeRef;
                                tx.memo = "Sent by Ledger";
                                tx.messages.push_back(msg);
                                MessageLog log;
                                log.messageIndex = 0;
                                log.success = true;
                                log.log = "Success";
                                tx.logs.push_back(log);
                        }

                        void assertSameSendMessage(const Message& msgRef, const Message& msgResult) {
                                const auto& sendMsgRef = boost::get<MsgSend>(msgRef.content);
                                const auto& sendMsgResult = boost::get<MsgSend>(msgResult.content);
                                EXPECT_EQ(sendMsgResult.fromAddress, sendMsgRef.fromAddress);
                                EXPECT_EQ(sendMsgResult.toAddress, sendMsgRef.toAddress);
                                EXPECT_EQ(sendMsgResult.amount.size(), 1);
                                EXPECT_EQ(sendMsgResult.amount[0].amount, sendMsgRef.amount[0].amount);
                                EXPECT_EQ(sendMsgResult.amount[0].denom, sendMsgRef.amount[0].denom);
                        }

                        void assertSameTransactionWithSingleMessage(const Transaction& txRef, const Transaction& txResult) {

                                EXPECT_EQ(txResult.hash, txRef.hash);
                                EXPECT_EQ(txResult.timestamp, txRef.timestamp);

                                EXPECT_EQ(txResult.block.hasValue(), txRef.block.hasValue());
                                EXPECT_EQ(txResult.block.getValue().blockHash, txRef.block.getValue().blockHash);
                                EXPECT_EQ(txResult.block.getValue().currencyName, txRef.block.getValue().currencyName);
                                EXPECT_EQ(txResult.block.getValue().height, txRef.block.getValue().height);
                                EXPECT_EQ(txResult.block.getValue().time, txRef.block.getValue().time);

                                EXPECT_EQ(txResult.fee.amount.size(), 1);
                                EXPECT_EQ(txResult.fee.amount[0].amount, txRef.fee.amount[0].amount);
                                EXPECT_EQ(txResult.fee.amount[0].denom, txRef.fee.amount[0].denom);
                                EXPECT_EQ(txResult.fee.gas, txRef.fee.gas);
                                EXPECT_EQ(txResult.gasUsed.hasValue(), txRef.gasUsed.hasValue());
                                EXPECT_EQ(txResult.gasUsed.getValue().to_string(), txRef.gasUsed.getValue().to_string());

                                EXPECT_EQ(txResult.messages.size(), 1);
                                EXPECT_EQ(txResult.messages[0].type, txRef.messages[0].type);

                                EXPECT_EQ(txResult.memo, txRef.memo);
                                EXPECT_EQ(txResult.logs.size(), 1);
                                EXPECT_EQ(txResult.logs[0].success, txRef.logs[0].success);
                        }

                }
        }
}
