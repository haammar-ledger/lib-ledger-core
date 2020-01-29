/*
 *
 * transactions_test
 *
 * Created by El Khalil Bellakrid on 11/06/2019.
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
#include <gtest/gtest.h>
#include <rapidjson/reader.h>
#include <rapidjson/writer.h>

#include <core/utils/Hex.hpp>
#include <core/collections/Vector.hpp>
#include <core/utils/Either.hpp>
#include <core/bytes/BytesWriter.hpp>
#include <core/crypto/SHA256.hpp>

#include <cosmos/CosmosLikeCurrencies.hpp>
#include <cosmos/bech32/CosmosBech32.hpp>
#include <cosmos/api/CosmosLikeTransactionBuilder.hpp>
#include <cosmos/api/CosmosLikeTransaction.hpp>
#include <cosmos/api/CosmosLikeAddress.hpp>
#include <cosmos/CosmosLikeCurrencies.hpp>
#include <cosmos/CosmosLikeExtendedPublicKey.hpp>
#include <cosmos/api_impl/CosmosLikeTransactionApi.hpp>
#include <cosmos/CosmosLikeMessage.hpp>

using namespace ledger::core::api;
using namespace ledger::core;

TEST(CosmosTransaction, Encode) {
    /*
     * Original Tx
     *
        {
            "type":"auth/StdTx",
            "value":
                {
                    "msg":[
                        {
                            "type":"cosmos-sdk/MsgSend",
                            "value":{
                                "from_address":"cosmos1rpsdl0wdk42jjf9sp57d3eflm8q8ektuz66at3",
                                "to_address":"cosmos1e4lhppa79t40eqdgl5her60p4zqtenyj839y8z",
                                "amount":[
                                    {
                                        "denom":"muon",
                                        "amount":"1000"
                                    }
                                ]
                            }
                        }
                    ],
                    "fee":{
                        "amount":[
                            {
                                "denom":"muon",
                                "amount":"5000"
                            }
                        ],
                        "gas":"200000"
                    },
                    "signatures":[
                        {
                            "pub_key":
                                {
                                    "type":"tendermint/PubKeySecp256k1",
                                    "value":"A9dZ7e2azCHXp9hoTbXxTfiH2elBIet4Crj/4t5vecX0"
                                },
                            "signature":"TSNafIKca3+hEA48Fbf66gPJJscXkVvqactKHZOMPnBzB0PtNeHSFXWPl1lr1OR5p01V6eaz8zuJH7ukvTRGog=="
                        }
                    ],
                    "memo":"userid001"
                }
        }
        // Reference : https://github.com/cosmos/cosmos-sdk/blob/dc84c99deeef2154555454e1339ea2418939cee7/x/auth/types/stdtx.go
        {
            type : string, // e.g. auth/StdTx
            value: StdTx
            {
                msg: sdk.Msg : [
                    type: string, // e.g. cosmos-sdk/MsgSend
                    value:
                    {
                        from_address: bytes, //sdk.AccAddress is byte array
                        to_address: bytes,
                        amount: sdk.Coin
                        [
                            denom: string,
                            amount: int
                        ],
                    }
                ],
                fee: StdFee :
                {
                    amount: sdk.Coin
                    [
                        denom: string,
                        amount: int
                    ],
                    gas: uint64
                },
                signatures: StdSignature
                [
                    pub_key: crypto.PubKey // optional (https://github.com/cosmos/cosmos-sdk/blob/dc84c99deeef2154555454e1339ea2418939cee7/x/auth/types/stdtx.go#L190)
                    {
                        type: string, // e.g. tendermint/PubKeySecp256k1
                        value: bytes, // Amino encoding of public key i.e. encodeBase64(<pubKeyBytesLength><pubKeyBytes>)
                    },
                    signature: bytes
                ],
                memo: string
            }
        }
     */

    //TODO: figure out format of gas (for the moment it's written as VarInt)
    auto rawTx = "zwHwYl3uCj6oo2GaChQYYN+9zbVVKSSwDTzY5T/ZwHzZfBIUzX9wh74q6vyBqP0vkenhqIC8zJIaDAoEbXVvbhIEMTAwMBISCgwKBG11b24SBDUwMDAQwJoMGmoKJuta6YchA9dZ7e2azCHXp9hoTbXxTfiH2elBIet4Crj/4t5vecX0EkBNI1p8gpxrf6EQDjwVt/rqA8kmxxeRW+ppy0odk4w+cHMHQ+014dIVdY+XWWvU5HmnTVXp5rPzO4kfu6S9NEaiIgl1c2VyaWQwMDE=";
    //TODO
    // Should be cf01f0625dee0a3ea8a3619a0a141860dfbdcdb5552924b00d3cd8e53fd9c07cd97c1214cd7f7087be2aeafc81a8fd2f91e9e1a880bccc921a0c0a046d756f6e12043130303012120a0c0a046d756f6e12043530303010c09a0c1a6a0a26eb5ae9872103d759eded9acc21d7a7d8684db5f14df887d9e94121eb780ab8ffe2de6f79c5f412404d235a7c829c6b7fa1100e3c15b7faea03c926c717915bea69cb4a1d938c3e70730743ed35e1d215758f97596bd4e479a74d55e9e6b3f33b891fbba4bd3446a22209757365726964303031
    // TODO: figure out encoding of gas price and replace with above value
    auto rawTxHex = "d001f0625dee0ac9a8a3619a0a141860dfbdcdb5552924b00d3cd8e53fd9c07cd97c1214cd7f7087be2aeafc81a8fd2f91e9e1a880bccc921a0b0a046d756f6e123130303012130a0b0a046d756f6e123530303010fe400d03001a6a0a26eb5ae9872103d759eded9acc21d7a7d8684db5f14df887d9e94121eb780ab8ffe2de6f79c5f412404d235a7c829c6b7fa1100e3c15b7faea03c926c717915bea69cb4a1d938c3e70730743ed35e1d215758f97596bd4e479a74d55e9e6b3f33b891fbba4bd3446a22209757365726964303031";

    // Reference : https://github.com/tendermint/go-amino/blob/master/amino.go#L98
    enum Types {
        VARINT = 0,
        BYTE8 = 1,
        BYTELENGTH = 2,
        BYTE4 = 5
    };

    auto getCoin = [] (const std::string &denom, const std::string &amount) -> std::vector<uint8_t> {
        BytesWriter wr;
        std::vector<uint8_t> vDenom{denom.data(), denom.data() + denom.size()};
        return wr.writeVarInt(1 << 3 | BYTELENGTH).writeVarInt(vDenom.size()).writeByteArray(vDenom)
                .writeVarInt(2 << 3 | BYTELENGTH).writeByteArray(std::vector<uint8_t>(amount.begin(), amount.end()))
                .toByteArray();
    };

    BytesWriter tmpWr;
    auto bech32 = std::make_shared<CosmosBech32>(api::CosmosBech32Type::ADDRESS);

    auto stdAmount = getCoin("muon", "1000");

    //From address
    auto from = bech32->decode("cosmos1rpsdl0wdk42jjf9sp57d3eflm8q8ektuz66at3");
    auto to = bech32->decode("cosmos1e4lhppa79t40eqdgl5her60p4zqtenyj839y8z");
    auto sdkMessageValue = tmpWr.writeVarInt(1 << 3 | BYTELENGTH).writeVarInt(from.second.size()).writeByteArray(from.second)
            .writeVarInt(2 << 3 | BYTELENGTH).writeVarInt(to.second.size()).writeByteArray(to.second)
            .writeVarInt(3 << 3 | BYTELENGTH).writeVarInt(stdAmount.size()).writeByteArray(stdAmount)
            .toByteArray();
    tmpWr.reset();

    //Message
    std::string messageSDK = "cosmos-sdk/MsgSend";
    // TODO: drop leading zeros
    // Getting prefix and disambiguity
    auto messageHash = SHA256::stringToBytesHash(messageSDK);
    auto sdkMessage = tmpWr.writeByteArray(std::vector<uint8_t>(messageHash.begin() + 3, messageHash.begin() + 7))
            .writeByteArray(sdkMessageValue)
            .toByteArray();
    tmpWr.reset();


    // Fees
    auto feesObject = getCoin("muon", "5000");

    uint32_t gas = 200000;
    auto stdFees = tmpWr.writeVarInt(1 << 3 | BYTELENGTH).writeVarInt(feesObject.size()).writeByteArray(feesObject)
            .writeVarInt(2 << 3 | VARINT).writeVarInt(gas)
            .toByteArray();
    tmpWr.reset();

    std::string signatureType = "tendermint/PubKeySecp256k1";
    auto signatureTypeHash = SHA256::stringToBytesHash(signatureType);
    auto pubKey = hex::toByteArray("03d759eded9acc21d7a7d8684db5f14df887d9e94121eb780ab8ffe2de6f79c5f4");
    auto pubKeyObject = tmpWr.writeByteArray(std::vector<uint8_t>(signatureTypeHash.begin() + 3, signatureTypeHash.begin() + 7))
            .writeVarInt(pubKey.size()).writeByteArray(pubKey)
            .toByteArray();
    tmpWr.reset();

    auto signature = hex::toByteArray("4d235a7c829c6b7fa1100e3c15b7faea03c926c717915bea69cb4a1d938c3e70730743ed35e1d215758f97596bd4e479a74d55e9e6b3f33b891fbba4bd3446a2");
    auto signatureObject = tmpWr.writeVarInt(1 << 3 | BYTELENGTH).writeVarInt(pubKeyObject.size()).writeByteArray(pubKeyObject)
            .writeVarInt(2 << 3 | BYTELENGTH).writeVarInt(signature.size()).writeByteArray(signature)
            .toByteArray();
    tmpWr.reset();

    auto stdSignature = tmpWr.writeVarInt(signatureObject.size()).writeByteArray(signatureObject)
            .toByteArray();
    tmpWr.reset();

    std::string memo = "userid001";
    auto memoObject = tmpWr.writeVarString(memo).toByteArray();
    tmpWr.reset();

    auto stdTx = tmpWr.writeByteArray(sdkMessage)
            .writeVarInt(2 << 3 | BYTELENGTH).writeVarInt(stdFees.size()).writeByteArray(stdFees)
            .writeVarInt(3 << 3 | BYTELENGTH).writeByteArray(stdSignature)
            .writeVarInt(4 << 3 | BYTELENGTH).writeByteArray(memoObject)
            .toByteArray();
    tmpWr.reset();


    std::string type = "auth/StdTx";
    auto typeHash = SHA256::stringToBytesHash(type);

    auto tx = tmpWr.writeVarInt(0 << 3 | BYTE8).writeByteArray(std::vector<uint8_t>(typeHash.begin() + 3, typeHash.begin() + 7))
            .writeVarInt(1 << 3 | BYTELENGTH).writeVarInt(stdTx.size()).writeByteArray(stdTx)
            .toByteArray();
    tmpWr.reset();

    auto finalResult = tmpWr.writeVarInt(tx.size()).writeByteArray(tx).toByteArray();


    EXPECT_EQ(hex::toString(finalResult), rawTxHex);

}

