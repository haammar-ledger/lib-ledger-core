/*
 *
 * rpcs_parsers.hpp
 * ledger-core
 *
 * Created by Pierre Pollastri on 15/06/2019.
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

#ifndef LEDGER_CORE_RPCS_PARSERS_HPP
#define LEDGER_CORE_RPCS_PARSERS_HPP

#include <cassert>

#include <rapidjson/document.h>

#include <core/utils/Option.hpp>
#include <core/utils/DateUtils.hpp>

#include <cosmos/explorers/CosmosLikeBlockchainExplorer.hpp>
#include <cosmos/CosmosLikeConstants.hpp>
#include <cosmos/CosmosLikeCurrencies.hpp>

#define COSMOS_PARSE_MSG_CONTENT(MsgType) if (out.type == "cosmos-sdk/"#MsgType) return parse##MsgType(contentNode, out.content);


namespace ledger {
    namespace core {
        using namespace cosmos::constants;
        namespace rpcs_parsers {

            template <class T>
            std::string getStringFromVariableKeys(const T& node, const std::list<std::string>& keys) {
                for (const auto& key : keys) {
                    if (node.HasMember(key.c_str())) {
                        return node[key.c_str()].GetString();
                    }
                }
                return "";
            }

            template <class T>
            void parseCoin(const T& node, cosmos::Coin& out) {
                assert((node.HasMember(kAmount)));
                assert((node.HasMember(kDenom)));
                out.amount = node[kAmount].GetString();
                out.denom = node[kDenom].GetString();
            }

            template <typename T>
            void parseCoinVector(const T& array, std::vector<cosmos::Coin>& out) {
                out.assign((std::size_t) array.Size(), cosmos::Coin());
                auto index = 0;
                for (const auto& n : array) {
                    parseCoin(n.GetObject(), out[index]);
                    index += 1;
                }
            }

            template <class T>
            void parseBlock(const T& node, const std::string& currencyName, Block& out) {
                out.currencyName = currencyName;
                out.hash = node[kBlockMeta].GetObject()[kBlockId].GetObject()[kHash].GetString();
                out.height = BigInt::fromString(node[kBlockMeta].GetObject()[kHeader].GetObject()[kHeight].GetString()).toUint64();
                out.time = DateUtils::fromJSON(node[kBlockMeta].GetObject()[kHeader].GetObject()[kTime].GetString());
            }

            template <typename T>
            void parseProposalContent(const T& node, cosmos::ProposalContent& out) {
                assert((node.HasMember(kType)));
                assert((node.HasMember(kDescription)));
                assert((node.HasMember(kTitle)));
                out.type = node[kType].GetString();
                out.description = node[kDescription].GetString();
                out.title = node[kTitle].GetString();
            }

            template <typename T>
            void parseFee(const T& node, cosmos::Fee& out) {
                assert((node.HasMember(kGas)));
                assert((node.HasMember(kAmount)));
                out.gas = BigInt::fromString(node[kGas].GetString());
                if (node[kAmount].IsArray()) {
                    const auto& amountArray = node[kAmount].GetArray();
                    out.amount.assign((std::size_t) amountArray.Capacity(), cosmos::Coin());
                    auto index = 0;
                    for (const auto& aNode : amountArray) {
                        parseCoin(aNode.GetObject(), out.amount[index]);
                        index += 1;
                    }
                }
            }

            template <class T>
            void parseAccount(const T& accountNode,
                    cosmos::Account& account) {
                assert((accountNode.HasMember(kValue)));
                assert((accountNode.HasMember(kType)));
                const auto& node = accountNode[kValue].GetObject();
                account.type = accountNode[kType].GetString();

                assert((node.HasMember(kAccountNumber)));
                assert((node.HasMember(kSequence)));
                assert((node.HasMember(kAddress)));
                assert((node.HasMember(kCoins)));
                account.accountNumber = node[kAccountNumber].GetString();
                account.sequence = node[kSequence].GetString();
                account.address = node[kAddress].GetString();
                const auto& balances = node[kCoins].GetArray();
                parseCoinVector(balances, account.balances);
            }

            template <typename T>
            void parseMsgSend(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgSend msg;

                assert((n.HasMember(kFromAddress)));
                assert((n.HasMember(kToAddress)));
                msg.fromAddress = n[kFromAddress].GetString();
                msg.toAddress = n[kToAddress].GetString();
                parseCoinVector(n[kAmount].GetArray(), msg.amount);
                out = msg;
            }

            template <typename T>
            void parseMsgDelegate(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgDelegate msg;

                assert((n.HasMember(kDelegatorAddress)));
                assert((n.HasMember(kValidatorAddress)));
                msg.delegatorAddress = n[kDelegatorAddress].GetString();
                msg.validatorAddress = n[kValidatorAddress].GetString();
                parseCoin(n[kAmount].GetObject(), msg.amount);
                out = msg;
            }

            template <typename T>
            void parseMsgRedelegate(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgRedelegate msg;

                msg.delegatorAddress = n[kDelegatorAddress].GetString();
                msg.validatorSourceAddress = n[kValidatorSrcAddress].GetString();
                msg.validatorDestinationAddress = n[kValidatorDstAddress].GetString();
                parseCoin(n[kAmount].GetObject(), msg.amount);
                out = msg;
            }

            template <typename T>
            void parseMsgUndelegate(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgUndelegate msg;

                msg.delegatorAddress = n[kDelegatorAddress].GetString();
                msg.validatorAddress = n[kValidatorAddress].GetString();
                parseCoin(n[kAmount].GetObject(), msg.amount);
                out = msg;
            }

            template <typename T>
            void parseMsgSubmitProposal(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgSubmitProposal msg;

                msg.proposer = n[kProposer].GetString();
                parseProposalContent(n[kContent].GetObject(), msg.content);
                parseCoinVector(n[kInitialDeposit].GetArray(), msg.initialDeposit);
                out = msg;
            }

            template <typename T>
            void parseMsgVote(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgVote msg;

                msg.voter = n[kVoter].GetString();
                msg.proposalId = n[kProposalId].GetString();
                const auto& option = n[kOption].GetString();
                if (option == "Yes") {
                    msg.option = cosmos::VoteOption::YES;
                } else if (option == "No") {
                    msg.option = cosmos::VoteOption::NO;
                } else if (option == "NoWithVeto") {
                    msg.option = cosmos::VoteOption::NOWITHVETO;
                } else if (option == "Abstain") {
                    msg.option = cosmos::VoteOption::ABSTAIN;
                }
                out = msg;
            }

            template <typename T>
            void parseMsgDeposit(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgDeposit msg;

                msg.depositor = n[kDepositor].GetString();
                msg.proposalId = n[kProposalId].GetString();
                parseCoinVector(n[kAmount].GetArray(), msg.amount);
                out = msg;
            }

            template <typename T>
            void parseMsgWithdrawDelegationReward(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgWithdrawDelegationReward msg;

                msg.delegatorAddress = n[kDelegatorAddress].GetString();
                msg.validatorAddress = n[kValidatorAddress].GetString();
                out = msg;
            }

            template <typename T>
            void parseMessage(const T& messageNode, cosmos::Message& out) {
                out.type = messageNode[kType].GetString();
                const auto& contentNode = messageNode[kValue].GetObject();
                COSMOS_PARSE_MSG_CONTENT(MsgSend)
                COSMOS_PARSE_MSG_CONTENT(MsgDelegate)
                COSMOS_PARSE_MSG_CONTENT(MsgRedelegate)
                COSMOS_PARSE_MSG_CONTENT(MsgUndelegate)
                COSMOS_PARSE_MSG_CONTENT(MsgSubmitProposal)
                COSMOS_PARSE_MSG_CONTENT(MsgVote)
                COSMOS_PARSE_MSG_CONTENT(MsgDeposit)
                COSMOS_PARSE_MSG_CONTENT(MsgWithdrawDelegationReward)
            }

            template <class T>
            void parseTransaction(const T& node,
                    cosmos::Transaction& transaction) {
                assert((node.HasMember(kTxHash)));
                transaction.hash = node[kTxHash].GetString();
                if (node.HasMember(kHeight)) {
                    Block block;
                    block.height = BigInt::fromString(node[kHeight].GetString()).toUint64();
                    transaction.block = block;
                }
                if (node.HasMember(kGasUsed)) {
                    transaction.gasUsed = Option<BigInt>(BigInt::fromString(node[kGasUsed].GetString()));
                }

                assert((node.HasMember(kLogs)));
                for (const auto& lNode : node[kLogs].GetArray()) {
                    cosmos::MessageLog log;
                    assert((lNode.HasMember(kLog)));
                    assert((lNode.HasMember(kSuccess)));
                    assert((lNode.HasMember(kMsgIndex)));
                    log.success = lNode[kSuccess].GetBool();
                    log.log = lNode[kLog].GetString();
                    log.messageIndex = BigInt::fromString(lNode[kMsgIndex].GetString()).toInt();
                    transaction.logs.emplace_back(log);
                }
                assert((node.HasMember(kTimestamp)));
                transaction.timestamp = DateUtils::fromJSON(node[kTimestamp].GetString());

                assert((node.HasMember(kTx)));
                const auto& tNode = node[kTx].GetObject();
                assert((tNode.HasMember(kValue)));
                const auto& vNode = tNode[kValue].GetObject();

                if (vNode.HasMember(kMemo)) {
                    transaction.memo = vNode[kMemo].GetString();
                }

                assert((vNode.HasMember(kMessage)));
                if (vNode[kMessage].IsArray()) {
                    const auto& msgArray = vNode[kMessage].GetArray();
                    transaction.messages.assign((std::size_t) msgArray.Capacity(), cosmos::Message());
                    auto index = 0;
                    for (const auto &mNode : vNode[kMessage].GetArray()) {
                        parseMessage(mNode, transaction.messages[index]);
                    }
                }
                assert((vNode.HasMember(kFee)));
                parseFee(vNode[kFee].GetObject(), transaction.fee);
            }

        }
    }
}

#undef COSMOS_PARSE_MSG_CONTENT
#endif //LEDGER_CORE_RPCS_PARSERS_HPP
