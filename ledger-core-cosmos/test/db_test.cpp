#include "Fixtures.hpp"

#include <cosmos/database/CosmosLikeTransactionDatabaseHelper.hpp>
#include <cosmos/CosmosLikeOperationQuery.hpp>
#include <cosmos/CosmosLikeWallet.hpp>
#include <cosmos/CosmosLikeCurrencies.hpp>
#include <cosmos/cosmos.hpp>
//#include <cosmos/api/CosmosLikeTransaction.hpp>
//#include <cosmos/api_impl/CosmosLikeTransactionApi.hpp>
//#include <cosmos/api/CosmosLikeAmount.hpp>
//#include <integration/BaseFixture.hpp>
//#include <core/api/Amount.hpp>
#include <core/Services.hpp>
#include <core/utils/DateUtils.hpp>

#include <gtest/gtest.h>

using namespace ledger::core::cosmos;

class CosmosDBTests : public BaseFixture {
public:
    void SetUp() override {
        BaseFixture::SetUp();
        //backend->enableQueryLogging(true);
    }

    void setupTest(std::shared_ptr<Services>& services,
                   std::shared_ptr<CosmosLikeAccount>& account,
                   std::shared_ptr<CosmosLikeWallet>& wallet) {

        services = newDefaultServices();

        auto walletStore = newWalletStore(services);
        wait(walletStore->addCurrency(currencies::ATOM));

        auto factory = std::make_shared<CosmosLikeWalletFactory>(currencies::ATOM, services);
        walletStore->registerFactory(currencies::ATOM, factory);

        auto configuration = DynamicObject::newInstance();
        //configuration->putString(api::Configuration::KEYCHAIN_DERIVATION_SCHEME, "44'/<coin_type>'/<account>'/<node>/<address>");
        wallet = std::dynamic_pointer_cast<CosmosLikeWallet>(
                        wait(walletStore->createWallet("e847815f-488a-4301-b67c-378a5e9c8a61", "atom", configuration)));

        auto accountInfo = wait(wallet->getNextAccountCreationInfo());
        EXPECT_EQ(accountInfo.index, 0);
        accountInfo.publicKeys.push_back(hex::toByteArray(ledger::testing::cosmos::DEFAULT_HEX_PUB_KEY));

        account = ledger::testing::cosmos::createCosmosLikeAccount(wallet, accountInfo.index, accountInfo);

    }

    void setupTestData(Transaction& tx, std::chrono::system_clock::time_point& timeRef) {
        tx.hash = "A1E44688B429AF17322EC33CE62876FA415EFC8D9244A2F51454BD025F416594";
        api::Block block;
        block.blockHash = "52B39D45B438C6995CD448B09963954883B0F7A57E9EFC7A95E0A6C5BAC09C00";
        block.currencyName = "atom";
        block.height = 744795;
        block.time = timeRef;
        tx.block = block;
        tx.fee.gas = BigInt(30000);
        tx.fee.amount.emplace_back("30", "uatom");
        tx.gasUsed = BigInt(26826);
        tx.timestamp = timeRef;
        tx.memo = "Sent by Ledger";
        Message msg;
        msg.type = "cosmos-sdk/MsgSend";
        MsgSend sendMsg;
        sendMsg.fromAddress = "cosmos155svs6sgxe55rnvs6ghprtqu0mh69kehrn0dqr";
        sendMsg.toAddress = "cosmos1sd4tl9aljmmezzudugs7zlaya7pg2895tyn79r";
        sendMsg.amount.emplace_back("900000", "uatom");
        msg.content = sendMsg;
        tx.messages.push_back(msg);
        MessageLog log;
        log.messageIndex = 0;
        log.success = true;
        log.log = "Success";
        tx.logs.push_back(log);
    }

};

// TODO
// - cleanup
// - reorder files

