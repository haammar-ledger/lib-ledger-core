/*
 *
 * CosmosLikeMessage
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

#include <cosmos/CosmosLikeMessage.hpp>

#include <algorithm>
#include <functional>

#include <fmt/format.h>

#include <core/api/ErrorCode.hpp>
#include <core/api/enum_from_string.hpp>
#include <core/collections/DynamicArray.hpp>

#include <cosmos/CosmosLikeConstants.hpp>

namespace ledger {
	namespace core {

		using namespace cosmos::constants;
		namespace {
			// a closure helper that check if a `DynamicObject` holds a specific key
			inline auto containsKeys(const std::shared_ptr<api::DynamicObject>& object) {
				return [=](auto const& key) {
					return object->contains(key);
				};
			}

			// add a `CosmosLikeAmount` to a `DynamicObject`
			inline auto addAmount(const std::shared_ptr<api::DynamicObject>& object, api::CosmosLikeAmount const& amount) {
				auto amountObject = std::make_shared<::ledger::core::DynamicObject>();

				amountObject->putString(kAmount, amount.amount);
				amountObject->putString(kDenom, amount.denom);

				object->putObject(kAmount, amountObject);
			}

			// add a `CosmosLikeAmount` to a `DynamicArray`
			inline auto addAmount(const std::shared_ptr<api::DynamicArray>& array, api::CosmosLikeAmount const& amount) {
				auto amountObject = std::make_shared<::ledger::core::DynamicObject>();

				amountObject->putString(kAmount, amount.amount);
				amountObject->putString(kDenom, amount.denom);

				array->pushObject(amountObject);
			}
		}

                CosmosLikeMessage::CosmosLikeMessage(const std::shared_ptr<DynamicObject>& content)
                    : _content(content) {}

                cosmos::Message CosmosLikeMessage::toRawMessage() const {
			auto msgType = getRawMessageType();
			cosmos::Message raw_message;
			raw_message.type = msgType;

                        if (raw_message.type == kMsgSend) {
                            raw_message.content =
                                unwrapMsgSend(std::make_shared<CosmosLikeMessage>(*this));
                        } else if (raw_message.type == kMsgDelegate) {
                            raw_message.content =
                                unwrapMsgDelegate(std::make_shared<CosmosLikeMessage>(*this));
                        } else if (raw_message.type == kMsgUndelegate) {
                            raw_message.content =
                                unwrapMsgUndelegate(std::make_shared<CosmosLikeMessage>(*this));
                        } else if (raw_message.type == kMsgRedelegate) {
                            raw_message.content =
                                unwrapMsgRedelegate(std::make_shared<CosmosLikeMessage>(*this));
                        } else if (raw_message.type == kMsgSubmitProposal) {
                            raw_message.content =
                                unwrapMsgSubmitProposal(std::make_shared<CosmosLikeMessage>(*this));
                        } else if (raw_message.type == kMsgVote) {
                            raw_message.content =
                                unwrapMsgVote(std::make_shared<CosmosLikeMessage>(*this));
                        } else if (raw_message.type == kMsgDeposit) {
                            raw_message.content =
                                unwrapMsgDeposit(std::make_shared<CosmosLikeMessage>(*this));
                        } else if (raw_message.type == kMsgWithdrawDelegationReward) {
                            raw_message.content = unwrapMsgWithdrawDelegationReward(
                                std::make_shared<CosmosLikeMessage>(*this));
                        }  // else the content stays empty
                        return raw_message;
                }

                CosmosLikeMessage::CosmosLikeMessage(const cosmos::Message& rawStruct) : _content() {
			auto msgType = rawStruct.type;
			std::shared_ptr<DynamicObject> content = nullptr;

                        if (msgType == kMsgSend) {
                            content = std::dynamic_pointer_cast<CosmosLikeMessage>(
                                          CosmosLikeMessage::wrapMsgSend(
                                              boost::get<cosmos::MsgSend>(rawStruct.content)))
                                          ->_content;
                        } else if (msgType == kMsgDelegate) {
                            content = std::dynamic_pointer_cast<CosmosLikeMessage>(
                                          CosmosLikeMessage::wrapMsgDelegate(
                                              boost::get<cosmos::MsgDelegate>(rawStruct.content)))
                                          ->_content;
                        } else if (msgType == kMsgUndelegate) {
                            content = std::dynamic_pointer_cast<CosmosLikeMessage>(
                                          CosmosLikeMessage::wrapMsgUndelegate(
                                              boost::get<cosmos::MsgUndelegate>(rawStruct.content)))
                                          ->_content;
                        } else if (msgType == kMsgRedelegate) {
                            content = std::dynamic_pointer_cast<CosmosLikeMessage>(
                                          CosmosLikeMessage::wrapMsgRedelegate(
                                              boost::get<cosmos::MsgRedelegate>(rawStruct.content)))
                                          ->_content;
                        } else if (msgType == kMsgSubmitProposal) {
                            content =
                                std::dynamic_pointer_cast<CosmosLikeMessage>(
                                    CosmosLikeMessage::wrapMsgSubmitProposal(
                                        boost::get<cosmos::MsgSubmitProposal>(rawStruct.content)))
                                    ->_content;
                        } else if (msgType == kMsgVote) {
                            content = std::dynamic_pointer_cast<CosmosLikeMessage>(
                                          CosmosLikeMessage::wrapMsgVote(
                                              boost::get<cosmos::MsgVote>(rawStruct.content)))
                                          ->_content;
                        } else if (msgType == kMsgDeposit) {
                            content = std::dynamic_pointer_cast<CosmosLikeMessage>(
                                          CosmosLikeMessage::wrapMsgDeposit(
                                              boost::get<cosmos::MsgDeposit>(rawStruct.content)))
                                          ->_content;
                        } else if (msgType == kMsgWithdrawDelegationReward) {
                            content = std::dynamic_pointer_cast<CosmosLikeMessage>(
                                          CosmosLikeMessage::wrapMsgWithdrawDelegationReward(
                                              boost::get<cosmos::MsgWithdrawDelegationReward>(
                                                  rawStruct.content)))
                                          ->_content;
                        }  // else the content stays nullptr

                        _content = content;
                }

		api::CosmosLikeMsgType CosmosLikeMessage::getMessageType() const {
			auto msgType = getRawMessageType();

			if (msgType == kMsgSend) {
				return api::CosmosLikeMsgType::MSGSEND;
			} else if (msgType == kMsgDelegate) {
				return api::CosmosLikeMsgType::MSGDELEGATE;
			} else if (msgType == kMsgUndelegate) {
				return api::CosmosLikeMsgType::MSGUNDELEGATE;
			} else if (msgType == kMsgRedelegate) {
				return api::CosmosLikeMsgType::MSGREDELEGATE;
			} else if (msgType == kMsgSubmitProposal) {
				return api::CosmosLikeMsgType::MSGSUBMITPROPOSAL;
			} else if (msgType == kMsgVote) {
				return api::CosmosLikeMsgType::MSGVOTE;
			} else if (msgType == kMsgDeposit) {
				return api::CosmosLikeMsgType::MSGDEPOSIT;
			} else if (msgType == kMsgWithdrawDelegationReward) {
				return api::CosmosLikeMsgType::MSGWITHDRAWDELEGATIONREWARD;
			}  else {
				return api::CosmosLikeMsgType::UNKNOWN;
			}
		}

		std::string CosmosLikeMessage::getRawMessageType() const {
			return _content->get<std::string>(kType).value_or("");
		}

		rapidjson::Value CosmosLikeMessage::toJson(rapidjson::Document::AllocatorType& allocator) const {
			// Note : this Json has not been sorted yet
			// This doesn't follow the spec
                        // https://github.com/cosmos/ledger-cosmos-app/blob/master/docs/TXSPEC.md
                        // but we defer the sorting to the TransactionApi::serialize() method
			return _content->toJson(allocator);
		}

		std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgSend(const api::CosmosLikeMsgSend & msg) {
			auto object = std::make_shared<::ledger::core::DynamicObject>();
			auto value = std::make_shared<::ledger::core::DynamicObject>();
			auto amounts = std::make_shared<::ledger::core::DynamicArray>();

			object->putString(kType, kMsgSend);
			object->putObject(kValue, value);

			value->putString(kFromAddress, msg.fromAddress);
			value->putString(kToAddress, msg.toAddress);
			value->putArray(kAmount, amounts);

			std::for_each(std::cbegin(msg.amount), std::cend(msg.amount), [&amounts](auto const& amount) {
				addAmount(amounts, amount);
			});

			return std::make_shared<::ledger::core::CosmosLikeMessage>(object);
		}

    	api::CosmosLikeMsgSend api::CosmosLikeMessage::unwrapMsgSend(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto implMsg = std::dynamic_pointer_cast<::ledger::core::CosmosLikeMessage>(msg);

			if (msg->getMessageType() != api::CosmosLikeMsgType::MSGSEND
				|| !implMsg->_content->contains(kValue)) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgSend");
			}

			auto underlyingMsg = api::CosmosLikeMsgSend{};
			auto value = std::dynamic_pointer_cast<::ledger::core::DynamicObject>(
				implMsg->_content->getObject(kValue));

			{
				static const char* keys[] = { kFromAddress, kToAddress, kAmount };

				if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(value))) {
					throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgSend: bad value fields");
				}

				underlyingMsg.fromAddress = value->getString(kFromAddress).value();
				underlyingMsg.toAddress = value->getString(kToAddress).value();
			}

			{
				static const char* keys[] = { kAmount, kDenom };

				auto amounts = std::dynamic_pointer_cast<::ledger::core::DynamicArray>(value->getArray(kAmount));

				for (auto size = amounts->size(), i = decltype(size){0}; i < size; ++i) {
					auto amount = amounts->getObject(i);

					if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(amount))) {
						throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgSend: bad amount fields");
					}

					underlyingMsg.amount.push_back(CosmosLikeAmount{
						amount->getString(kAmount).value(),
						amount->getString(kDenom).value()
					});
				}
			}

			return underlyingMsg;
		}

    	std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgDelegate(const api::CosmosLikeMsgDelegate & msg) {
			auto object = std::make_shared<::ledger::core::DynamicObject>();
			auto value = std::make_shared<::ledger::core::DynamicObject>();

			object->putString(kType, kMsgDelegate);
			object->putObject(kValue, value);

			value->putString(kDelegatorAddress, msg.delegatorAddress);
			value->putString(kValidatorAddress, msg.validatorAddress);

			addAmount(value, msg.amount);

			return std::make_shared<::ledger::core::CosmosLikeMessage>(object);
		}

    	api::CosmosLikeMsgDelegate api::CosmosLikeMessage::unwrapMsgDelegate(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto implMsg = std::dynamic_pointer_cast<::ledger::core::CosmosLikeMessage>(msg);

			if (msg->getMessageType() != api::CosmosLikeMsgType::MSGDELEGATE
				|| !implMsg->_content->contains(kValue)) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgDelegate");
			}

			auto underlyingMsg = api::CosmosLikeMsgDelegate{};
			auto value = std::dynamic_pointer_cast<::ledger::core::DynamicObject>(implMsg->_content->getObject(kValue));

			{
				static const char* keys[] = { kDelegatorAddress, kValidatorAddress, kAmount };

				if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(value))) {
					throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgDelegate: bad value fields");
				}

				underlyingMsg.delegatorAddress = value->getString(kDelegatorAddress).value();
				underlyingMsg.validatorAddress = value->getString(kValidatorAddress).value();
			}

			{
				static const char* keys[] = { kAmount, kDenom };

				auto amount = std::dynamic_pointer_cast<::ledger::core::DynamicObject>(value->getObject(kAmount));

				if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(amount))) {
					throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgDelegate: bad amount fields");
				}

				underlyingMsg.amount = {
					amount->getString(kAmount).value(),
					amount->getString(kDenom).value()
				};
			}

			return underlyingMsg;
		}

    	std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgUndelegate(const api::CosmosLikeMsgUndelegate & msg) {
			auto object = std::make_shared<::ledger::core::DynamicObject>();
			auto value = std::make_shared<::ledger::core::DynamicObject>();

			object->putString(kType, kMsgUndelegate);
			object->putObject(kValue, value);

			value->putString(kDelegatorAddress, msg.delegatorAddress);
			value->putString(kValidatorAddress, msg.validatorAddress);

			addAmount(value, msg.amount);

			return std::make_shared<::ledger::core::CosmosLikeMessage>(object);
		}

    	api::CosmosLikeMsgUndelegate api::CosmosLikeMessage::unwrapMsgUndelegate(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto implMsg = std::dynamic_pointer_cast<::ledger::core::CosmosLikeMessage>(msg);

			if (msg->getMessageType() != api::CosmosLikeMsgType::MSGUNDELEGATE
				|| !implMsg->_content->contains(kValue)) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgUndelegate");
			}

			auto underlyingMsg = api::CosmosLikeMsgUndelegate{};
			auto value = std::dynamic_pointer_cast<::ledger::core::DynamicObject>(implMsg->_content->getObject(kValue));

			{
				static const char* keys[] = { kDelegatorAddress, kValidatorAddress, kAmount };

				if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(value))) {
					throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgUnDelegate: bad value fields");
				}

				underlyingMsg.delegatorAddress = value->getString(kDelegatorAddress).value();
				underlyingMsg.validatorAddress = value->getString(kValidatorAddress).value();
			}

			{
				static const char* keys[] = { kAmount, kDenom };

				auto amount = std::dynamic_pointer_cast<::ledger::core::DynamicObject>(value->getObject(kAmount));

				if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(amount))) {
					throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgUnDelegate: bad amount fields");
				}

				underlyingMsg.amount = {
					amount->getString(kAmount).value(),
					amount->getString(kDenom).value()
				};
			}

			return underlyingMsg;
		}

    	std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgRedelegate(const api::CosmosLikeMsgRedelegate & msg) {
			auto object = std::make_shared<::ledger::core::DynamicObject>();
			auto value = std::make_shared<::ledger::core::DynamicObject>();

			object->putString(kType, kMsgRedelegate);
			object->putObject(kValue, value);

			value->putString(kDelegatorAddress, msg.delegatorAddress);
			value->putString(kValidatorSrcAddress, msg.validatorSourceAddress);
			value->putString(kValidatorDstAddress, msg.validatorDestinationAddress);

			addAmount(value, msg.amount);

			return std::make_shared<::ledger::core::CosmosLikeMessage>(object);
		}

    	api::CosmosLikeMsgRedelegate api::CosmosLikeMessage::unwrapMsgRedelegate(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto implMsg = std::dynamic_pointer_cast<::ledger::core::CosmosLikeMessage>(msg);

			if (msg->getMessageType() != api::CosmosLikeMsgType::MSGREDELEGATE
				|| !implMsg->_content->contains(kValue)) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgRedelegate");
			}

			auto underlyingMsg = api::CosmosLikeMsgRedelegate{};
			auto value = std::dynamic_pointer_cast<::ledger::core::DynamicObject>(implMsg->_content->getObject(kValue));

			{
				static const char* keys[] = { kDelegatorAddress, kValidatorSrcAddress, kValidatorDstAddress, kAmount };

				if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(value))) {
					throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgRedelegate: bad value fields");
				}

				underlyingMsg.delegatorAddress = value->getString(kDelegatorAddress).value();
				underlyingMsg.validatorSourceAddress = value->getString(kValidatorSrcAddress).value();
				underlyingMsg.validatorDestinationAddress = value->getString(kValidatorDstAddress).value();
			}

			{
				static const char* keys[] = { kAmount, kDenom };

				auto amount = std::dynamic_pointer_cast<::ledger::core::DynamicObject>(value->getObject(kAmount));

				if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(amount))) {
					throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgRedelegate: bad amount fields");
				}

				underlyingMsg.amount = {
					amount->getString(kAmount).value(),
					amount->getString(kDenom).value()
				};
			}

			return underlyingMsg;
		}

		std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgSubmitProposal(const api::CosmosLikeMsgSubmitProposal & msg) {
			auto object = std::make_shared<::ledger::core::DynamicObject>();
			auto value = std::make_shared<::ledger::core::DynamicObject>();
			auto amounts = std::make_shared<::ledger::core::DynamicArray>();
			auto content = std::make_shared<::ledger::core::DynamicObject>();
			auto contentValue = std::make_shared<::ledger::core::DynamicObject>();

			object->putString(kType, kMsgSubmitProposal);
			object->putObject(kValue, value);

			value->putObject(kContent, content);
			value->putString(kProposer, msg.proposer);
			value->putArray(kInitialDeposit, amounts);

			std::for_each(std::cbegin(msg.initialDeposit), std::cend(msg.initialDeposit), [&amounts](auto const& amount) {
				addAmount(amounts, amount);
			});

			content->putString(kType, msg.content.type);
			content->putString(kTitle, msg.content.title);
			content->putString(kDescription, msg.content.description);

			return std::make_shared<::ledger::core::CosmosLikeMessage>(object);
		}

    	api::CosmosLikeMsgSubmitProposal api::CosmosLikeMessage::unwrapMsgSubmitProposal(const std::shared_ptr<api::CosmosLikeMessage> & msg) {

			auto implMsg = std::dynamic_pointer_cast<::ledger::core::CosmosLikeMessage>(msg);

			if (msg->getMessageType() != api::CosmosLikeMsgType::MSGSUBMITPROPOSAL
				|| !implMsg->_content->contains(kValue)) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgSubmitProposal");
			}

			auto underlyingMsg = api::CosmosLikeMsgSubmitProposal{};
			auto value = std::dynamic_pointer_cast<::ledger::core::DynamicObject>(implMsg->_content->getObject(kValue));
			{
				static const char* keys[] = { kContent, kProposer, kInitialDeposit };

				if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(value))) {
					throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgSubmitProposal: bad value fields");
				}

				underlyingMsg.proposer = value->getString(kProposer).value();
			}

			auto content = std::dynamic_pointer_cast<::ledger::core::DynamicObject>(value->getObject(kContent));

			{
				static const char* keys[] = { kType, kTitle, kDescription };

				if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(content))) {
					throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgSubmitProposal: bad content fields");
				}

				underlyingMsg.content.type = content->getString(kType).value();
				underlyingMsg.content.title = content->getString(kTitle).value();
				underlyingMsg.content.description = content->getString(kDescription).value();
			}

			{
				static const char* keys[] = { kAmount, kDenom };

				auto amounts = std::dynamic_pointer_cast<::ledger::core::DynamicArray>(value->getArray(kInitialDeposit));

				for (auto size = amounts->size(), i = decltype(size){0}; i < size; ++i) {
					auto amount = amounts->getObject(i);

					if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(amount))) {
						throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgSubmitProposal: bad amount fields");
					}

					underlyingMsg.initialDeposit.push_back(CosmosLikeAmount{
						amount->getString(kAmount).value(),
						amount->getString(kDenom).value()
					});
				}
			}

			return underlyingMsg;
		}


		std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgVote(const api::CosmosLikeMsgVote & msg) {
			auto object = std::make_shared<::ledger::core::DynamicObject>();
			auto value = std::make_shared<::ledger::core::DynamicObject>();

			object->putString(kType, kMsgVote);
			object->putObject(kValue, value);

			value->putString(kVoter, msg.voter);
			value->putString(kProposalId, msg.proposalId);
			value->putString(kOption, api::to_string(msg.option));

			return std::make_shared<::ledger::core::CosmosLikeMessage>(object);
		}


		api::CosmosLikeMsgVote api::CosmosLikeMessage::unwrapMsgVote(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto implMsg = std::dynamic_pointer_cast<::ledger::core::CosmosLikeMessage>(msg);

			if (msg->getMessageType() != api::CosmosLikeMsgType::MSGVOTE
				|| !implMsg->_content->contains(kValue)) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgVote");
			}

			auto underlyingMsg = api::CosmosLikeMsgVote{};
			auto value = std::dynamic_pointer_cast<::ledger::core::DynamicObject>(implMsg->_content->getObject(kValue));

			{
				static const char* keys[] = { kVoter, kProposalId, kOption };

				if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(value))) {
					throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgVote: bad value fields");
				}

				underlyingMsg.voter = value->getString(kVoter).value();
				underlyingMsg.proposalId = value->getString(kProposalId).value();
				underlyingMsg.option = api::from_string<api::CosmosLikeVoteOption>(value->getString(kOption).value());
			}

			return underlyingMsg;
		}

		std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgDeposit(const api::CosmosLikeMsgDeposit & msg) {
			auto object = std::make_shared<::ledger::core::DynamicObject>();
			auto value = std::make_shared<::ledger::core::DynamicObject>();
			auto amounts = std::make_shared<::ledger::core::DynamicArray>();

			object->putString(kType, kMsgDeposit);
			object->putObject(kValue, value);

			value->putString(kDepositor, msg.depositor);
			value->putString(kProposalId, msg.proposalId);
			value->putArray(kAmount, amounts);

			std::for_each(std::cbegin(msg.amount), std::cend(msg.amount), [&amounts](auto const& amount) {
				addAmount(amounts, amount);
			});

			return std::make_shared<::ledger::core::CosmosLikeMessage>(object);
		}

		api::CosmosLikeMsgDeposit api::CosmosLikeMessage::unwrapMsgDeposit(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto implMsg = std::dynamic_pointer_cast<::ledger::core::CosmosLikeMessage>(msg);

			if (msg->getMessageType() != api::CosmosLikeMsgType::MSGDEPOSIT
				|| !implMsg->_content->contains(kValue)) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgDeposit");
			}

			auto underlyingMsg = api::CosmosLikeMsgDeposit{};
			auto value = std::dynamic_pointer_cast<::ledger::core::DynamicObject>(implMsg->_content->getObject(kValue));

			{
				static const char* keys[] = { kDepositor, kProposalId, kAmount };

				if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(value))) {
					throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgDeposit: bad value fields");
				}

				underlyingMsg.depositor = value->getString(kDepositor).value();
				underlyingMsg.proposalId = value->getString(kProposalId).value();
			}

			{
				static const char* keys[] = { kAmount, kDenom };

				auto amounts = std::dynamic_pointer_cast<::ledger::core::DynamicArray>(value->getArray(kAmount));

				for (auto size = amounts->size(), i = decltype(size){0}; i < size; ++i) {
					auto amount = amounts->getObject(i);

					if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(amount))) {
						throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgDeposit: bad amount fields");
					}

					underlyingMsg.amount.push_back(CosmosLikeAmount{
						amount->getString(kAmount).value(),
						amount->getString(kDenom).value()
					});
				}
			}

			return underlyingMsg;
		}


		std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgWithdrawDelegationReward(const api::CosmosLikeMsgWithdrawDelegationReward & msg) {
			auto object = std::make_shared<::ledger::core::DynamicObject>();
			auto value = std::make_shared<::ledger::core::DynamicObject>();

			object->putString(kType, kMsgWithdrawDelegationReward);
			object->putObject(kValue, value);

			value->putString(kDelegatorAddress, msg.delegatorAddress);
			value->putString(kValidatorAddress, msg.validatorAddress);

			return std::make_shared<::ledger::core::CosmosLikeMessage>(object);
		}


    	api::CosmosLikeMsgWithdrawDelegationReward api::CosmosLikeMessage::unwrapMsgWithdrawDelegationReward(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto implMsg = std::dynamic_pointer_cast<::ledger::core::CosmosLikeMessage>(msg);

			if (msg->getMessageType() != api::CosmosLikeMsgType::MSGWITHDRAWDELEGATIONREWARD
				|| !implMsg->_content->contains(kValue)) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgWithDrawDelegationReward");
			}

			auto underlyingMsg = api::CosmosLikeMsgWithdrawDelegationReward{};
			auto value = std::dynamic_pointer_cast<::ledger::core::DynamicObject>(implMsg->_content->getObject(kValue));

			{
				static const char* keys[] = { kDelegatorAddress, kValidatorAddress };

				if (!std::all_of(std::cbegin(keys), std::cend(keys), containsKeys(value))) {
					throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgWithDrawDelegationReward: bad value fields");
				}

				underlyingMsg.delegatorAddress = value->getString(kDelegatorAddress).value();
				underlyingMsg.validatorAddress = value->getString(kValidatorAddress).value();
			}

			return underlyingMsg;
		}
	}
}
