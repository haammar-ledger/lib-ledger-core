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
#include "Fixtures.hpp"

#include <wallet/cosmos/api_impl/CosmosLikeTransactionApi.hpp>
#include <wallet/cosmos/CosmosLikeCurrencies.hpp>
#include <wallet/cosmos/CosmosLikeWallet.hpp>
#include <wallet/cosmos/CosmosLikeAccount.hpp>

#include <api/CosmosLikeTransactionBuilder.hpp>
#include <api/CosmosLikeMessage.hpp>
#include <api/StringCallback.hpp>

#include <cosmos/CosmosLikeExtendedPublicKey.hpp>

#include <utils/hex.h>
#include <utils/DateUtils.hpp>

#include <gtest/gtest.h>

using namespace ledger::testing::cosmos;
using namespace ledger::core;

class CosmosTransactionTest : public BaseFixture {
public:
 void SetUp() override
 {
     BaseFixture::SetUp();
#ifdef PG_SUPPORT
     const bool usePostgreSQL = true;
     configuration->putString(
         api::PoolConfiguration::DATABASE_NAME, "postgres://localhost:5432/test_db");
     pool = newDefaultPool("postgres", "", configuration, usePostgreSQL);
#else
     pool = newDefaultPool();
#endif
     backend->enableQueryLogging(true);
 }

 void setupTest(std::shared_ptr<CosmosLikeAccount> &account,
                std::shared_ptr<CosmosLikeWallet> &wallet)
 {
     auto configuration = DynamicObject::newInstance();
     configuration->putString(
         api::Configuration::KEYCHAIN_DERIVATION_SCHEME,
         "44'/<coin_type>'/<account>'/<node>/<address>");
     wallet = std::dynamic_pointer_cast<CosmosLikeWallet>(
         wait(pool->createWallet("e847815f-488a-4301-b67c-378a5e9c8a61", "atom", configuration)));

     auto accountInfo = wait(wallet->getNextAccountCreationInfo());
     EXPECT_EQ(accountInfo.index, 0);
     accountInfo.publicKeys.push_back(hex::toByteArray(DEFAULT_HEX_PUB_KEY));

     account = createCosmosLikeAccount(wallet, accountInfo.index, accountInfo);
 }

 std::shared_ptr<WalletPool> pool;
};

