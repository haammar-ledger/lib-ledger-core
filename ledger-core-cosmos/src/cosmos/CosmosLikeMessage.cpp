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

		namespace {

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

				json.AddMember(kContent, jsonContent, allocator);
			}

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

			// add an array of `CosmosLikeAmount` to a `rapidjson::Value`
			inline auto addAmounts(std::vector<api::CosmosLikeAmount> const& amounts,
								  rapidjson::Value& jsonAmounts,
								  rapidjson::Document::AllocatorType& allocator) {
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
			}
		}

        CosmosLikeMessage::CosmosLikeMessage(const cosmos::Message& msg) :
			_msgData(msg)
		{}

		void CosmosLikeMessage::setRawData(const cosmos::Message &msgData) {
			_msgData = msgData;
		}

		const cosmos::Message& CosmosLikeMessage::getRawData() const {
			return _msgData;
		}

		api::CosmosLikeMsgType CosmosLikeMessage::getMessageType() const {
			auto msgType = getRawMessageType();
			return cosmos::stringToMsgType(msgType.c_str());
		}

		std::string CosmosLikeMessage::getRawMessageType() const {
			return _msgData.type;
		}

		// Note : this Json has not been sorted yet
		// This doesn't follow the spec
		// https://github.com/cosmos/ledger-cosmos-app/blob/master/docs/TXSPEC.md
		// but we defer the sorting to the TransactionApi::serialize() method
		rapidjson::Value CosmosLikeMessage::toJson(rapidjson::Document::AllocatorType& allocator) const {

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
				rapidjson::Value jsonAmounts(rapidjson::kArrayType);
				addAmounts(content.amount, jsonAmounts, allocator);
            	jsonContent.AddMember(kAmount, jsonAmounts, allocator);

			} else if (_msgData.type == kMsgDelegate) {

				const auto& content = boost::get<cosmos::MsgDelegate>(_msgData.content);

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

				// cosmos::MsgSubmitProposal::content
				addContent(content.content, jsonContent, allocator);

				// cosmos::MsgSubmitProposal::proposer
				jsonString.SetString(content.proposer.c_str(), static_cast<rapidjson::SizeType>(content.proposer.length()), allocator);
				jsonContent.AddMember(kProposer, jsonString, allocator);

				// cosmos::MsgSubmitProposal::initialDeposit
				rapidjson::Value jsonAmounts(rapidjson::kArrayType);
				addAmounts(content.initialDeposit, jsonAmounts, allocator);
            	jsonContent.AddMember(kInitialDeposit, jsonAmounts, allocator);

			} else if (_msgData.type == kMsgVote) {

				const auto& content = boost::get<cosmos::MsgVote>(_msgData.content);

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

				// cosmos::MsgDeposit::depositor
				jsonString.SetString(content.depositor.c_str(), static_cast<rapidjson::SizeType>(content.depositor.length()), allocator);
				jsonContent.AddMember(kDepositor, jsonString, allocator);

				// cosmos::MsgDeposit::proposalId
				jsonString.SetString(content.proposalId.c_str(), static_cast<rapidjson::SizeType>(content.proposalId.length()), allocator);
				jsonContent.AddMember(kProposalId, jsonString, allocator);

				// cosmos::MsgDeposit::amount
				rapidjson::Value jsonAmounts(rapidjson::kArrayType);
				addAmounts(content.amount, jsonAmounts, allocator);
            	jsonContent.AddMember(kAmount, jsonAmounts, allocator);

			} else if (_msgData.type == kMsgWithdrawDelegationReward) {

				const auto& content = boost::get<cosmos::MsgWithdrawDelegationReward>(_msgData.content);

				// cosmos::MsgWithdrawDelegationReward::delegatorAddress
				jsonString.SetString(content.delegatorAddress.c_str(), static_cast<rapidjson::SizeType>(content.delegatorAddress.length()), allocator);
				jsonContent.AddMember(kDelegatorAddress, jsonString, allocator);

				// cosmos::MsgWithdrawDelegationReward::validatorAddress
				jsonString.SetString(content.validatorAddress.c_str(), static_cast<rapidjson::SizeType>(content.validatorAddress.length()), allocator);
				jsonContent.AddMember(kValidatorAddress, jsonString, allocator);

			} // else the content stays empty

			json.AddMember(kValue, jsonContent, allocator);

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
