/*
 *
 * CosmosLikeTransactionBuilder
 *
 * Created by El Khalil Bellakrid on  14/06/2019.
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

#include <cosmos/transaction_builders/CosmosLikeTransactionBuilder.hpp>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <core/math/BigInt.hpp>
#include <core/api/enum_from_string.hpp>
#include <cosmos/CosmosLikeCurrencies.hpp>
#include <core/bytes/BytesReader.hpp>

#include <cosmos/api_impl/CosmosLikeTransactionApi.hpp>
#include <cosmos/api/CosmosLikeMessage.hpp>
#include <cosmos/CosmosLikeConstants.hpp>
#include <cosmos/api/CosmosLikeMsgType.hpp>
#include <cosmos/api/CosmosLikeMsgSend.hpp>
#include <cosmos/api/CosmosLikeMsgDelegate.hpp>
#include <cosmos/api/CosmosLikeMsgUndelegate.hpp>
#include <cosmos/api/CosmosLikeMsgRedelegate.hpp>
#include <cosmos/api/CosmosLikeMsgSubmitProposal.hpp>
#include <cosmos/api/CosmosLikeMsgVote.hpp>
#include <cosmos/api/CosmosLikeMsgDeposit.hpp>
#include <cosmos/api/CosmosLikeMsgWithdrawDelegationReward.hpp>

using namespace rapidjson;
namespace ledger {
    namespace core {

        using namespace cosmos::constants;

        using Object = GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>::Object;

        namespace {
            auto getString(const Object& object, const char *fieldName) {
                if (!object.HasMember(fieldName)) {
                    throw Exception(api::ErrorCode::INVALID_ARGUMENT, fmt::format("Error while getting {} from rawTransaction : non existing key", fieldName));
                }
                if (!object[fieldName].IsString()) {
                    throw Exception(api::ErrorCode::INVALID_ARGUMENT, fmt::format("Error while getting string {} from rawTransaction : not a string", fieldName));
                }
                return object[fieldName].GetString();
            }

            // auto getObject(const Object& object, const char *fieldName) {
            //     if (!object[fieldName].IsObject()) {
            //         throw Exception(api::ErrorCode::INVALID_ARGUMENT, fmt::format("Error while getting object {} from rawTransaction", fieldName));
            //     }
            //     return object[fieldName].GetObject();
            // }

            // TODO ?
            /*
            template <typename T>
            api::CosmosLikeMsgSend parseMsgSend(const T& n) {
                cosmos::MsgSend msg;

                msg.fromAddress = n[cosmos::constants::kFromAddress].GetString();
                msg.toAddress = n[cosmos::constants::kToAddress].GetString();
                parseCoinVector(n[cosmos::constants::kAmount].GetArray(), msg.amount);
                return msg;
            }
            */

            api::CosmosLikeMsgSend buildMsgSendFromRawMessage(Object const& object) {
                std::vector<api::CosmosLikeAmount> amounts;

                if (!object[kValue][kAmount].IsArray() && !object[kValue][kAmount].IsObject()) {
                    return {
                    "",
                    "",
                    amounts
                };
                } else if (object[kValue][kAmount].IsObject()) {
                    auto amountObject = object[kValue][kAmount].GetObject();
                    amounts.push_back(api::CosmosLikeAmount{
                        getString(amountObject, kAmount),
                        getString(amountObject, kDenom)
                    });
                    return {
                    getString(object[kValue].GetObject(), kFromAddress),
                    getString(object[kValue].GetObject(), kToAddress),
                    amounts
                };
                } else {

                    // We are sure object[kValue][kAmount] is an array here

                    // the size of the array of amount should be frequently equals to one
                    amounts.reserve(object[kValue][kAmount].GetArray().Size());

                    for (auto& amount : object[kValue][kAmount].GetArray()) {
                        if (amount.IsObject()) {
                            auto amountObject = amount.GetObject();
                            amounts.push_back(api::CosmosLikeAmount{
                                getString(amountObject, kAmount),
                                getString(amountObject, kDenom)
                            });
                        }
                    }

                    return {
                        getString(object[kValue].GetObject(), kFromAddress),
                        getString(object[kValue].GetObject(), kToAddress),
                        amounts
                    };
                }
            }

            api::CosmosLikeMsgDelegate buildMsgDelegateFromRawMessage(Object const& object) {
                auto valueObject = object[kValue].GetObject();
                auto amountObject = valueObject[kAmount].GetObject();

                return {
                    getString(valueObject, kDelegatorAddress),
                    getString(valueObject, kValidatorAddress),
                    api::CosmosLikeAmount(
                        getString(amountObject, kAmount),
                        getString(amountObject, kDenom)
                    )
                };
            }

            api::CosmosLikeMsgUndelegate buildMsgUndelegateFromRawMessage(Object const& object) {
                auto valueObject = object[kValue].GetObject();
                auto amountObject = valueObject[kAmount].GetObject();

                return {
                    getString(valueObject, kDelegatorAddress),
                    getString(valueObject, kValidatorAddress),
                    api::CosmosLikeAmount(
                        getString(amountObject, kAmount),
                        getString(amountObject, kDenom)
                    )
                };
            }

             api::CosmosLikeMsgRedelegate buildMsgRedelegateFromRawMessage(Object const& object) {
                auto valueObject = object[kValue].GetObject();
                auto amountObject = valueObject[kAmount].GetObject();

                return {
                    getString(valueObject, kDelegatorAddress),
                    getString(valueObject, kValidatorSrcAddress),
                    getString(valueObject, kValidatorDstAddress),
                    api::CosmosLikeAmount(
                        getString(amountObject, kAmount),
                        getString(amountObject, kDenom)
                    )
                };
            }

             api::CosmosLikeMsgSubmitProposal buildMsgSubmitProposalFromRawMessage(Object const& object) {
                auto valueObject = object[kValue].GetObject();
                auto contentObject = valueObject[kContent].GetObject();
                auto initialDepositArray = valueObject[kInitialDeposit].GetArray();

                auto content = api::CosmosLikeContent(
                    getString(contentObject, kType),
                    getString(contentObject, kTitle),
                    getString(contentObject, kDescription)
                );

                std::vector<api::CosmosLikeAmount> amounts;
                // the size of the array of amounts should be frequently equal to one
                amounts.reserve(initialDepositArray.Size());
                for (auto& amount : initialDepositArray) {
                    auto amountObject = amount.GetObject();
                    amounts.push_back(api::CosmosLikeAmount(
                        getString(amountObject, kAmount),
                        getString(amountObject, kDenom)
                    ));
                }

                return {
                    content,
                    getString(valueObject, kProposer),
                    amounts
                };
            }

            api::CosmosLikeMsgVote buildMsgVoteFromRawMessage(Object const& object) {
                auto valueObject = object[kValue].GetObject();
                return {
                    getString(valueObject, kVoter),
                    getString(valueObject, kProposalId),
                    api::from_string<api::CosmosLikeVoteOption>(getString(valueObject, kOption))
                };
            }

            api::CosmosLikeMsgDeposit buildMsgDepositFromRawMessage(Object const& object) {
                auto valueObject = object[kValue].GetObject();
                std::vector<api::CosmosLikeAmount> amounts;

                if (valueObject[kAmount].IsArray()) {
                    // the size of the array of amount should be frequently equals to one
                    amounts.reserve(valueObject[kAmount].GetArray().Size());

                    for (auto& amount : valueObject[kAmount].GetArray()) {
                        if (amount.IsObject()) {
                            auto amountObject = amount.GetObject();

                            amounts.push_back(api::CosmosLikeAmount{
                                getString(amountObject, kAmount),
                                getString(amountObject, kDenom)
                            });
                        }
                    }
                }

                return {
                    getString(valueObject, kDepositor),
                    getString(valueObject, kProposalId),
                    amounts
               };
            }

            api::CosmosLikeMsgWithdrawDelegationReward buildMsgWithdrawDelegatationRewardFromRawMessage(Object const& object) {
                auto valueObject = object[kValue].GetObject();
                return {
                    getString(valueObject, kDelegatorAddress),
                    getString(valueObject, kValidatorAddress)
                };
            }
        }

        CosmosLikeTransactionBuilder::CosmosLikeTransactionBuilder(
                const std::shared_ptr<api::ExecutionContext> &context,
                const api::Currency &currency,
                const std::shared_ptr<CosmosLikeBlockchainExplorer> &explorer,
                const std::shared_ptr<spdlog::logger> &logger,
                const CosmosLikeTransactionBuildFunction &buildFunction) {
            _context = context;
            _currency = currency;
            _explorer = explorer;
            _build = buildFunction;
            _logger = logger;
            _request.wipe = false;
        }

        CosmosLikeTransactionBuilder::CosmosLikeTransactionBuilder(const CosmosLikeTransactionBuilder &cpy) {
            _currency = cpy._currency;
            _build = cpy._build;
            _request = cpy._request;
            _context = cpy._context;
            _logger = cpy._logger;
        }

         std::shared_ptr<api::CosmosLikeTransactionBuilder>  CosmosLikeTransactionBuilder::setSequence(const std::string & sequence) {
            _request.sequence = sequence;
            return shared_from_this();
        }

        std::shared_ptr<api::CosmosLikeTransactionBuilder>  CosmosLikeTransactionBuilder::setMemo(const std::string & memo) {
            _request.memo = memo;
            return shared_from_this();
        }

        std::shared_ptr<api::CosmosLikeTransactionBuilder>  CosmosLikeTransactionBuilder::setGas(const std::shared_ptr<api::Amount> & gas) {
            _request.gas = std::make_shared<BigInt>(gas->toString());
            return shared_from_this();
        }

        std::shared_ptr<api::CosmosLikeTransactionBuilder>  CosmosLikeTransactionBuilder::setFee(const std::shared_ptr<api::Amount> & fee) {
            _request.fee = std::make_shared<BigInt>(fee->toString());
            return shared_from_this();
        }

        std::shared_ptr<api::CosmosLikeTransactionBuilder> CosmosLikeTransactionBuilder::addMessage(const std::shared_ptr<api::CosmosLikeMessage> & message) {
            _request.messages.push_back(message);
            return shared_from_this();
        }

        void CosmosLikeTransactionBuilder::build(const std::shared_ptr<api::CosmosLikeTransactionCallback> &callback) {
            build().callback(_context, callback);
        }

        Future<std::shared_ptr<api::CosmosLikeTransaction>> CosmosLikeTransactionBuilder::build() {
            return _build(_request, _explorer);
        }

        std::shared_ptr<api::CosmosLikeTransactionBuilder> CosmosLikeTransactionBuilder::clone() {
            return std::make_shared<CosmosLikeTransactionBuilder>(*this);
        }

        void CosmosLikeTransactionBuilder::reset() {
            _request = CosmosLikeTransactionBuildRequest();
        }

        std::shared_ptr<api::CosmosLikeTransaction>
        api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(const api::Currency &currency,
                                                                       const std::string &rawTransaction) {
            return ::ledger::core::CosmosLikeTransactionBuilder::parseRawTransaction(currency, rawTransaction, false);
        }

        std::shared_ptr<api::CosmosLikeTransaction>
        api::CosmosLikeTransactionBuilder::parseRawSignedTransaction(const api::Currency &currency,
                                                                     const std::string &rawTransaction) {
            return ::ledger::core::CosmosLikeTransactionBuilder::parseRawTransaction(currency, rawTransaction, true);
        }

        std::shared_ptr<api::CosmosLikeTransaction>
        CosmosLikeTransactionBuilder::parseRawTransaction(const api::Currency &currency,
                                                          const std::string &rawTransaction,
                                                          bool isSigned) {
            auto tx = std::make_shared<CosmosLikeTransactionApi>(currency);
            Document document;
            document.Parse(rawTransaction.c_str());

            auto getAmount = [=] (const Object &object) -> Amount {
                auto denom = getString(object, kDenom);
                auto amount = getString(object, kAmount);

                auto unit = std::find_if(currency.units.begin(), currency.units.end(), [&] (const api::CurrencyUnit &unit) {
                    return unit.name == denom;
                });
                if (unit == currency.units.end()) {
                    throw Exception(api::ErrorCode::INVALID_ARGUMENT, "Unknown unit while parsing transaction");
                }
                //TODO: Fix Amount::toUnit
                return Amount(currency, 0, BigInt(amount) * BigInt(10).powu(static_cast<unsigned short>((*unit).numberOfDecimal)));
            };

            tx->setAccountNumber(getString(document.GetObject(), kAccountNumber));
            tx->setMemo(getString(document.GetObject(), kMemo));
            tx->setSequence(getString(document.GetObject(), kSequence));

            //Get fees
            if (document[kFee].IsObject()) {
                auto feeObject = document[kFee].GetObject();
                // Gas Limit
                auto gas = std::make_shared<BigInt>(getString(feeObject, kGas));
                tx->setGas(gas);

                // Total Tx fees
                // Gas Price is then deduced with Total_Tx_fees / Gas Limit
                // TODO figure out why the fee contains an array of amounts
                if (feeObject[kAmount].IsArray()) {
                    auto fee = BigInt();

                    // accumlate all types of fee
                    for (auto& amount : feeObject[kAmount].GetArray()) {
                        if (amount.IsObject()) {
                            fee = fee +  BigInt(getAmount(amount.GetObject()).toString());
                        }
                    }

                    tx->setFee(std::make_shared<BigInt>(fee));
                }
            }

            // Msgs object
            if (document[kMessages].IsArray()) {
                std::vector<std::shared_ptr<api::CosmosLikeMessage>> messages;

                messages.reserve(document[kMessages].GetArray().Size());

                for (auto& msg: document[kMessages].GetArray()) {
                    if (msg.IsObject()) {
                        auto msgObject = msg.GetObject();

                        switch (cosmos::stringToMsgType(getString(msgObject, kType))) {
                            case api::CosmosLikeMsgType::MSGSEND:
                                messages.push_back(api::CosmosLikeMessage::wrapMsgSend(
                                    buildMsgSendFromRawMessage(msgObject)));
                                break;
                            case api::CosmosLikeMsgType::MSGDELEGATE:
                                messages.push_back(api::CosmosLikeMessage::wrapMsgDelegate(
                                    buildMsgDelegateFromRawMessage(msgObject)));
                                break;
                            case api::CosmosLikeMsgType::MSGUNDELEGATE:
                                messages.push_back(api::CosmosLikeMessage::wrapMsgUndelegate(
                                    buildMsgUndelegateFromRawMessage(msgObject)));
                                break;
                            case api::CosmosLikeMsgType::MSGREDELEGATE:
                                messages.push_back(api::CosmosLikeMessage::wrapMsgRedelegate(
                                    buildMsgRedelegateFromRawMessage(msgObject)));
                                break;
                            case api::CosmosLikeMsgType::MSGSUBMITPROPOSAL:
                                messages.push_back(api::CosmosLikeMessage::wrapMsgSubmitProposal(
                                    buildMsgSubmitProposalFromRawMessage(msgObject)));
                                break;
                            case api::CosmosLikeMsgType::MSGVOTE:
                                messages.push_back(api::CosmosLikeMessage::wrapMsgVote(
                                    buildMsgVoteFromRawMessage(msgObject)));
                                break;
                            case api::CosmosLikeMsgType::MSGDEPOSIT:
                                messages.push_back(api::CosmosLikeMessage::wrapMsgDeposit(
                                    buildMsgDepositFromRawMessage(msgObject)));
                                break;
                            case api::CosmosLikeMsgType::MSGWITHDRAWDELEGATIONREWARD:
                                messages.push_back(api::CosmosLikeMessage::wrapMsgWithdrawDelegationReward(
                                    buildMsgWithdrawDelegatationRewardFromRawMessage(msgObject)));
                                break;
                            default:
                                throw Exception(
                                    api::ErrorCode::INVALID_ARGUMENT,
                                    fmt::format("unknown message {} while parsing transaction", getString(msgObject, kType)));
                                break;
                        }
                    }
                }

                tx->setMessages(messages);
            }

            return tx;
        }
    }
}