TEST(CosmosTransaction, EncodeToJSON) {
    auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgSend\",\"value\":{\"amount\":{\"amount\":\"1000000\",\"denom\":\"uatom\"},\"from_address\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"to_address\":\"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7\"}}],\"sequence\":\"0\"}";
    auto tx = api::CosmosLikeTransactionBuilder::parseRawSignedTransaction(ledger::core::currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto sendMessage = api::CosmosLikeMessage::unwrapMsgSend(message);
    // ensure the values are correct
   EXPECT_EQ(tx->getFee()->toLong(), 5000L);
   EXPECT_EQ(tx->getGas()->toLong(), 200000L);
   EXPECT_EQ(sendMessage.fromAddress, "cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl");
   EXPECT_EQ(sendMessage.toAddress, "cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7");
   EXPECT_EQ(sendMessage.amount.size(), 1);
   EXPECT_EQ(sendMessage.amount.front().amount, "1000000");
   EXPECT_EQ(sendMessage.amount.front().denom, "uatom");

   EXPECT_EQ(tx->serialize(), strTx);
}

TEST(CosmosTransaction, ParseSignedRawMsgSendTransaction) {
    auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgSend\",\"value\":{\"amount\":{\"amount\":\"1000000\",\"denom\":\"uatom\"},\"from_address\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"to_address\":\"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7\"}}],\"sequence\":\"0\"}";
    auto tx = api::CosmosLikeTransactionBuilder::parseRawSignedTransaction(ledger::core::currencies::ATOM, strTx);

    //Put public key
    auto pubKeyBech32 = "cosmospub1addwnpepqtztanmggwrgm92kafpagegck5dp8jc6frxkcpdzrspfafprrlx7gmvhdq6";
    auto pubKey = ledger::core::CosmosLikeExtendedPublicKey::fromBech32(ledger::core::currencies::ATOM, pubKeyBech32, Option<std::string>("44'/118'/0'"))->derivePublicKey("");
    std::dynamic_pointer_cast<CosmosLikeTransactionApi>(tx)->setSigningPubKey(pubKey);

    //Put signature
    auto sSignature = "3045022100f4d9fb033eb6bc32d6d1b32075c3b6ea4cce795aed77f3341b15c5b1e3e085c802207b958d3df288bafcc167af4ccaa1eca83f12d48c22129fa251be9549db65a606";
    auto signature = hex::toByteArray(sSignature);
    tx->setDERSignature(signature);

    // clang-format off
    // Disable formatters here so the structure of the expected json is easier to read
    EXPECT_EQ(tx->serialize(),
              "{\"account_number\":\"6571\","
              "\"chain_id\":\"cosmoshub-3\","
              "\"fee\":{"
                  "\"amount\":[{"
                      "\"amount\":\"5000\","
                      "\"denom\":\"uatom\"}],"
                  "\"gas\":\"200000\"},"
              "\"memo\":\"Sent from Ledger\","
              "\"msgs\":[{"
                  "\"type\":\"cosmos-sdk/MsgSend\","
                  "\"value\":{"
                      "\"amount\":[{"
                          "\"amount\":\"1000000\","
                          "\"denom\":\"uatom\"}],"
                      "\"from_address\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\","
                      "\"to_address\":\"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7\"}}],"
              "\"sequence\":\"0\","
              "\"signature\":[{"
                  "\"pub_key\":{"
                      "\"type\":\"tendermint/PubKeySecp256k1\","
                      "\"value\":\"AsS+z2hDho2VVupD1GUYtRoTyxpIzWwFohwCnqQjH83k\"},"
                  "\"signature\":\"9Nn7Az62vDLW0bMgdcO26kzOeVrtd/M0GxXFsePghch7lY098oi6/MFnr0zKoeyoPxLUjCISn6JRvpVJ22WmBg==\"}]}") << "The keys in dictionnaries must be lexicographically sorted!";
              // clang-format on
}
