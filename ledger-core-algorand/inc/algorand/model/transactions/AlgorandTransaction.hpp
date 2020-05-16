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

#pragma once

#include <algorand/AlgorandAddress.hpp>
#include <algorand/utils/B64String.hpp>
#include <algorand/model/transactions/AlgorandAsset.hpp>
#include <algorand/model/transactions/AlgorandKeyreg.hpp>
#include <algorand/model/transactions/AlgorandPayment.hpp>

#include <core/utils/Option.hpp>

#include <boost/variant.hpp>

#include <cstdint>
#include <string>

namespace ledger {
namespace core {
namespace algorand {
namespace constants {

    // Header
    static constexpr char fee[] = "fee";
    static constexpr char fv[] = "fv";
    static constexpr char gen[] = "gen";
    static constexpr char gh[] = "gh";
    static constexpr char grp[] = "grp";
    static constexpr char lv[] = "lv";
    static constexpr char lx[] = "lx";
    static constexpr char note[] = "note";
    static constexpr char snd[] = "snd";
    static constexpr char type[] = "type";

    // Key Registration Transaction
    static constexpr char nonpart[] = "nonpart";
    static constexpr char selkey[] = "selkey";
    static constexpr char votefst[] = "votefst";
    static constexpr char votekd[] = "votekd";
    static constexpr char votekey[] = "votekey";
    static constexpr char votelst[] = "votelst";

    // Payment Transaction
    static constexpr char amt[] = "amt";
    static constexpr char close[] = "close";
    static constexpr char rcv[] = "rcv";

    // Asset configuration Transaction
    static constexpr char apar[] = "apar";
    static constexpr char caid[] = "caid";
    // Asset parameters
    static constexpr char am[] = "am";
    static constexpr char an[] = "an";
    static constexpr char au[] = "au";
    static constexpr char c[] = "c";
    static constexpr char dc[] = "dc";
    static constexpr char df[] = "df";
    static constexpr char f[] = "f";
    static constexpr char m[] = "m";
    static constexpr char r[] = "r";
    static constexpr char t[] = "t";
    static constexpr char un[] = "un";

    // Asset Transfer Transaction
    static constexpr char aamt[] = "aamt";
    static constexpr char aclose[] = "aclose";
    static constexpr char arcv[] = "arcv";
    static constexpr char asnd[] = "asnd";
    static constexpr char xaid[] = "xaid";

    // Asset Freeze Transaction
    static constexpr char afrz[] = "afrz";
    static constexpr char fadd[] = "fadd";
    static constexpr char faid[] = "faid";

    // Signed Transaction
    static constexpr char lsig[] = "lsig";
    static constexpr char msig[] = "msig";
    static constexpr char sig[] = "sig";
    static constexpr char txn[] = "txn";

} // namespace constants

namespace model {

    class Transaction
    {

    public:
        class Header
        {
        public:
            Header() = default;
            Header(uint64_t fee,
                   uint64_t firstValid,
                   Option<std::string> genesisId,
                   B64String genesisHash,
                   Option<std::vector<uint8_t>> group,
                   uint64_t lastValid,
                   Option<std::vector<uint8_t>> lease,
                   Option<std::vector<uint8_t>> note,
                   Address sender,
                   std::string type)
                : fee(fee)
                , firstValid(firstValid)
                , genesisId(std::move(genesisId))
                , genesisHash(std::move(genesisHash))
                , group(std::move(group))
                , lastValid(lastValid)
                , lease(std::move(lease))
                , note(std::move(note))
                , sender(std::move(sender))
                , type(std::move(type))
            {}

            uint64_t fee;
            uint64_t firstValid;
            Option<std::string> genesisId;
            B64String genesisHash;
            Option<std::vector<uint8_t>> group;
            uint64_t lastValid;
            Option<std::vector<uint8_t>> lease;
            Option<std::vector<uint8_t>> note;
            Address sender;
            std::string type;

            // Additional fields retrieved from the blockchain
            Option<std::string> id;
            Option<uint64_t> round;
            Option<uint64_t> fromRewards;

        };

        using Details = boost::variant<KeyRegTxnFields,
                                       PaymentTxnFields,
                                       AssetConfigTxnFields,
                                       AssetTransferTxnFields,
                                       AssetFreezeTxnFields>;

        Transaction() = default;
        Transaction(Header header, Details details)
            : header(std::move(header))
            , details(std::move(details))
        {}

        Header header;
        Details details;
    };

    namespace constants {

        static constexpr char pay[] = "pay";
        static constexpr char keyreg[] = "keyreg";
        static constexpr char acfg[] = "acfg";
        static constexpr char axfer[] = "axfer";
        static constexpr char afreeze[] = "afrz";

    } // namespace constants

    struct TransactionsBulk {
        std::vector<Transaction> transactions;
        bool hasNext;
    };

} // namespace model
} // namespace ledger
} // namespace core
} // namespace algorand

