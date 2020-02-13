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
//#include <core/collections/DynamicArray.hpp>

#include <cosmos/CosmosLikeConstants.hpp>

namespace ledger {
	namespace core {

		using namespace cosmos::constants;

        //using Object = GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>::Object;
		//using rapidjsonArray = rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>::Array;

		namespace {
			/*
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
			*/
			// FIXME Test this
			// add a `CosmosLikeContent` to a `rapidjson::Value`
			inline auto addContent(api::CosmosLikeContent const& content,
								   rapidjson::Value& json,
								   rapidjson::Document::AllocatorType& allocator) {
				rapidjson::Value jsonContent(rapidjson::kObjectType);
            	rapidjson::Value jsonString(rapidjson::kStringType);

				// api::CosmosLikeContent::type
				jsonString.SetString(content.type.c_str(), static_cast<rapidjson::SizeType>(content.type.length()), allocator);
				jsonContent.AddMember(kType, jsonString, allocator);

				// api::CosmosLikeContent::title
				jsonString.SetString(content.title.c_str(), static_cast<rapidjson::SizeType>(content.title.length()), allocator);
				jsonContent.AddMember(kTitle, jsonString, allocator);

				// api::CosmosLikeContent::description
				jsonString.SetString(content.description.c_str(), static_cast<rapidjson::SizeType>(content.description.length()), allocator);
				jsonContent.AddMember(kDescription, jsonString, allocator);

				json.AddMember(kAmount, jsonContent, allocator);
			}

			// FIXME Test this
			// add a `CosmosLikeAmount` to a `rapidjson::Value`
			inline auto addAmount(api::CosmosLikeAmount const& amount,
								  rapidjson::Value& json,
								  rapidjson::Document::AllocatorType& allocator) {
				rapidjson::Value jsonAmount(rapidjson::kObjectType);
            	rapidjson::Value jsonString(rapidjson::kStringType);

				// api::CosmosLikeAmount::amount
				jsonString.SetString(amount.amount.c_str(), static_cast<rapidjson::SizeType>(amount.amount.length()), allocator);
				jsonAmount.AddMember(kAmount, jsonString, allocator);

				// api::CosmosLikeAmount::denom
				jsonString.SetString(amount.denom.c_str(), static_cast<rapidjson::SizeType>(amount.denom.length()), allocator);
				jsonAmount.AddMember(kDenom, jsonString, allocator);

				json.AddMember(kAmount, jsonAmount, allocator);
			}

			// FIXME Test this
			// add a `CosmosLikeAmount` to an array of `rapidjson::Value`
			inline auto addAmounts(std::vector<api::CosmosLikeAmount> const& amounts,
								  rapidjson::Value& json,
								  rapidjson::Document::AllocatorType& allocator) {
				rapidjson::Value jsonAmounts(rapidjson::kArrayType);
				rapidjson::Value jsonString(rapidjson::kStringType);
				for (auto amount : amounts) {
					rapidjson::Value jsonAmount(rapidjson::kObjectType);

					// api::CosmosLikeAmount::amount
					jsonString.SetString(amount.amount.c_str(), static_cast<rapidjson::SizeType>(amount.amount.length()), allocator);
					jsonAmount.AddMember(kAmount, jsonString, allocator);

					// api::CosmosLikeAmount::denom
					jsonString.SetString(amount.denom.c_str(), static_cast<rapidjson::SizeType>(amount.denom.length()), allocator);
					jsonAmount.AddMember(kDenom, jsonString, allocator);

					jsonAmounts.PushBack(jsonAmount, allocator);
				}
            	json.AddMember(kAmount, jsonAmounts, allocator);
			}
		}

/*
		CosmosLikeMessage::CosmosLikeMessage(const std::shared_ptr<DynamicObject> &content) :
			_content(content)
		{}

*/
        CosmosLikeMessage::CosmosLikeMessage(const cosmos::Message& msg) :
			_msgData(msg)
		{}

		const cosmos::Message& CosmosLikeMessage::getRawData() const {
			return _msgData;
		}

