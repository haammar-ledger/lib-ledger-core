/*
 * AlgorandSynchronizationTests
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

#include "AlgorandTestFixtures.hpp"
#include <algorand/AlgorandAccount.hpp>
#include <algorand/AlgorandWalletFactory.hpp>
#include <algorand/AlgorandWallet.hpp>
//#include <algorand/AlgorandBlockchainExplorer.hpp>
#include <algorand/AlgorandLikeCurrencies.hpp>
#include <algorand/AlgorandNetworks.hpp>
#include <algorand/operations/AlgorandOperationQuery.hpp>
#include <algorand/api/AlgorandConfigurationDefaults.hpp>
#include <algorand/api/AlgorandBlockchainExplorerEngines.hpp>
#include <core/api/Configuration.hpp>

#include <integration/WalletFixture.hpp>

#include <functional>

/*
#include <gtest/gtest.h>
#include <iostream>
#include <set>

#include <core/api/BlockchainExplorerEngines.hpp>
#include <core/api/KeychainEngines.hpp>
#include <core/utils/DateUtils.hpp>

#include <tezos/TezosLikeWallet.hpp>
#include <tezos/api/TezosConfiguration.hpp>
#include <tezos/api/TezosConfigurationDefaults.hpp>
#include <tezos/database/TezosLikeAccountDatabaseHelper.hpp>
#include <tezos/delegation/TezosLikeOriginatedAccount.hpp>
#include <tezos/operations/TezosLikeOperation.hpp>
#include <tezos/operations/TezosLikeOperationQuery.hpp>
#include <tezos/transactions/TezosLikeTransactionBuilder.hpp>

#include <integration/WalletFixture.hpp>

#include "Common.hpp"
*/

using namespace ledger::testing::algorand;
using namespace ledger::core::algorand;

//const api::AlgorandNetworkParameters ALGORAND_PARAMS = networks::getAlgorandNetworkParameters("algorand-testnet");

class AlgorandSynchronizationTest : public WalletFixture<WalletFactory> {
};

// TODO A tester:
//- more than 100 txs
//- synchro incrémentale??

TEST_F(AlgorandSynchronizationTest, AccountSimpleSynchronizationTest) {
    registerCurrency(currencies::algorand());

    auto configuration = DynamicObject::newInstance();
    configuration->putString(api::Configuration::KEYCHAIN_DERIVATION_SCHEME,"44'/<coin_type>'/<account>'/<node>'/<address>");
    //configuration->putString(api::TezosConfiguration::TEZOS_XPUB_CURVE, api::TezosConfigurationDefaults::TEZOS_XPUB_CURVE_ED25519);
    configuration->putString(api::Configuration::BLOCKCHAIN_EXPLORER_ENGINE, api::AlgorandBlockchainExplorerEngines::ALGORAND_NODE);
    configuration->putString(api::Configuration::BLOCKCHAIN_EXPLORER_API_ENDPOINT, api::AlgorandConfigurationDefaults::ALGORAND_API_ENDPOINT);

    auto wallet = std::dynamic_pointer_cast<Wallet>(wait(walletStore->createWallet("test-wallet", "algorand", configuration)));

    auto nextIndex = wait(wallet->getNextAccountIndex());
    EXPECT_EQ(nextIndex, 0);

    const api::AccountCreationInfo info(
        nextIndex,
        {"main"},
        {"44'/283'/0'/0'"},
        {{ std::begin(OBELIX_ADDRESS), std::end(OBELIX_ADDRESS) }},
        {hex::toByteArray("")}
    );

    auto account = std::dynamic_pointer_cast<Account>(wait(wallet->newAccountWithInfo(info)));

    auto receiver = make_receiver([=](const std::shared_ptr<api::Event> &event) {
        fmt::print("Received event {}\n", api::to_string(event->getCode()));

        if (event->getCode() == api::EventCode::SYNCHRONIZATION_STARTED) {
            return;
        }
        EXPECT_EQ(event->getCode(), api::EventCode::SYNCHRONIZATION_SUCCEED);

        dispatcher->stop();
    });

    account->synchronize()->subscribe(dispatcher->getMainExecutionContext(), receiver);
    dispatcher->waitUntilStopped();

    auto operations = wait(std::dynamic_pointer_cast<algorand::OperationQuery>(account->queryOperations()->complete())->execute());
    std::cout << ">>> Nb of operations: " << operations.size() << std::endl;
    EXPECT_GT(operations.size(), 0);
}

