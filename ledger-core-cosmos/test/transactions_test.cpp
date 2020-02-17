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

TEST(CosmosTransaction, EncodeToJSON) {
    auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgSend\",\"value\":{\"amount\":[{\"amount\":\"1000000\",\"denom\":\"uatom\"}],\"from_address\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"to_address\":\"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7\"}}],\"sequence\":\"0\"}";
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

TEST(CosmosTransaction, ParseRawSignedMsgSendTransaction) {
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

TEST(CosmosTransaction, ParseRawMsgDelegateTransaction) {
    auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgDelegate\",\"value\":{\"amount\":{\"amount\":\"1000000\",\"denom\":\"uatom\"},\"delegator_address\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"validator_address\":\"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7\"}}],\"sequence\":\"0\"}";
    auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(ledger::core::currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto delegateMessage = api::CosmosLikeMessage::unwrapMsgDelegate(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5000L);
    EXPECT_EQ(tx->getGas()->toLong(), 200000L);
    EXPECT_EQ(delegateMessage.delegatorAddress, "cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl");
    EXPECT_EQ(delegateMessage.validatorAddress, "cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7");
    EXPECT_EQ(delegateMessage.amount.amount, "1000000");
    EXPECT_EQ(delegateMessage.amount.denom, "uatom");

    EXPECT_EQ(tx->serialize(), strTx);
}

TEST(CosmosTransaction, ParseRawMsgUndelegateTransaction) {
    auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgUndelegate\",\"value\":{\"amount\":{\"amount\":\"1000000\",\"denom\":\"uatom\"},\"delegator_address\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"validator_address\":\"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7\"}}],\"sequence\":\"0\"}";
    auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(ledger::core::currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto undelegateMessage = api::CosmosLikeMessage::unwrapMsgUndelegate(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5000L);
    EXPECT_EQ(tx->getGas()->toLong(), 200000L);
    EXPECT_EQ(undelegateMessage.delegatorAddress, "cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl");
    EXPECT_EQ(undelegateMessage.validatorAddress, "cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7");
    EXPECT_EQ(undelegateMessage.amount.amount, "1000000");
    EXPECT_EQ(undelegateMessage.amount.denom, "uatom");

    EXPECT_EQ(tx->serialize(), strTx);
}

TEST(CosmosTransaction, ParseRawMsgRedelegateTransaction) {
    auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgBeginRedelegate\",\"value\":{\"amount\":{\"amount\":\"1000000\",\"denom\":\"uatom\"},\"delegator_address\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"validator_dst_address\":\"cosmosvaloper1sd4tl9aljmmezzudugs7zlaya7pg2895ws8tfs\",\"validator_src_address\":\"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7\"}}],\"sequence\":\"0\"}";
    auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(ledger::core::currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto redelegateMessage = api::CosmosLikeMessage::unwrapMsgRedelegate(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5000L);
    EXPECT_EQ(tx->getGas()->toLong(), 200000L);
    EXPECT_EQ(redelegateMessage.delegatorAddress, "cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl");
    EXPECT_EQ(redelegateMessage.validatorSourceAddress, "cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7");
    EXPECT_EQ(redelegateMessage.validatorDestinationAddress, "cosmosvaloper1sd4tl9aljmmezzudugs7zlaya7pg2895ws8tfs");
    EXPECT_EQ(redelegateMessage.amount.amount, "1000000");
    EXPECT_EQ(redelegateMessage.amount.denom, "uatom");

    EXPECT_EQ(tx->serialize(), strTx);
}

TEST(CosmosTransaction, ParseRawMsgSubmitProposalTransaction) {
    auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgSubmitProposal\",\"value\":{\"content\":{\"description\":\"My awesome proposal\",\"title\":\"Test Proposal\",\"type\":\"Text\"},\"initial_deposit\":[{\"amount\":\"1000000\",\"denom\":\"uatom\"}],\"proposer\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\"}}],\"sequence\":\"0\"}";
    auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(ledger::core::currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto submitProposalMessage = api::CosmosLikeMessage::unwrapMsgSubmitProposal(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5000L);
    EXPECT_EQ(tx->getGas()->toLong(), 200000L);
    EXPECT_EQ(submitProposalMessage.proposer, "cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl");
    EXPECT_EQ(submitProposalMessage.content.title, "Test Proposal");
    EXPECT_EQ(submitProposalMessage.content.description, "My awesome proposal");
    EXPECT_EQ(submitProposalMessage.initialDeposit.size(), 1);
    EXPECT_EQ(submitProposalMessage.initialDeposit.front().amount, "1000000");
    EXPECT_EQ(submitProposalMessage.initialDeposit.front().denom, "uatom");

    EXPECT_EQ(tx->serialize(), strTx);
}

TEST(CosmosTransaction, ParseRawMsgVoteTransaction) {
    auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgVote\",\"value\":{\"option\":\"YES\",\"proposal_id\":\"123\",\"voter\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\"}}],\"sequence\":\"0\"}";
    auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(ledger::core::currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto voteMessage = api::CosmosLikeMessage::unwrapMsgVote(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5000L);
    EXPECT_EQ(tx->getGas()->toLong(), 200000L);
    EXPECT_EQ(to_string(voteMessage.option), "YES");
    EXPECT_EQ(voteMessage.proposalId, "123");
    EXPECT_EQ(voteMessage.voter, "cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl");

    EXPECT_EQ(tx->serialize(), strTx);
}

TEST(CosmosTransaction, ParseRawMsgDepositTransaction) {
    auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgDeposit\",\"value\":{\"amount\":[{\"amount\":\"1000000\",\"denom\":\"uatom\"}],\"depositor\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"proposal_id\":\"123\"}}],\"sequence\":\"0\"}";
    auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(ledger::core::currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto depositMessage = api::CosmosLikeMessage::unwrapMsgDeposit(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5000L);
    EXPECT_EQ(tx->getGas()->toLong(), 200000L);
    EXPECT_EQ(depositMessage.proposalId, "123");
    EXPECT_EQ(depositMessage.depositor, "cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl");
    EXPECT_EQ(depositMessage.amount.size(), 1);
    EXPECT_EQ(depositMessage.amount.front().amount, "1000000");
    EXPECT_EQ(depositMessage.amount.front().denom, "uatom");

    EXPECT_EQ(tx->serialize(), strTx);
}

TEST(CosmosTransaction, ParseRawMsgWithdrawDelegationRewardTransation) {
    auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgWithdrawDelegationReward\",\"value\":{\"delegator_address\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"validator_address\":\"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7\"}}],\"sequence\":\"0\"}";
    auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(ledger::core::currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto withdrawMessage = api::CosmosLikeMessage::unwrapMsgWithdrawDelegationReward(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5000L);
    EXPECT_EQ(tx->getGas()->toLong(), 200000L);
    EXPECT_EQ(withdrawMessage.delegatorAddress, "cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl");
    EXPECT_EQ(withdrawMessage.validatorAddress, "cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7");

    EXPECT_EQ(tx->serialize(), strTx);
}

