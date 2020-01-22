/*
 *
 * CosmosBech32
 *
 * Created by El Khalil Bellakrid on 12/06/2019.
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


#ifndef LEDGER_CORE_COSMOSBECH32_H
#define LEDGER_CORE_COSMOSBECH32_H

#include <core/math/bech32/Bech32.hpp>
#include <core/math/bech32/Bech32Factory.hpp>
#include <core/math/bech32/Bech32Parameters.hpp>

#include <cosmos/api/CosmosBech32Type.hpp>

using namespace ledger::core;

namespace ledger {
        namespace core {

                //Reference: https://github.com/cosmos/cosmos-sdk/blob/9a16e2675f392b083dd1074ff92ff1f9fbda750d/docs/spec/addresses/bech32.md
                //TODO: support Cosmos Validator Consensus address & public key
                const Bech32Parameters::Bech32Struct COSMOS = {
                "cosmos",
                "cosmos",
                "1",
                6,
                {0x3b6a57b2ULL, 0x26508e6dULL, 0x1ea119faULL, 0x3d4233ddULL, 0x2a1462b3ULL},
                {0x01},
                {0x01}
        };

                const Bech32Parameters::Bech32Struct COSMOS_VAL = {
                "cosmos",
                "cosmosvaloper",
                "1",
                6,
                {0x3b6a57b2ULL, 0x26508e6dULL, 0x1ea119faULL, 0x3d4233ddULL, 0x2a1462b3ULL},
                {0x01},
                {0x01}
        };

                const Bech32Parameters::Bech32Struct COSMOS_PUB = {
                "cosmos",
                "cosmospub",
                "1",
                6,
                {0x3b6a57b2ULL, 0x26508e6dULL, 0x1ea119faULL, 0x3d4233ddULL, 0x2a1462b3ULL},
                {0x00},
                {0x00}
        };

                const Bech32Parameters::Bech32Struct COSMOS_PUB_VAL = {
                "cosmos",
                "cosmosvaloperpub",
                "1",
                6,
                {0x3b6a57b2ULL, 0x26508e6dULL, 0x1ea119faULL, 0x3d4233ddULL, 0x2a1462b3ULL},
                {0x00},
                {0x00}
        };

                class CosmosBech32 : public Bech32 {
                        public:
                                explicit CosmosBech32(api::CosmosBech32Type type, size_t offsetConversion = 1);

                                virtual ~CosmosBech32(){};

                                uint64_t polymod(const std::vector<uint8_t>& values) override;

                                std::vector<uint8_t> expandHrp(const std::string& hrp) override;

                                std::string encode(const std::vector<uint8_t>& hash,
                                                   const std::vector<uint8_t>& version) override;

                                std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
                                decode(const std::string& str) override;

                        protected:
                                size_t _offsetConversion;
                };
        }
}

#endif //LEDGER_CORE_COSMOSBECH32_H
