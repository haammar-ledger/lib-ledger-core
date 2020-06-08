/*
 * AlgorandBlockDatabaseHelper
 *
 * Created by Hakim Aammar on 01/06/2020.
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

#pragma once

//#include <core/wallet/BlockDatabaseHelper.hpp>
#include <soci.h>
//#include <string>

#include <core/api/Block.hpp>
//#include <core/utils/Option.hpp>


namespace ledger {
namespace core {
namespace algorand {

    class BlockDatabaseHelper {//: public ledger::core::BlockDatabaseHelper {

    public:

        static bool blockExists(soci::session & sql, const int64_t blockHeight, const std::string & currencyName);
        static bool putBlock(soci::session & sql, api::Block & block);


    private:
        //static std::string createBlockUid(const api::Block & block);
        static std::string createBlockUid(const int64_t blockHeight, const std::string & currencyName);

        /*
        static std::string createBlockUid(const api::Block& block);
        static std::string createBlockUid(const std::string& blockhash, const std::string& currencyName);
        static bool putBlock(soci::session& sql, const api::Block& block);
        static bool blockExists(soci::session& sql, const std::string& blockHash, const std::string& currencyName);
        static Option<api::Block> getLastBlock(soci::session& sql, const std::string& currencyName);
        static Option<api::Block> getPreviousBlockInDatabase(soci::session& sql, const std::string& currencyName, int64_t blockHeight);
        static Option<api::Block> getPreviousBlockInDatabase(soci::session &sql, const std::string &currencyName, std::chrono::system_clock::time_point date);
        */
    };
}
}
}