TEST_F(CosmosTransactionTest, EncodeToJSON) {
    const auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgSend\",\"value\":{\"amount\":[{\"amount\":\"1000000\",\"denom\":\"uatom\"}],\"from_address\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"to_address\":\"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7\"}}],\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawSignedTransaction(currencies::ATOM, strTx);

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

TEST_F(CosmosTransactionTest, ParseRawSignedMsgSendTransaction) {
    const auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgSend\",\"value\":{\"amount\":{\"amount\":\"1000000\",\"denom\":\"uatom\"},\"from_address\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"to_address\":\"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7\"}}],\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawSignedTransaction(currencies::ATOM, strTx);

    //Put public key
    auto pubKeyBech32 = "cosmospub1addwnpepqtztanmggwrgm92kafpagegck5dp8jc6frxkcpdzrspfafprrlx7gmvhdq6";
    auto pubKey = CosmosLikeExtendedPublicKey::fromBech32(currencies::ATOM, pubKeyBech32, Option<std::string>("44'/118'/0'"))->derivePublicKey("");
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

TEST_F(CosmosTransactionTest, ParseRawMsgDelegateTransaction) {
    const auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgDelegate\",\"value\":{\"amount\":{\"amount\":\"1000000\",\"denom\":\"uatom\"},\"delegator_address\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"validator_address\":\"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7\"}}],\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(currencies::ATOM, strTx);

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

TEST_F(CosmosTransactionTest, ParseRawMsgUndelegateTransaction) {
    const auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgUndelegate\",\"value\":{\"amount\":{\"amount\":\"1000000\",\"denom\":\"uatom\"},\"delegator_address\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"validator_address\":\"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7\"}}],\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(currencies::ATOM, strTx);

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

TEST_F(CosmosTransactionTest, ParseRawMsgBeginRedelegateTransaction) {
    const auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgBeginRedelegate\",\"value\":{\"amount\":{\"amount\":\"1000000\",\"denom\":\"uatom\"},\"delegator_address\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"validator_dst_address\":\"cosmosvaloper1sd4tl9aljmmezzudugs7zlaya7pg2895ws8tfs\",\"validator_src_address\":\"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7\"}}],\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto redelegateMessage = api::CosmosLikeMessage::unwrapMsgBeginRedelegate(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5000L);
    EXPECT_EQ(tx->getGas()->toLong(), 200000L);
    EXPECT_EQ(redelegateMessage.delegatorAddress, "cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl");
    EXPECT_EQ(redelegateMessage.validatorSourceAddress, "cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7");
    EXPECT_EQ(redelegateMessage.validatorDestinationAddress, "cosmosvaloper1sd4tl9aljmmezzudugs7zlaya7pg2895ws8tfs");
    EXPECT_EQ(redelegateMessage.amount.amount, "1000000");
    EXPECT_EQ(redelegateMessage.amount.denom, "uatom");

    EXPECT_EQ(tx->serialize(), strTx);
}

TEST_F(CosmosTransactionTest, ParseRawMsgSubmitProposalTransaction) {
    const auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgSubmitProposal\",\"value\":{\"content\":{\"description\":\"My awesome proposal\",\"title\":\"Test Proposal\",\"type\":\"Text\"},\"initial_deposit\":[{\"amount\":\"1000000\",\"denom\":\"uatom\"}],\"proposer\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\"}}],\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(currencies::ATOM, strTx);

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

TEST_F(CosmosTransactionTest, ParseRawMsgVoteTransaction) {
    const auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgVote\",\"value\":{\"option\":\"YES\",\"proposal_id\":\"123\",\"voter\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\"}}],\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto voteMessage = api::CosmosLikeMessage::unwrapMsgVote(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5000L);
    EXPECT_EQ(tx->getGas()->toLong(), 200000L);
    EXPECT_EQ(to_string(voteMessage.option), "YES");
    EXPECT_EQ(voteMessage.proposalId, "123");
    EXPECT_EQ(voteMessage.voter, "cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl");

    EXPECT_EQ(tx->serialize(), strTx);
}

TEST_F(CosmosTransactionTest, ParseRawMsgDepositTransaction) {
    const auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgDeposit\",\"value\":{\"amount\":[{\"amount\":\"1000000\",\"denom\":\"uatom\"}],\"depositor\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"proposal_id\":\"123\"}}],\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(currencies::ATOM, strTx);

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

TEST_F(CosmosTransactionTest, ParseRawMsgWithdrawDelegationRewardTransaction) {
    const auto strTx = "{\"account_number\":\"6571\",\"chain_id\":\"cosmoshub-3\",\"fee\":{\"amount\":[{\"amount\":\"5000\",\"denom\":\"uatom\"}],\"gas\":\"200000\"},\"memo\":\"Sent from Ledger\",\"msgs\":[{\"type\":\"cosmos-sdk/MsgWithdrawDelegationReward\",\"value\":{\"delegator_address\":\"cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl\",\"validator_address\":\"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7\"}}],\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto withdrawMessage = api::CosmosLikeMessage::unwrapMsgWithdrawDelegationReward(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5000L);
    EXPECT_EQ(tx->getGas()->toLong(), 200000L);
    EXPECT_EQ(withdrawMessage.delegatorAddress, "cosmos102hty0jv2s29lyc4u0tv97z9v298e24t3vwtpl");
    EXPECT_EQ(withdrawMessage.validatorAddress, "cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7");

    EXPECT_EQ(tx->serialize(), strTx);
}


TEST_F(CosmosTransactionTest, ParseRawMsgMultiSendTransaction) {
    // From cosmos/cosmos-sdk tests :
    // https://github.com/cosmos/cosmos-sdk/blob/ebbfaf2a47d3e97a4720f643ca21d5a41676cdc0/x/bank/types/msgs_test.go#L217-L229
    const auto strTx = "{\"account_number\":\"6571\","
        "\"chain_id\":\"cosmoshub-3\","
        "\"fee\":{\"amount\":[{\"amount\":\"5001\",\"denom\":\"uatom\"}],\"gas\":\"200020\"},"
        "\"memo\":\"Sent from Ledger\","
        "\"msgs\":["
        "{\"type\":\"cosmos-sdk/MsgMultiSend\""
        ",\"value\":{\"inputs\":[{\"address\":\"cosmos1d9h8qat57ljhcm\",\"coins\":[{\"amount\":\"10\",\"denom\":\"uatom\"}]}]"
        ",\"outputs\":[{\"address\":\"cosmos1da6hgur4wsmpnjyg\",\"coins\":[{\"amount\":\"10\",\"denom\":\"atom\"}]}]}}"
        "],"
        "\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto multiSendMessage = api::CosmosLikeMessage::unwrapMsgMultiSend(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5001L);
    EXPECT_EQ(tx->getGas()->toLong(), 200020L);
    ASSERT_EQ(multiSendMessage.inputs.size(), 1);
    EXPECT_EQ(multiSendMessage.inputs[0].fromAddress, "cosmos1d9h8qat57ljhcm");
    EXPECT_EQ(multiSendMessage.inputs[0].coins.size(), 1);
    EXPECT_EQ(multiSendMessage.inputs[0].coins[0].amount, "10");
    EXPECT_EQ(multiSendMessage.inputs[0].coins[0].denom, "uatom");
    ASSERT_EQ(multiSendMessage.outputs.size(), 1);
    EXPECT_EQ(multiSendMessage.outputs[0].toAddress, "cosmos1da6hgur4wsmpnjyg");
    EXPECT_EQ(multiSendMessage.outputs[0].coins.size(), 1);
    EXPECT_EQ(multiSendMessage.outputs[0].coins[0].amount, "10");
    EXPECT_EQ(multiSendMessage.outputs[0].coins[0].denom, "atom");

    ASSERT_EQ(tx->serialize(), strTx);
}

TEST_F(CosmosTransactionTest, ParseRawMsgCreateValidatorTransaction) {
    // TODO : find a transaction in Explorer to confirm the format here
    // For the time being we're using protobuf from cosmos-sdk as source :
    // https://github.com/cosmos/cosmos-sdk/blob/53bf2271d5bac054a8f74723732f21055c1b72d4/x/staking/types/types.pb.go
    const auto strTx = "{\"account_number\":\"6571\","
        "\"chain_id\":\"cosmoshub-3\","
        "\"fee\":{\"amount\":[{\"amount\":\"5001\",\"denom\":\"uatom\"}],\"gas\":\"200020\"},"
        "\"memo\":\"Sent from Ledger\","
        "\"msgs\":["
        "{\"type\":\"cosmos-sdk/MsgCreateValidator\","
        "\"value\":{"
        "\"commission\":{\"max_change_rate\":\"0.05\",\"max_rate\":\"0.60\",\"rate\":\"0.45\",\"update_time\":\"2020-02-25T13:42:29Z\"},"
        "\"delegator_address\":\"cosmostest\","
        "\"description\":{\"details\":\"It flies well\\\\nnewline\",\"identity\":\"Pocket Monsters\",\"moniker\":\"Hélédelle\",\"website\":\"https://www.pokepedia.fr/H%C3%A9l%C3%A9delle\"},"
        "\"min_self_delegation\":\"1\","
        "\"pub_key\":\"0\","
        "\"validator_address\":\"cosmosvalopertest\","
        "\"value\":{\"amount\":\"1059860\",\"denom\":\"uatom\"}"
        "}}],"
        "\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto createValidatorMessage = api::CosmosLikeMessage::unwrapMsgCreateValidator(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5001L);
    EXPECT_EQ(tx->getGas()->toLong(), 200020L);
    EXPECT_EQ(createValidatorMessage.description.moniker, "Hélédelle");
    EXPECT_EQ(createValidatorMessage.description.identity.value(), "Pocket Monsters");
    EXPECT_EQ(createValidatorMessage.description.website.value(), "https://www.pokepedia.fr/H%C3%A9l%C3%A9delle");
    EXPECT_EQ(createValidatorMessage.description.details.value(), "It flies well\\nnewline");
    EXPECT_EQ(createValidatorMessage.commission.rates.rate, "0.45");
    EXPECT_EQ(createValidatorMessage.commission.rates.maxRate, "0.60");
    EXPECT_EQ(createValidatorMessage.commission.rates.maxChangeRate, "0.05");
    EXPECT_EQ(createValidatorMessage.commission.updateTime, DateUtils::fromJSON("2020-02-25T13:42:29Z"));
    EXPECT_EQ(createValidatorMessage.minSelfDelegation, "1");
    EXPECT_EQ(createValidatorMessage.delegatorAddress, "cosmostest");
    EXPECT_EQ(createValidatorMessage.validatorAddress, "cosmosvalopertest");
    EXPECT_EQ(createValidatorMessage.pubkey, "0");
    EXPECT_EQ(createValidatorMessage.value.amount, "1059860");
    EXPECT_EQ(createValidatorMessage.value.denom, "uatom");

    EXPECT_EQ(tx->serialize(), strTx);
}

TEST_F(CosmosTransactionTest, ParseRawMsgEditValidatorTransaction) {
    // TODO : find a transaction in Explorer to confirm the format here
    // For the time being we're using protobuf from cosmos-sdk as source :
    // https://github.com/cosmos/cosmos-sdk/blob/53bf2271d5bac054a8f74723732f21055c1b72d4/x/staking/types/types.pb.go
    const auto strTx = "{\"account_number\":\"6571\","
        "\"chain_id\":\"cosmoshub-3\","
        "\"fee\":{\"amount\":[{\"amount\":\"5001\",\"denom\":\"uatom\"}],\"gas\":\"200020\"},"
        "\"memo\":\"Sent from Ledger\","
        "\"msgs\":["
        "{\"type\":\"cosmos-sdk/MsgEditValidator\""
        ",\"value\":{"
        "\"commission_rate\":\"0.50\","
        "\"description\":{\"details\":\"Cachabouée\",\"identity\":\"évolution de Wailmer\",\"moniker\":\"Wailord\",\"website\":\"https://www.pokepedia.fr/Wailord\"},"
        "\"min_self_delegation\":\"800\","
        "\"validator_address\":\"cosmostest\""
        "}}],"
        "\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto editValidatorMessage = api::CosmosLikeMessage::unwrapMsgEditValidator(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5001L);
    EXPECT_EQ(tx->getGas()->toLong(), 200020L);
    ASSERT_TRUE(editValidatorMessage.description);
    EXPECT_EQ(editValidatorMessage.description.value().moniker, "Wailord");
    ASSERT_TRUE(editValidatorMessage.description.value().identity);
    EXPECT_EQ(editValidatorMessage.description.value().identity.value(), "évolution de Wailmer");
    ASSERT_TRUE(editValidatorMessage.description.value().website);
    EXPECT_EQ(editValidatorMessage.description.value().website.value(), "https://www.pokepedia.fr/Wailord");
    ASSERT_TRUE(editValidatorMessage.description.value().details);
    EXPECT_EQ(editValidatorMessage.description.value().details.value(), "Cachabouée");
    EXPECT_EQ(editValidatorMessage.validatorAddress, "cosmostest");
    ASSERT_TRUE(editValidatorMessage.commissionRate);
    EXPECT_EQ(editValidatorMessage.commissionRate.value(), "0.50");
    ASSERT_TRUE(editValidatorMessage.minSelfDelegation);
    EXPECT_EQ(editValidatorMessage.minSelfDelegation.value(), "800");

    EXPECT_EQ(tx->serialize(), strTx);
}

TEST_F(CosmosTransactionTest, ParseRawMsgSetWithdrawAddressTransaction) {
    // TODO : find a transaction in Explorer to confirm the format here
    const auto strTx = "{\"account_number\":\"6571\","
        "\"chain_id\":\"cosmoshub-3\","
        "\"fee\":{\"amount\":[{\"amount\":\"5001\",\"denom\":\"uatom\"}],\"gas\":\"200020\"},"
        "\"memo\":\"Sent from Ledger\","
        "\"msgs\":["
        "{\"type\":\"cosmos-sdk/MsgSetWithdrawAddress\""
        ",\"value\":{"
        "\"delegator_address\":\"cosmos1dafe\","
        "\"withdraw_address\":\"cosmos1erfdsa\""
        "}}],"
        "\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto setWithdrawAddressMessage = api::CosmosLikeMessage::unwrapMsgSetWithdrawAddress(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5001L);
    EXPECT_EQ(tx->getGas()->toLong(), 200020L);
    EXPECT_EQ(setWithdrawAddressMessage.delegatorAddress, "cosmos1dafe");
    EXPECT_EQ(setWithdrawAddressMessage.withdrawAddress, "cosmos1erfdsa");

    EXPECT_EQ(tx->serialize(), strTx);
}

TEST_F(CosmosTransactionTest, ParseRawMsgWithdrawDelegatorRewardsTransaction) {
    // TODO : find a transaction in Explorer to confirm the format here
    const auto strTx = "{\"account_number\":\"6571\","
        "\"chain_id\":\"cosmoshub-3\","
        "\"fee\":{\"amount\":[{\"amount\":\"5001\",\"denom\":\"uatom\"}],\"gas\":\"200020\"},"
        "\"memo\":\"Sent from Ledger\","
        "\"msgs\":["
        "{\"type\":\"cosmos-sdk/MsgWithdrawDelegatorReward\""
        ",\"value\":{"
        "\"delegator_address\":\"cosmos1targ\","
        "\"validator_address\":\"cosmosvaloperwdfae\""
        "}}],"
        "\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto withdrawDorRewardMessage = api::CosmosLikeMessage::unwrapMsgWithdrawDelegatorReward(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5001L);
    EXPECT_EQ(tx->getGas()->toLong(), 200020L);
    EXPECT_EQ(withdrawDorRewardMessage.delegatorAddress, "cosmos1targ");
    EXPECT_EQ(withdrawDorRewardMessage.validatorAddress, "cosmosvaloperwdfae");

    EXPECT_EQ(tx->serialize(), strTx);
}

TEST_F(CosmosTransactionTest, ParseRawMsgWithdrawValidatorCommissionTransaction) {
    // TODO : find a transaction in Explorer to confirm the format here
    const auto strTx = "{\"account_number\":\"6571\","
        "\"chain_id\":\"cosmoshub-3\","
        "\"fee\":{\"amount\":[{\"amount\":\"5001\",\"denom\":\"uatom\"}],\"gas\":\"200020\"},"
        "\"memo\":\"Sent from Ledger\","
        "\"msgs\":["
        "{\"type\":\"cosmos-sdk/MsgWithdrawValidatorCommission\""
        ",\"value\":{"
        "\"validator_address\":\"cosmosvaloper1234567890\""
        "}}],"
        "\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto withdrawVorCommissionMessage = api::CosmosLikeMessage::unwrapMsgWithdrawValidatorCommission(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5001L);
    EXPECT_EQ(tx->getGas()->toLong(), 200020L);
    EXPECT_EQ(withdrawVorCommissionMessage.validatorAddress, "cosmosvaloper1234567890");

    EXPECT_EQ(tx->serialize(), strTx);
}

TEST_F(CosmosTransactionTest, ParseRawMsgUnjailTransaction) {
    // TODO : find a transaction in Explorer to confirm the format here
    const auto strTx = "{\"account_number\":\"6571\","
        "\"chain_id\":\"cosmoshub-3\","
        "\"fee\":{\"amount\":[{\"amount\":\"5001\",\"denom\":\"uatom\"}],\"gas\":\"200020\"},"
        "\"memo\":\"Sent from Ledger\","
        "\"msgs\":["
        "{\"type\":\"cosmos-sdk/MsgUnjail\""
        ",\"value\":{"
        "\"validator_address\":\"cosmosvaloper1dalton\""
        "}}],"
        "\"sequence\":\"0\"}";
    const auto tx = api::CosmosLikeTransactionBuilder::parseRawUnsignedTransaction(currencies::ATOM, strTx);

    auto message = tx->getMessages().front();
    auto unjailMessage = api::CosmosLikeMessage::unwrapMsgUnjail(message);
    EXPECT_EQ(tx->getFee()->toLong(), 5001L);
    EXPECT_EQ(tx->getGas()->toLong(), 200020L);
    EXPECT_EQ(unjailMessage.validatorAddress, "cosmosvaloper1dalton");

    EXPECT_EQ(tx->serialize(), strTx);
}

// TODO Assert success
TEST_F(CosmosTransactionTest, BroadcastSendTransaction) {

    class JsonCallback : public api::StringCallback {
        public:
        void onCallback(const std::experimental::optional<std::string> & result, const std::experimental::optional<api::Error> & error) override {
            if (result) {
                std::cout << "Result: " << result.value() << std::endl;
            }
            if (error) {
                std::cout << "Error: " << api::to_string(error.value().code) << " " << error.value().message << std::endl;
            }
        }
    };

    std::shared_ptr<CosmosLikeAccount> account;
    std::shared_ptr<CosmosLikeWallet> wallet;
    setupTest(account, wallet);

    auto msg = setupSendMessage();
    auto tx = setupTransactionRequest(std::vector<Message>{ msg });

    account->broadcastTransaction(std::make_shared<CosmosLikeTransactionApi>(tx), std::make_shared<JsonCallback>());

    FAIL() << "(CONSIDERED FAILED UNTIL PROVEN SUCCESSFUL)";
}
