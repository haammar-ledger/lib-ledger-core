/*
 *
 * SociCosmosAmount.cpp
 * ledger-core
 *
 * Created by Pierre Pollastri on 15/01/2020.
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

#include <cosmos/database/SociCosmosAmount.hpp>

namespace soci {
    std::string coinsToString(const std::vector<ledger::core::cosmos::Coin> &coins) {
        using namespace rapidjson;
        Document d;
        auto& allocator = d.GetAllocator();
        auto& array = d.SetArray();
        for (const auto& coin : coins) {
            Value tuple(kArrayType);
            soci::cosmos_coin_to_json_tuple(coin, tuple, allocator);
            array.PushBack(tuple.Move(), allocator);
        }

        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        d.Accept(writer);
        return std::string(buffer.GetString());
    }

    void stringToCoins(const std::string &str, std::vector<ledger::core::cosmos::Coin> &out) {
        using namespace rapidjson;
        Document d;
        d.Parse(str.data());
        const auto& list = d.GetArray();
        auto index = 0;
        out.assign(list.Size(), ledger::core::cosmos::Coin());
        for (const auto& n : list) {
            const auto& tuple = n.GetArray();
            cosmos_coin_from_json_tuple(tuple, out[index]);
            index += 1;
        }
    }
}
