/*
 * AlgorandTransaction
 *
 * Created by Rémi Barjon on 04/05/2020.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Ledger
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

#include <algorand/model/transactions/AlgorandTransaction.hpp>

namespace ledger {
namespace core {
namespace algorand {

    Header::Header(uint64_t fee,
                   uint64_t firstValid,
                   Option<std::string> genesisId,
                   B64String genesisHash,
                   Option<std::vector<uint8_t>> group,
                   uint64_t lastValid,
                   Option<std::vector<uint8_t>> lease,
                   Option<std::vector<uint8_t>> note,
                   Address sender,
                   constants::TxType type)
        : fee(fee)
        , firstValid(firstValid)
        , genesisId(std::move(genesisId))
        , genesisHash(std::move(genesisHash))
        , group(std::move(group))
        , lastValid(lastValid)
        , lease(std::move(lease))
        , note(std::move(note))
        , sender(std::move(sender)), type(type)
    {}

    Transaction::Transaction(Header header,
                             TransactionDetails details)
        : header(std::move(header))
        , details(std::move(details))
    {}

} // namespace algorand
} // namespace core
} // namespace ledger

