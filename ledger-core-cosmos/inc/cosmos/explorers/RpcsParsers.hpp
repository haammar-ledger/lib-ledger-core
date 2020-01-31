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

#include <rapidjson/document.h>

#include <core/utils/Option.hpp>
#include <core/utils/DateUtils.hpp>

#include <cosmos/explorers/CosmosLikeBlockchainExplorer.hpp>
#include <cosmos/CosmosLikeConstants.hpp>
#include <cosmos/CosmosLikeCurrencies.hpp>

#define COSMOS_PARSE_MSG_CONTENT(MsgType) if (out.type == "cosmos-sdk/"#MsgType) return parse##MsgType(contentNode, out.content);


namespace ledger {
    namespace core {
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
                out.amount = node["amount"].GetString();
                out.denom = node["denom"].GetString();
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
                out.hash = node["block_meta"].GetObject()["block_id"].GetObject()["hash"].GetString();
                out.height = BigInt::fromString(node["block_meta"].GetObject()["header"].GetObject()["height"].GetString()).toUint64();
                out.time = DateUtils::fromJSON(node["block_meta"].GetObject()["header"].GetObject()["time"].GetString());
            }

            template <typename T>
            void parseProposalContent(const T& node, cosmos::ProposalContent& out) {
                out.type = node[cosmos::constants::kType].GetString();
                out.description = node[cosmos::constants::kDescription].GetString();
                out.title = node[cosmos::constants::kTitle].GetString();
            }

            template <typename T>
            void parseFee(const T& node, cosmos::Fee& out) {
                out.gas = BigInt::fromString(node["gas"].GetString());
                if (node["amount"].IsArray()) {
                    const auto& amountArray = node["amount"].GetArray();
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
                const auto& node = accountNode[cosmos::constants::kValue].GetObject();
                account.accountNumber = node[cosmos::constants::kAccountNumber].GetString();
                account.sequence = node[cosmos::constants::kSequence].GetString();
                account.address = node[cosmos::constants::kAddress].GetString();
                account.type = accountNode["type"].GetString();
                const auto& balances = node["coins"].GetArray();
                parseCoinVector(balances, account.balances);
            }

            template <typename T>
            void parseMsgSend(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgSend msg;

                msg.fromAddress = n[cosmos::constants::kFromAddress].GetString();
                msg.toAddress = n[cosmos::constants::kToAddress].GetString();
                parseCoinVector(n[cosmos::constants::kAmount].GetArray(), msg.amount);
                out = msg;
            }

            template <typename T>
            void parseMsgDelegate(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgDelegate msg;

                msg.delegatorAddress = n[cosmos::constants::kDelegatorAddress].GetString();
                msg.validatorAddress = n[cosmos::constants::kValidatorAddress].GetString();
                parseCoin(n[cosmos::constants::kAmount].GetObject(), msg.amount);
                out = msg;
            }

            template <typename T>
            void parseMsgRedelegate(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgRedelegate msg;

                msg.delegatorAddress = n[cosmos::constants::kDelegatorAddress].GetString();
                msg.validatorSourceAddress = n[cosmos::constants::kValidatorSrcAddress].GetString();
                msg.validatorDestinationAddress = n[cosmos::constants::kValidatorDstAddress].GetString();
                parseCoin(n[cosmos::constants::kAmount].GetObject(), msg.amount);
                out = msg;
            }

            template <typename T>
            void parseMsgUndelegate(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgUndelegate msg;

                msg.delegatorAddress = n[cosmos::constants::kDelegatorAddress].GetString();
                msg.validatorAddress = n[cosmos::constants::kValidatorAddress].GetString();
                parseCoin(n[cosmos::constants::kAmount].GetObject(), msg.amount);
                out = msg;
            }

            template <typename T>
            void parseMsgSubmitProposal(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgSubmitProposal msg;

                msg.proposer = n[cosmos::constants::kProposer].GetString();
                parseProposalContent(n[cosmos::constants::kContent].GetObject(), msg.content);
                parseCoinVector(n[cosmos::constants::kInitialDeposit].GetArray(), msg.initialDeposit);
                out = msg;
            }

            template <typename T>
            void parseMsgVote(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgVote msg;

                msg.voter = n[cosmos::constants::kVoter].GetString();
                msg.proposalId = n[cosmos::constants::kProposalId].GetString();
                const auto& option = n[cosmos::constants::kOption].GetString();
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

                msg.depositor = n[cosmos::constants::kDepositor].GetString();
                msg.proposalId = n[cosmos::constants::kProposalId].GetString();
                parseCoinVector(n[cosmos::constants::kAmount].GetArray(), msg.amount);
                out = msg;
            }

            template <typename T>
            void parseMsgWithdrawDelegationReward(const T& n, cosmos::MessageContent &out) {
                cosmos::MsgWithdrawDelegationReward msg;

                msg.delegatorAddress = n[cosmos::constants::kDelegatorAddress].GetString();
                msg.validatorAddress = n[cosmos::constants::kValidatorAddress].GetString();
                out = msg;
            }

            template <typename T>
            void parseMessage(const T& messageNode, cosmos::Message& out) {
                out.type = messageNode["type"].GetString();
                const auto& contentNode = messageNode["value"].GetObject();
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
                transaction.hash = node["txhash"].GetString();
                if (node.HasMember("height")) {
                    Block block;
                    block.height = BigInt::fromString(node["height"].GetString()).toUint64();
                    transaction.block = block;
                }
                if (node.HasMember("gas_used"))
                    transaction.gasUsed = Option<BigInt>(BigInt::fromString(node["gas_used"].GetString()));
				for (const auto& lNode : node["logs"].GetArray()) {
                    cosmos::MessageLog log;
                    log.success = lNode["success"].GetBool();
                    log.log = lNode["log"].GetString();
                    log.messageIndex = BigInt::fromString(lNode["msg_index"].GetString()).toInt();
                    transaction.logs.emplace_back(log);
                }
                transaction.timestamp = DateUtils::fromJSON(node["timestamp"].GetString());

                const auto& tNode = node["tx"].GetObject();
                const auto& vNode = tNode["value"].GetObject();

                if (vNode.HasMember("memo")) {
                    transaction.memo = vNode["memo"].GetString();
                }

                if (vNode["msg"].IsArray()) {
                    const auto& msgArray = vNode["msg"].GetArray();
                    transaction.messages.assign((std::size_t) msgArray.Capacity(), cosmos::Message());
                    auto index = 0;
                    for (const auto &mNode : vNode["msg"].GetArray()) {
                        parseMessage(mNode, transaction.messages[index]);
                    }
                }
                parseFee(vNode["fee"].GetObject(), transaction.fee);
            }

        }
    }
}

#undef COSMOS_PARSE_MSG_CONTENT
#endif //LEDGER_CORE_RPCS_PARSERS_HPP