		/*
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
				content = std::std::dynamic_pointer_cast<CosmosLikeMessage>(
								CosmosLikeMessage::wrapMsgSend(
									boost::get<cosmos::MsgSend>(rawStruct.content)))
								->_content;
			} else if (msgType == kMsgDelegate) {
				content = std::std::dynamic_pointer_cast<CosmosLikeMessage>(
								CosmosLikeMessage::wrapMsgDelegate(
									boost::get<cosmos::MsgDelegate>(rawStruct.content)))
								->_content;
			} else if (msgType == kMsgUndelegate) {
				content = std::std::dynamic_pointer_cast<CosmosLikeMessage>(
								CosmosLikeMessage::wrapMsgUndelegate(
									boost::get<cosmos::MsgUndelegate>(rawStruct.content)))
								->_content;
			} else if (msgType == kMsgRedelegate) {
				content = std::std::dynamic_pointer_cast<CosmosLikeMessage>(
								CosmosLikeMessage::wrapMsgRedelegate(
									boost::get<cosmos::MsgRedelegate>(rawStruct.content)))
								->_content;
			} else if (msgType == kMsgSubmitProposal) {
				content =
					std::std::dynamic_pointer_cast<CosmosLikeMessage>(
						CosmosLikeMessage::wrapMsgSubmitProposal(
							boost::get<cosmos::MsgSubmitProposal>(rawStruct.content)))
						->_content;
			} else if (msgType == kMsgVote) {
				content = std::std::dynamic_pointer_cast<CosmosLikeMessage>(
								CosmosLikeMessage::wrapMsgVote(
									boost::get<cosmos::MsgVote>(rawStruct.content)))
								->_content;
			} else if (msgType == kMsgDeposit) {
				content = std::std::dynamic_pointer_cast<CosmosLikeMessage>(
								CosmosLikeMessage::wrapMsgDeposit(
									boost::get<cosmos::MsgDeposit>(rawStruct.content)))
								->_content;
			} else if (msgType == kMsgWithdrawDelegationReward) {
				content = std::std::dynamic_pointer_cast<CosmosLikeMessage>(
								CosmosLikeMessage::wrapMsgWithdrawDelegationReward(
									boost::get<cosmos::MsgWithdrawDelegationReward>(
										rawStruct.content)))
								->_content;
			}  // else the content stays nullptr

			_content = content;
		}
		*/

