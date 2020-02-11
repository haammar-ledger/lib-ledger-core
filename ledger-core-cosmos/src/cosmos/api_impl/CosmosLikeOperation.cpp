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

namespace ledger {
    namespace core {

        CosmosLikeOperation::CosmosLikeOperation(const api::Currency& currency,
                                                 ledger::core::cosmos::Transaction const& txData) :
            _txApi(std::make_shared<CosmosLikeTransactionApi>(currency)),
            _msgApi(nullptr), // FIXME
            _txData(txData)
        {}

        CosmosLikeOperation::CosmosLikeOperation(const std::shared_ptr<CosmosLikeOperation> &copy,
                                                 ledger::core::cosmos::Transaction const& txData) :
            _txApi(std::make_shared<CosmosLikeTransactionApi>(copy)),
            _msgApi(nullptr), // FIXME
            _txData(txData)
        {}

        void CosmosLikeOperation::setTransactionData(ledger::core::cosmos::Transaction const& txData) {
            _txData = txData;
        }

        ledger::core::cosmos::Transaction& CosmosLikeOperation::getTransactionData() {
            return _txData;
        }

        // TODO ?
        /*
        void CosmosLikeOperation::setMessageData(ledger::core::cosmos::Message const& msgData) {
            _msgData = msgData;
        }

        ledger::core::cosmos::Message& CosmosLikeOperation::getMessageData() {
            return _msgData;
        }
        */

		std::shared_ptr<api::CosmosLikeTransaction> CosmosLikeOperation::getTransaction() {
            return _txApi;
        }

		std::shared_ptr<api::CosmosLikeMessage> CosmosLikeOperation::getMessage() {
			return _msgApi;
		}

        // FIXME Test this
        void CosmosLikeOperation::refreshUid(const std::string &additional) {
            auto final = fmt::format("{}+{}", _txApi->getHash(), api::to_string(_msgApi->getMessageType()));
            if (!additional.empty()) {
                final = fmt::format("{}+{}", final, additional);
            }
            uid = OperationDatabaseHelper::createUid(accountUid, final, getOperationType());
        }

    }
}
