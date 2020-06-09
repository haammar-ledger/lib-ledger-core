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

#include <algorand/database/AlgorandBlockDatabaseHelper.hpp>

#include <core/crypto/SHA256.hpp>
#include <core/database/SociDate.hpp>
#include <core/database/SociNumber.hpp>

#include <fmt/format.h>

namespace ledger {
namespace core {
namespace algorand {

    static Option<api::Block> getBlockFromRow(soci::row &databaseRow, const std::string &currencyName) {
        auto uid = databaseRow.get<std::string>(0);
        auto hash = databaseRow.get<std::string>(1);
        auto height = soci::get_number<int64_t>(databaseRow, 2);
        auto time = databaseRow.get<std::chrono::system_clock::time_point>(3);
        return Option<api::Block>(
                api::Block(hash, uid, time, currencyName, height)
        );
    }

    Option<api::Block> BlockDatabaseHelper::getLastBlockBefore(soci::session &sql,
                                                               const std::string &currencyName,
                                                               std::chrono::system_clock::time_point date) {
        soci::rowset<soci::row> rows = (sql.prepare << "SELECT uid, hash, height, time FROM blocks WHERE "
                "currency_name = :name AND time < :date ORDER BY height DESC LIMIT 1",
                soci::use(currencyName), soci::use(date));

        for (auto& row : rows) {
            return getBlockFromRow(row, currencyName);
        }
        return Option<api::Block>();
    }

    bool BlockDatabaseHelper::blockExists(soci::session & sql, const int64_t blockHeight, const std::string & currencyName) {
        auto count = 0;
        auto uid = createBlockUid(blockHeight, currencyName);
        sql << "SELECT COUNT(*) FROM blocks WHERE uid = :uid", soci::use(uid), soci::into(count);
        return count > 0;
    }

    bool BlockDatabaseHelper::putBlock(soci::session & sql, api::Block & block) {
        if (!blockExists(sql, block.height, block.currencyName)) {
            block.uid = createBlockUid(block.height, block.currencyName);
            sql << "INSERT INTO blocks VALUES(:uid, :hash, :height, :time, :currency_name)",
                    soci::use(block.uid), soci::use(block.blockHash), soci::use(block.height), soci::use(block.time), soci::use(block.currencyName);
            return true;
        }
        return false;
    }

    std::string BlockDatabaseHelper::createBlockUid(const int64_t blockHeight, const std::string & currencyName) {
        return SHA256::stringToHexHash(fmt::format("uid:{}+{}", blockHeight, currencyName));
    }

}
}
}