		api::CosmosLikeMsgType CosmosLikeMessage::getMessageType() const {
			auto msgType = getRawMessageType();

			if (msgType == kMsgSend) {
				return api::CosmosLikeMsgType::MSGSEND;
			} else if (msgType == kMsgDelegate) {
				return api::CosmosLikeMsgType::MSGDELEGATE;
			} else if (msgType == kMsgUndelegate) {
				return api::CosmosLikeMsgType::MSGDELEGATE;
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
			//return _content->get<std::string>(kType).value_or("");
			return _msgData.type;
		}

		// Note : this Json has not been sorted yet
		// This doesn't follow the spec
		// https://github.com/cosmos/ledger-cosmos-app/blob/master/docs/TXSPEC.md
		// but we defer the sorting to the TransactionApi::serialize() method
		rapidjson::Value CosmosLikeMessage::toJson(rapidjson::Document::AllocatorType& allocator) const {

			// TODO [refacto] Implement a cosmos Serializer/Deserializer
			// for all kind of conversions from/to cosmos::structures (here toJson)

			rapidjson::Value json(rapidjson::kObjectType);

            rapidjson::Value jsonString(rapidjson::kStringType);

			// cosmos::Message::type
            jsonString.SetString(_msgData.type.c_str(), static_cast<rapidjson::SizeType>(_msgData.type.length()), allocator);
            json.AddMember(kType, jsonString, allocator);

			rapidjson::Value jsonContent(rapidjson::kObjectType);
			if (_msgData.type == kMsgSend) {

				const auto& content = boost::get<cosmos::MsgSend>(_msgData.content);

				// cosmos::MsgSend::fromAddress
				jsonString.SetString(content.fromAddress.c_str(), static_cast<rapidjson::SizeType>(content.fromAddress.length()), allocator);
				jsonContent.AddMember(kFromAddress, jsonString, allocator);

				// cosmos::MsgSend::toAddress
				jsonString.SetString(content.toAddress.c_str(), static_cast<rapidjson::SizeType>(content.toAddress.length()), allocator);
				jsonContent.AddMember(kToAddress, jsonString, allocator);

				// cosmos::MsgSend::amount
				addAmounts(content.amount, jsonContent, allocator);

			} else if (_msgData.type == kMsgDelegate) {

				const auto& content = boost::get<cosmos::MsgDelegate>(_msgData.content);
				/*
				std::string delegatorAddress;
				std::string validatorAddress;
				CosmosLikeAmount amount;
				*/
				// cosmos::MsgDelegate::delegatorAddress
				jsonString.SetString(content.delegatorAddress.c_str(), static_cast<rapidjson::SizeType>(content.delegatorAddress.length()), allocator);
				jsonContent.AddMember(kDelegatorAddress, jsonString, allocator);

				// cosmos::MsgDelegate::validatorAddress
				jsonString.SetString(content.validatorAddress.c_str(), static_cast<rapidjson::SizeType>(content.validatorAddress.length()), allocator);
				jsonContent.AddMember(kValidatorAddress, jsonString, allocator);

				// cosmos::MsgDelegate::amount
				addAmount(content.amount, jsonContent, allocator);

			} else if (_msgData.type == kMsgUndelegate) {

				const auto& content = boost::get<cosmos::MsgUndelegate>(_msgData.content);
				/*
				std::string delegatorAddress;
				std::string validatorAddress;
				CosmosLikeAmount amount;
				*/
				// cosmos::MsgUndelegate::delegatorAddress
				jsonString.SetString(content.delegatorAddress.c_str(), static_cast<rapidjson::SizeType>(content.delegatorAddress.length()), allocator);
				jsonContent.AddMember(kDelegatorAddress, jsonString, allocator);

				// cosmos::MsgUndelegate::validatorAddress
				jsonString.SetString(content.validatorAddress.c_str(), static_cast<rapidjson::SizeType>(content.validatorAddress.length()), allocator);
				jsonContent.AddMember(kValidatorAddress, jsonString, allocator);

				// cosmos::MsgUndelegate::amount
				addAmount(content.amount, jsonContent, allocator);

			} else if (_msgData.type == kMsgRedelegate) {

				const auto& content = boost::get<cosmos::MsgRedelegate>(_msgData.content);
				/*
				std::string delegatorAddress;
				std::string validatorSourceAddress;
				std::string validatorDestinationAddress;
				CosmosLikeAmount amount;
				*/
				// cosmos::MsgRedelegate::delegatorAddress
				jsonString.SetString(content.delegatorAddress.c_str(), static_cast<rapidjson::SizeType>(content.delegatorAddress.length()), allocator);
				jsonContent.AddMember(kDelegatorAddress, jsonString, allocator);

				// cosmos::MsgRedelegate::validatorSourceAddress
				jsonString.SetString(content.validatorSourceAddress.c_str(), static_cast<rapidjson::SizeType>(content.validatorSourceAddress.length()), allocator);
				jsonContent.AddMember(kValidatorSrcAddress, jsonString, allocator);

				// cosmos::MsgRedelegate::validatorDestinationAddress
				jsonString.SetString(content.validatorDestinationAddress.c_str(), static_cast<rapidjson::SizeType>(content.validatorDestinationAddress.length()), allocator);
				jsonContent.AddMember(kValidatorDstAddress, jsonString, allocator);

				// cosmos::MsgRedelegate::amount
				addAmount(content.amount, jsonContent, allocator);

			} else if (_msgData.type == kMsgSubmitProposal) {

				const auto& content = boost::get<cosmos::MsgSubmitProposal>(_msgData.content);
				/*
				CosmosLikeContent content;
				std::string proposer;
				std::vector<CosmosLikeAmount> initialDeposit;
				*/

				// cosmos::MsgSubmitProposal::content
				addContent(content.content, jsonContent, allocator);

				// cosmos::MsgSubmitProposal::proposer
				jsonString.SetString(content.proposer.c_str(), static_cast<rapidjson::SizeType>(content.proposer.length()), allocator);
				jsonContent.AddMember(kProposer, jsonString, allocator);

				// cosmos::MsgSubmitProposal::initialDeposit
				addAmounts(content.initialDeposit, jsonContent, allocator);

			} else if (_msgData.type == kMsgVote) {

				const auto& content = boost::get<cosmos::MsgVote>(_msgData.content);
				/*
				std::string voter;
				std::string proposalId;
				CosmosLikeVoteOption option;
				*/

				// cosmos::MsgVote::voter
				jsonString.SetString(content.voter.c_str(), static_cast<rapidjson::SizeType>(content.voter.length()), allocator);
				jsonContent.AddMember(kVoter, jsonString, allocator);

				// cosmos::MsgVote::proposalId
				jsonString.SetString(content.proposalId.c_str(), static_cast<rapidjson::SizeType>(content.proposalId.length()), allocator);
				jsonContent.AddMember(kProposalId, jsonString, allocator);

				// cosmos::MsgVote::option
				auto option = api::to_string(content.option);
				jsonString.SetString(option.c_str(), static_cast<rapidjson::SizeType>(option.length()), allocator);
				jsonContent.AddMember(kOption, jsonString, allocator);

			} else if (_msgData.type == kMsgDeposit) {

				const auto& content = boost::get<cosmos::MsgDeposit>(_msgData.content);
				/*
				std::string depositor;
				std::string proposalId;
				std::vector<CosmosLikeAmount> amount;
				*/

				// cosmos::MsgDeposit::depositor
				jsonString.SetString(content.depositor.c_str(), static_cast<rapidjson::SizeType>(content.depositor.length()), allocator);
				jsonContent.AddMember(kDepositor, jsonString, allocator);

				// cosmos::MsgDeposit::proposalId
				jsonString.SetString(content.proposalId.c_str(), static_cast<rapidjson::SizeType>(content.proposalId.length()), allocator);
				jsonContent.AddMember(kProposalId, jsonString, allocator);

				// cosmos::MsgDeposit::amount
				addAmounts(content.amount, jsonContent, allocator);

			} else if (_msgData.type == kMsgWithdrawDelegationReward) {

				const auto& content = boost::get<cosmos::MsgWithdrawDelegationReward>(_msgData.content);
				/*
				std::string delegatorAddress;
				std::string validatorAddress;
				*/

				// cosmos::MsgWithdrawDelegationReward::delegatorAddress
				jsonString.SetString(content.delegatorAddress.c_str(), static_cast<rapidjson::SizeType>(content.delegatorAddress.length()), allocator);
				jsonContent.AddMember(kDelegatorAddress, jsonString, allocator);

				// cosmos::MsgWithdrawDelegationReward::validatorAddress
				jsonString.SetString(content.validatorAddress.c_str(), static_cast<rapidjson::SizeType>(content.validatorAddress.length()), allocator);
				jsonContent.AddMember(kValidatorAddress, jsonString, allocator);

			} // else the content stays empty

			json.AddMember(kContent, jsonContent, allocator);

			return json;
		}

		// TODO [refacto] Templatize all the following functions

		std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgSend(const api::CosmosLikeMsgSend & msgContent) {
			cosmos::Message msg;
			msg.type = kMsgSend;
			msg.content = msgContent;
			return std::make_shared<::ledger::core::CosmosLikeMessage>(msg);
		}

    	api::CosmosLikeMsgSend api::CosmosLikeMessage::unwrapMsgSend(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto cosmosMsg = std::dynamic_pointer_cast<ledger::core::CosmosLikeMessage>(msg);
			if (cosmosMsg->getMessageType() != api::CosmosLikeMsgType::MSGSEND) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgSend");
			}
			return boost::get<cosmos::MsgSend>(cosmosMsg->getRawData().content);
		}