#if 0
TEST_F(AlgorandSynchronizationTest, AccountSynchronizationTest) {
    registerCurrency(currencies::algorand());

    auto configuration = DynamicObject::newInstance();
    configuration->putString(api::Configuration::KEYCHAIN_DERIVATION_SCHEME,"44'/<coin_type>'/<account>'/<node>'/<address>");
    //configuration->putString(api::TezosConfiguration::TEZOS_XPUB_CURVE, api::TezosConfigurationDefaults::TEZOS_XPUB_CURVE_ED25519);
    configuration->putString(api::Configuration::BLOCKCHAIN_EXPLORER_ENGINE, api::AlgorandBlockchainExplorerEngines::ALGORAND_NODE);
    configuration->putString(api::Configuration::BLOCKCHAIN_EXPLORER_API_ENDPOINT, api::AlgorandConfigurationDefaults::ALGORAND_API_ENDPOINT);

    auto wallet = std::dynamic_pointer_cast<Wallet>(wait(walletStore->createWallet("test-wallet", "algorand", configuration)));

    std::set<std::string> emittedOperations;
    {
        auto nextIndex = wait(wallet->getNextAccountIndex());
        EXPECT_EQ(nextIndex, 0);

        const api::AccountCreationInfo info(
            nextIndex,
            {"main"},
            {"44'/283'/0'/0'"},
            {OBELIX_ADDRESS.begin(), OBELIX_ADDRESS.end()},
            {hex::toByteArray("")}
        );

        auto account = std::dynamic_pointer_cast<Account>(wait(wallet->newAccountWithInfo(info)));

        auto receiver = make_receiver([&](const std::shared_ptr<api::Event> &event) {
            if (event->getCode() == api::EventCode::NEW_OPERATION) {
                auto uid = event->getPayload()->getString(api::Account::EV_NEW_OP_UID).value();
                EXPECT_EQ(emittedOperations.find(uid), emittedOperations.end());
            }
        });

        // FIXME WTF is this doing?
        services->getEventBus()->subscribe(dispatcher->getMainExecutionContext(), receiver);

        receiver.reset();
        receiver = make_receiver([=](const std::shared_ptr<api::Event> &event) {
            fmt::print("Received event {}\n", api::to_string(event->getCode()));
            if (event->getCode() == api::EventCode::SYNCHRONIZATION_STARTED) {
                return;
            }

            EXPECT_EQ(event->getCode(), api::EventCode::SYNCHRONIZATION_SUCCEED);

            auto balance = wait(account->getBalance());
            EXPECT_GT(balance->toLong(), 0L);

            // FIXME Remove originated stuff & retrieve algorand operations instead I guess?...

            auto operations = wait(std::dynamic_pointer_cast<algorand::OperationQuery>(account->queryOperations()->complete())->execute());
            EXPECT_GE(operations.size(), 3);
            std::cout << ">>> Nb of operations: "<< operations.size() << std::endl;

            balance = wait(account->getBalance());
            EXPECT_NE(balance->toLong(), 0L);
            std::cout << ">>> Balance: "<< balance->toString() << std::endl;

            auto fromDate = "2018-01-01T13:38:23Z";
            auto toDate = DateUtils::toJSON(DateUtils::now());
            auto balanceHistory = wait(account->getBalanceHistory(fromDate, toDate, api::TimePeriod::MONTH));
            EXPECT_EQ(balanceHistory[balanceHistory.size() - 1]->toLong(), balance->toLong());

            dispatcher->stop();
        });

        auto restoreKey = account->getRestoreKey();
        EXPECT_EQ(restoreKey, hex::toString(info.publicKeys[0]));

        // Perform sychronization
        account->synchronize()->subscribe(dispatcher->getMainExecutionContext(), receiver);

        dispatcher->waitUntilStopped();

        // FIXME WTF ???
        // Re-launch a synchronization if it’s the first time
        std::cout << "Running a second synchronization." << std::endl;
        dispatcher = std::make_shared<QtThreadDispatcher>();
        account->synchronize()->subscribe(dispatcher->getMainExecutionContext(), receiver);

        dispatcher->waitUntilStopped();

        auto operations = wait(std::dynamic_pointer_cast<algorand::OperationQuery>(account->queryOperations()->complete())->execute());
        std::cout << ">>> Nb of operations: " << operations.size() << std::endl;
        EXPECT_GT(operations.size(), 0);

        //EXPECT_EQ(operations[0]->getTransaction()->getStatus(), 1);
        //auto fees = wait(account->getFeeEstimate()); // FIXME Create & use a no-callback version for getFeeEstimate()
        //EXPECT_GT(fees->toUint64(), 0);

        //auto storage = wait(account->getStorage("tz1ZshTmtorFVkcZ7CpceCAxCn7HBJqTfmpk"));
        //EXPECT_GT(storage->toUint64(), 0);

        // TODO
        //auto gasLimit = wait(account->getEstimatedGasLimit("tz1ZshTmtorFVkcZ7CpceCAxCn7HBJqTfmpk"));
        //EXPECT_GT(gasLimit->toUint64(), 0);

        //test(nextWalletName, "", explorerURL);
    }
}
#endif
