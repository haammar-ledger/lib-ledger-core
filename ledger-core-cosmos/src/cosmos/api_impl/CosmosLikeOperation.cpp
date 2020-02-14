/*
 *
 * CosmosLikeOperation
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


#include <cosmos/api_impl/CosmosLikeOperation.hpp>

#include <core/api/ErrorCode.hpp>
#include <core/utils/Exception.hpp>
#include <core/operation/OperationDatabaseHelper.hpp>

#include <cosmos/api_impl/CosmosLikeTransactionApi.hpp>
#include <cosmos/CosmosLikeMessage.hpp>

namespace ledger {
    namespace core {

        CosmosLikeOperation::CosmosLikeOperation(std::shared_ptr<AbstractAccount> const& account,
                                                 ledger::core::cosmos::Transaction const& tx,
                                                 ledger::core::cosmos::Message const& msg) :
            Operation(account),
            _txApi(std::make_shared<CosmosLikeTransactionApi>(account->getWallet()->getCurrency(), tx)),
            _msgApi(std::make_shared<CosmosLikeMessage>(msg)),
            txData(tx),
            msgData(msg)
        {}

        void CosmosLikeOperation::setTransactionData(ledger::core::cosmos::Transaction const& tx) {
            txData = tx;
            std::static_pointer_cast<CosmosLikeTransactionApi>(_txApi)->setRawData(txData);
        }

/*
        ledger::core::cosmos::Transaction& CosmosLikeOperation::getTransactionData() {
            return _txData;
        }
*/
        void CosmosLikeOperation::setMessageData(ledger::core::cosmos::Message const& msg) {
            msgData = msg;
            std::static_pointer_cast<CosmosLikeMessage>(_msgApi)->setRawData(msgData);
        }
/*
        ledger::core::cosmos::Message& CosmosLikeOperation::getMessageData() {
            return _msgData;
        }
*/
		std::shared_ptr<api::CosmosLikeTransaction> CosmosLikeOperation::getTransaction() {
            return _txApi;
            //return std::make_shared<CosmosLikeTransactionApi>(TODO);
        }

		std::shared_ptr<api::CosmosLikeMessage> CosmosLikeOperation::getMessage() {
			return _msgApi;
			//return _txApi->getMessages();
            //return std::make_shared<CosmosLikeMessage>(TODO);
		}

        // FIXME Test this
        void CosmosLikeOperation::refreshUid(const std::string &msgIndex) {
            // FIXME which one ??
            //auto final = fmt::format("{}+{}", _txApi->getHash(), api::to_string(_msgApi->getMessageType()));
            auto final = fmt::format("{}+{}", txData.hash, msgIndex, msgData.type);

            /*
            if (!additional.empty()) {
                final = fmt::format("{}+{}", final, additional);
            }
            */
            uid = OperationDatabaseHelper::createUid(accountUid, final, getOperationType());
        }

    }
}
