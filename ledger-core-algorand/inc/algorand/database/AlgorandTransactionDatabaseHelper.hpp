/*
 * AlgorandTransactionDatabaseHelper
 *
 * Created by Hakim Aammar on 18/05/2020.
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

#ifndef LEDGER_CORE_ALGORANDTRANSACTIONDATABASEHELPER_H
#define LEDGER_CORE_ALGORANDTRANSACTIONDATABASEHELPER_H

#include <algorand/model/transactions/AlgorandTransaction.hpp>

#include <boost/optional.hpp>

#define SOCI_USE_BOOST
#include <soci.h>
#include <core/database/SociNumber.hpp>

namespace ledger {
namespace core {
namespace algorand {

    // Helpers to deal with Option<> types,
    // because SOCI only understands boost::optional<>...

    template<class Raw>
    static auto optionalValue(Option<Raw> opt) {
        return opt.hasValue() ? *opt : boost::optional<Raw>();
    }

    template<class Raw, class Transformed>
    static auto optionalValueWithTransform(Option<Raw> opt, const std::function<Transformed(Raw)> & transform) {
        return opt.hasValue() ? transform(*opt) : boost::optional<Transformed>();
    }

    static std::string getString(const soci::row & row, const int32_t colId) {
        return row.get<std::string>(colId);
    }

    static Option<std::string> getOptionalString(const soci::row & row, const int32_t colId) {
        return row.get_indicator(colId) != soci::i_null ? Option<std::string>(row.get<std::string>(colId)) : Option<std::string>::NONE;
    }

    template<class Transformed>
    static Option<Transformed> getOptionalStringWithTransform(const soci::row & row, const int32_t colId, const std::function<Transformed(std::string)> & transform) {
        return row.get_indicator(colId) != soci::i_null ? Option<Transformed>(transform(row.get<std::string>(colId))) : Option<Transformed>::NONE;
    }

    static uint64_t getNumber(const soci::row & row, const int32_t colId) {
        return soci::get_number<uint64_t>(row, colId);
    }

    static Option<uint64_t> getOptionalNumber(const soci::row & row, const int32_t colId) {
        return row.get_indicator(colId) != soci::i_null ? Option<uint64_t>(soci::get_number<uint64_t>(row, colId)) : Option<uint64_t>::NONE;
    }

    template<class Transformed>
    static Option<Transformed> getOptionalNumberWithTransform(const soci::row & row, const int32_t colId, const std::function<Transformed(uint64_t)> & transform) {
        return row.get_indicator(colId) != soci::i_null ? Option<Transformed>(transform(soci::get_number<uint64_t>(row, colId))) : Option<Transformed>::NONE;
    }

    class TransactionDatabaseHelper {

    public:

        static bool transactionExists(soci::session & sql,
                                      const std::string & txUid);

        static bool getTransactionByHash(soci::session & sql,
                                         const std::string & hash,
                                         model::Transaction & tx);

        static std::string createTransactionUid(const std::string & accountUid,
                                                const std::string & txHash);

        static std::string putTransaction(soci::session & sql,
                                          const std::string & accountUid,
                                          const model::Transaction & tx);

    };

} // namespace algorand
} // namespace core
} // namespace ledger

#endif // LEDGER_CORE_ALGORANDTRANSACTIONDATABASEHELPER_H
