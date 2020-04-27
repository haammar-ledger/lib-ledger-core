/*
 * AlgorandAddress
 *
 * Created by Hakim Aammar on 20/04/2020.
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

#include <algorand/AlgorandAddress.hpp>

#include <core/collections/Vector.hpp>
#include <core/crypto/SHA512256.hpp>
#include <core/math/BaseConverter.hpp>


namespace ledger {
    namespace core {

        AlgorandAddress::AlgorandAddress(const std::vector<uint8_t> & pubKey) :
            Address(
                currencies::algorand(),
                optional<std::string>("")),
            _publicKey(pubKey)
        {
            _address = AlgorandAddress::fromPublicKey(pubKey);
        }

        std::string AlgorandAddress::toString() {
            return _address;
        }

        std::vector<uint8_t> AlgorandAddress::getPublicKey() {
            return _publicKey;
        }

        // FIXME Test this
        std::string AlgorandAddress::fromPublicKey(const std::vector<uint8_t> & pubKey) {
            // 1. pubkey --> pubKeyHash
            const std::vector<uint8_t> pubKeyHash = SHA512256::bytesToBytesHash(pubKey);
            // 2. 4 last bytes of pubKeyHash
            const std::vector<uint8_t> pubKeyHashChecksum(pubKeyHash.cbegin() + pubKeyHash.size() - CHECKSUM_LEN_BYTES, pubKeyHash.cend());
            // 3. pubkey + 4 last bytes of pubKeyHash
            const std::vector<uint8_t> addressBytes = vector::concat<uint8_t>(pubKey, pubKeyHashChecksum);
            // 4. Encode to Base32
            return BaseConverter::encode(addressBytes, BaseConverter::BASE32_RFC4648_NO_PADDING);
        }

        // FIXME Test this
        std::vector<uint8_t> AlgorandAddress::toPublicKey(const std::string & address) {
            std::vector<uint8_t> addressBytes;
            // 1. Decode from Base32
            BaseConverter::decode(address, BaseConverter::BASE32_RFC4648_NO_PADDING, addressBytes);
            // 2. Strip last 4 bytes
            return std::vector<uint8_t>(addressBytes.cbegin(), addressBytes.cbegin() + addressBytes.size() - CHECKSUM_LEN_BYTES);
        }
    }
}