TEST_F(CosmosDBTests, CosmosDBTest) {

    std::shared_ptr<Services> services;
    std::shared_ptr<CosmosLikeAccount> account;
    std::shared_ptr<CosmosLikeWallet> wallet;
    setupTest(services, account, wallet);

    std::chrono::system_clock::time_point timeRef = DateUtils::now();

    Transaction tx;
    setupTestData(tx, timeRef);
    /*
    tx.hash = "A1E44688B429AF17322EC33CE62876FA415EFC8D9244A2F51454BD025F416594";
    api::Block block;
    block.blockHash = "52B39D45B438C6995CD448B09963954883B0F7A57E9EFC7A95E0A6C5BAC09C00";
    block.currencyName = "atom";
    block.height = 744795;
    block.time = timeRef;
    tx.block = block;
    tx.fee.gas = BigInt(30000);
    tx.fee.amount.emplace_back("30", "uatom");
    tx.gasUsed = BigInt(26826);
    tx.timestamp = timeRef;
    tx.memo = "Sent by Ledger";
    Message msg;
    msg.type = "cosmos-sdk/MsgSend";
    MsgSend sendMsg;
    sendMsg.fromAddress = "cosmos155svs6sgxe55rnvs6ghprtqu0mh69kehrn0dqr";
    sendMsg.toAddress = "cosmos1sd4tl9aljmmezzudugs7zlaya7pg2895tyn79r";
    sendMsg.amount.emplace_back("900000", "uatom");
    msg.content = sendMsg;
    tx.messages.push_back(msg);
    MessageLog log;
    log.messageIndex = 0;
    log.success = true;
    log.log = "Success";
    tx.logs.push_back(log);
    */

    // Test writing into DB
    {
        soci::session sql(services->getDatabaseSessionPool()->getPool());
        CosmosLikeTransactionDatabaseHelper::putTransaction(sql, account->getAccountUid(), tx);
    }

    // Test reading from DB
    {
        soci::session sql(services->getDatabaseSessionPool()->getPool());
        Transaction txRetrieved;
        auto result = CosmosLikeTransactionDatabaseHelper::getTransactionByHash(sql, tx.hash, txRetrieved);
        EXPECT_EQ(result, true);
        EXPECT_EQ(txRetrieved.hash, tx.hash);
        EXPECT_EQ(txRetrieved.block.getValue().blockHash, tx.block.getValue().blockHash);
        EXPECT_EQ(txRetrieved.block.getValue().currencyName, tx.block.getValue().currencyName);
        EXPECT_EQ(txRetrieved.block.getValue().height, tx.block.getValue().height);
        EXPECT_EQ(txRetrieved.block.getValue().time, tx.block.getValue().time);
        EXPECT_EQ(txRetrieved.fee.amount.size(), 1);
        EXPECT_EQ(txRetrieved.fee.amount[0].amount, tx.fee.amount[0].amount);
        EXPECT_EQ(txRetrieved.fee.amount[0].denom, tx.fee.amount[0].denom);
        EXPECT_EQ(txRetrieved.fee.gas, tx.fee.gas);
        EXPECT_EQ(txRetrieved.gasUsed.getValue().to_string(), tx.gasUsed.getValue().to_string());
        EXPECT_EQ(txRetrieved.timestamp, timeRef);
        EXPECT_EQ(txRetrieved.messages.size(), 1);
        EXPECT_EQ(txRetrieved.messages[0].type, tx.messages[0].type);
        EXPECT_EQ(txRetrieved.memo, tx.memo);
        EXPECT_EQ(txRetrieved.logs.size(), 1);
        EXPECT_EQ(txRetrieved.logs[0].success, true);

        // TODO Test other (all?) message types
        auto sendMsg = boost::get<MsgSend>(tx.messages[0].content);
        auto sendMsgRetrieved = boost::get<MsgSend>(txRetrieved.messages[0].content);
        EXPECT_EQ(sendMsgRetrieved.fromAddress, sendMsg.fromAddress);
        EXPECT_EQ(sendMsgRetrieved.toAddress, sendMsg.toAddress);
        EXPECT_EQ(sendMsgRetrieved.amount.size(), 1);
        EXPECT_EQ(sendMsgRetrieved.amount[0].amount, sendMsg.amount[0].amount);
        EXPECT_EQ(sendMsgRetrieved.amount[0].denom, sendMsg.amount[0].denom);
    }

    // TODO Test retreiving operations
    /*{
        //std::vector<std::shared_ptr<api::Operation>>
        auto ops = wait(std::dynamic_pointer_cast<CosmosLikeOperationQuery>(account->queryOperations()->complete())->execute());
        EXPECT_EQ(ops.size(), 1);
        auto op = ops[0];

        // TODO Test retrieved operation
    }*/
}

TEST_F(CosmosDBTests, CosmosOperationQueryTest) {
    std::shared_ptr<Services> services;
    std::shared_ptr<CosmosLikeAccount> account;
    std::shared_ptr<CosmosLikeWallet> wallet;
    setupTest(services, account, wallet);

    std::chrono::system_clock::time_point timeRef = DateUtils::now();

    Transaction tx;
    setupTestData(tx, timeRef);

    {
        soci::session sql(services->getDatabaseSessionPool()->getPool());
        CosmosLikeTransactionDatabaseHelper::putTransaction(sql, account->getAccountUid(), tx);
    }

    /*{
        //std::vector<std::shared_ptr<api::Operation>>
        auto ops = wait(std::dynamic_pointer_cast<CosmosLikeOperationQuery>(account->queryOperations()->complete())->execute());
        EXPECT_EQ(ops.size(), 1);
        auto op = ops[0];

        // TODO Test retrieved operation
    }*/
}