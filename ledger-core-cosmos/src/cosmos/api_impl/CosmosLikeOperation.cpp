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

#include <cosmos/api_impl/CosmosLikeTransactionApi.hpp>

namespace ledger {
    namespace core {

        CosmosLikeOperation::CosmosLikeOperation(const std::shared_ptr<Operation>& baseOp) {
            _transaction = std::make_shared<CosmosLikeTransactionApi>(baseOp);
        }

		std::shared_ptr<api::CosmosLikeTransaction> CosmosLikeOperation::getTransaction() {
            return _transaction;
        }

		std::shared_ptr<api::CosmosLikeMessage> CosmosLikeOperation::getMessage() {
			throw make_exception(api::ErrorCode::IMPLEMENTATION_IS_MISSING, "CosmosLikeOperation::getMessage");
		}
    }
}