    	std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgDelegate(const api::CosmosLikeMsgDelegate & msgContent) {
			cosmos::Message msg;
			msg.type = kMsgDelegate;
			msg.content = msgContent;
			return std::make_shared<::ledger::core::CosmosLikeMessage>(msg);
		}

    	api::CosmosLikeMsgDelegate api::CosmosLikeMessage::unwrapMsgDelegate(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto cosmosMsg = std::dynamic_pointer_cast<ledger::core::CosmosLikeMessage>(msg);
			if (cosmosMsg->getMessageType() != api::CosmosLikeMsgType::MSGDELEGATE) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgDelegate");
			}
			return boost::get<cosmos::MsgDelegate>(cosmosMsg->getRawData().content);
		}

    	std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgUndelegate(const api::CosmosLikeMsgUndelegate & msgContent) {
			cosmos::Message msg;
			msg.type = kMsgUndelegate;
			msg.content = msgContent;
			return std::make_shared<::ledger::core::CosmosLikeMessage>(msg);
		}

    	api::CosmosLikeMsgUndelegate api::CosmosLikeMessage::unwrapMsgUndelegate(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto cosmosMsg = std::dynamic_pointer_cast<ledger::core::CosmosLikeMessage>(msg);
			if (cosmosMsg->getMessageType() != api::CosmosLikeMsgType::MSGUNDELEGATE) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgUndelegate");
			}
			return boost::get<cosmos::MsgUndelegate>(cosmosMsg->getRawData().content);
		}

    	std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgRedelegate(const api::CosmosLikeMsgRedelegate & msgContent) {
			cosmos::Message msg;
			msg.type = kMsgRedelegate;
			msg.content = msgContent;
			return std::make_shared<::ledger::core::CosmosLikeMessage>(msg);
		}

    	api::CosmosLikeMsgRedelegate api::CosmosLikeMessage::unwrapMsgRedelegate(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto cosmosMsg = std::dynamic_pointer_cast<ledger::core::CosmosLikeMessage>(msg);
			if (cosmosMsg->getMessageType() != api::CosmosLikeMsgType::MSGREDELEGATE) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgRedelegate");
			}
			return boost::get<cosmos::MsgRedelegate>(cosmosMsg->getRawData().content);
		}

		std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgSubmitProposal(const api::CosmosLikeMsgSubmitProposal & msgContent) {
			cosmos::Message msg;
			msg.type = kMsgSubmitProposal;
			msg.content = msgContent;
			return std::make_shared<::ledger::core::CosmosLikeMessage>(msg);

		}

    	api::CosmosLikeMsgSubmitProposal api::CosmosLikeMessage::unwrapMsgSubmitProposal(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto cosmosMsg = std::dynamic_pointer_cast<ledger::core::CosmosLikeMessage>(msg);
			if (cosmosMsg->getMessageType() != api::CosmosLikeMsgType::MSGSUBMITPROPOSAL) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgSubmitProposal");
			}
			return boost::get<cosmos::MsgSubmitProposal>(cosmosMsg->getRawData().content);
		}


		std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgVote(const api::CosmosLikeMsgVote & msgContent) {
			cosmos::Message msg;
			msg.type = kMsgVote;
			msg.content = msgContent;
			return std::make_shared<::ledger::core::CosmosLikeMessage>(msg);
		}


		api::CosmosLikeMsgVote api::CosmosLikeMessage::unwrapMsgVote(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto cosmosMsg = std::dynamic_pointer_cast<ledger::core::CosmosLikeMessage>(msg);
			if (cosmosMsg->getMessageType() != api::CosmosLikeMsgType::MSGVOTE) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgVote");
			}
			return boost::get<cosmos::MsgVote>(cosmosMsg->getRawData().content);
		}

		std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgDeposit(const api::CosmosLikeMsgDeposit & msgContent) {
			cosmos::Message msg;
			msg.type = kMsgDeposit;
			msg.content = msgContent;
			return std::make_shared<::ledger::core::CosmosLikeMessage>(msg);
		}

		api::CosmosLikeMsgDeposit api::CosmosLikeMessage::unwrapMsgDeposit(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto cosmosMsg = std::dynamic_pointer_cast<ledger::core::CosmosLikeMessage>(msg);
			if (cosmosMsg->getMessageType() != api::CosmosLikeMsgType::MSGDEPOSIT) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgDeposit");
			}
			return boost::get<cosmos::MsgDeposit>(cosmosMsg->getRawData().content);
		}


		std::shared_ptr<api::CosmosLikeMessage> api::CosmosLikeMessage::wrapMsgWithdrawDelegationReward(const api::CosmosLikeMsgWithdrawDelegationReward & msgContent) {
			cosmos::Message msg;
			msg.type = kMsgWithdrawDelegationReward;
			msg.content = msgContent;
			return std::make_shared<::ledger::core::CosmosLikeMessage>(msg);
		}


    	api::CosmosLikeMsgWithdrawDelegationReward api::CosmosLikeMessage::unwrapMsgWithdrawDelegationReward(const std::shared_ptr<api::CosmosLikeMessage> & msg) {
			auto cosmosMsg = std::dynamic_pointer_cast<ledger::core::CosmosLikeMessage>(msg);
			if (cosmosMsg->getMessageType() != api::CosmosLikeMsgType::MSGWITHDRAWDELEGATIONREWARD) {
				throw Exception(api::ErrorCode::RUNTIME_ERROR, "unable to unwrap MsgWithdrawDelegationReward");
			}
			return boost::get<cosmos::MsgWithdrawDelegationReward>(cosmosMsg->getRawData().content);
		}
	}
}
