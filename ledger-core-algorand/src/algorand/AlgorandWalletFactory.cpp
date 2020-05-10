/*
 *
 * AlgorandWalletFactory
 *
 * Created by Hakim Aammar on 11/05/2020.
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

#include <algorand/AlgorandWalletFactory.hpp>
#include <algorand/AlgorandWallet.hpp>
#include <algorand/AlgorandNetworks.hpp>
#include <algorand/database/Migrations.hpp>

#include <algorand/api/AlgorandConfigurationDefaults.hpp>
#include <algorand/api/AlgorandBlockchainExplorerEngines.hpp>
#include <algorand/api/AlgorandBlockchainObserverEngines.hpp>

#include <core/Services.hpp>
#include <core/api/Configuration.hpp>
#include <core/api/SynchronizationEngines.hpp>

#define STRING(key, def) entry.configuration->getString(key).value_or(def)

namespace ledger {
namespace core {
namespace algorand {

    WalletFactory::WalletFactory(const api::Currency &currency, const std::shared_ptr<Services> &services) :
            AbstractWalletFactory(currency, services) {
        // create the DB structure if not already created
        services->getDatabaseSessionPool()->forwardMigration<AlgorandMigration>();
    }

    std::shared_ptr<AbstractWallet> WalletFactory::build(const WalletDatabaseEntry &entry) {
        auto services = getServices();
        services->logger()->info("Building wallet instance '{}' for {} with parameters: {}",
            entry.name,
            entry.currencyName,
            entry.configuration->dump());

        // Get currency
        try {
            auto currency = networks::getAlgorandNetworkParameters(entry.currencyName);
        } catch (Exception) {
            throw make_exception(api::ErrorCode::UNSUPPORTED_CURRENCY, "Unsupported currency '{}'.", entry.currencyName);
        }

        // Configure explorer
        auto explorer = getExplorer(entry.currencyName, entry.configuration);
        if (explorer == nullptr) {
            throw make_exception(api::ErrorCode::UNKNOWN_BLOCKCHAIN_EXPLORER_ENGINE,
                                 "Engine '{}' is not a supported explorer engine.",
                                 STRING(api::Configuration::BLOCKCHAIN_EXPLORER_ENGINE, "undefined"));
        }

        // Configure observer
        auto observer = getObserver(entry.currencyName, entry.configuration);
        if (observer == nullptr) {
            services->logger()->warn(
                    "Observer engine '{}' is not supported. Wallet {} was created anyway. Real time events won't be handled by this instance.",
                    STRING(api::Configuration::BLOCKCHAIN_OBSERVER_ENGINE, "undefined"), entry.name);
        }

        // Configure synchronizer
        Option<AlgorandAccountSynchronizerFactory> synchronizerFactory;
        {
            auto engine = entry.configuration->getString(api::Configuration::SYNCHRONIZATION_ENGINE)
                                            .value_or(api::SynchronizationEngines::BLOCKCHAIN_EXPLORER_SYNCHRONIZATION);
            if (engine == api::SynchronizationEngines::BLOCKCHAIN_EXPLORER_SYNCHRONIZATION) {
                std::weak_ptr<Services> weakServices = services;
                synchronizerFactory = Option<AlgorandAccountSynchronizerFactory>([weakServices, explorer]() {
                    auto services = weakServices.lock();
                    if (!services) {
                        throw make_exception(api::ErrorCode::NULL_POINTER, "Pool was released.");
                    }
                    return std::make_shared<AccountSynchronizer>(services, explorer);
                });
            }
        }

        if (synchronizerFactory.isEmpty()) {
            throw make_exception(api::ErrorCode::UNKNOWN_SYNCHRONIZATION_ENGINE,
                                "Engine '{}' is not a supported synchronization engine.",
                                STRING(api::Configuration::SYNCHRONIZATION_ENGINE, "undefined"));
        }

        // Sets the derivation scheme
        DerivationScheme scheme(STRING(api::Configuration::KEYCHAIN_DERIVATION_SCHEME, "44'/<coin_type>'/<account>'/<node>/<address>"));

        // Build wallet
        return std::make_shared<Wallet>(
            entry.name,
            getCurrency(),
            services,
            entry.configuration,
            scheme,
            explorer,
            observer,
            synchronizerFactory.getValue()
        );
    }

    std::shared_ptr<BlockchainExplorer>
    WalletFactory::getExplorer(const std::string &currencyName, const std::shared_ptr<api::DynamicObject> &configuration) {

        auto it = _runningExplorers.begin();
        while (it != _runningExplorers.end()) {
            auto explorer = it->lock();
            if (explorer != nullptr) {
                if (explorer->match(configuration))
                    return explorer;
                it++;
            } else {
                it = _runningExplorers.erase(it);
            }
        }

        std::shared_ptr<BlockchainExplorer> explorer = nullptr;

        auto engine = configuration->getString(api::Configuration::BLOCKCHAIN_EXPLORER_ENGINE)
                                    .value_or(api::AlgorandBlockchainExplorerEngines::ALGORAND_NODE);

        if (engine == api::AlgorandBlockchainExplorerEngines::ALGORAND_NODE) {
            auto &networkParams = networks::getAlgorandNetworkParameters(getCurrency().name);
            auto services = getServices();
            auto http = services->getHttpClient(fmt::format("{}",
                                                configuration->getString(api::Configuration::BLOCKCHAIN_EXPLORER_API_ENDPOINT)
                                                               .value_or(api::AlgorandConfigurationDefaults::ALGORAND_API_ENDPOINT)));
            auto context = services->getDispatcher()->getSerialExecutionContext(
                    fmt::format("{}-{}-explorer",
                                api::AlgorandBlockchainExplorerEngines::ALGORAND_NODE,
                                networkParams.genesisHash)
            );

            explorer = std::make_shared<BlockchainExplorer>(context,
                                                                    http,
                                                                    networkParams,
                                                                    configuration);
        }

        if (explorer) {
            _runningExplorers.push_back(explorer);
        }

        return explorer;
    }

    std::shared_ptr<BlockchainObserver>
    WalletFactory::getObserver(const std::string &currencyName, const std::shared_ptr<api::DynamicObject> &configuration) {

        auto it = _runningObservers.begin();
        while (it != _runningObservers.end()) {
            auto observer = it->lock();
            if (observer != nullptr) {
                if (observer->match(configuration)) {
                    return observer;
                }
                it++;
            } else {
                it = _runningObservers.erase(it);
            }
        }

        std::shared_ptr<BlockchainObserver> observer;

        auto engine = configuration->getString(api::Configuration::BLOCKCHAIN_OBSERVER_ENGINE)
                                     .value_or(api::AlgorandBlockchainObserverEngines::ALGORAND_NODE);

        if (engine == api::AlgorandBlockchainObserverEngines::ALGORAND_NODE) {
            const auto &currency = getCurrency();
            auto &networkParams = networks::getAlgorandNetworkParameters(currency.name);
            auto services = getServices();
            auto logger = services->logger();
            auto webSocketClient = services->getWebSocketClient();
            auto context = services->getDispatcher()->getSerialExecutionContext(
                    fmt::format("{}-{}-explorer",
                                api::AlgorandBlockchainObserverEngines::ALGORAND_NODE,
                                networkParams.genesisHash)
            );

            observer = std::make_shared<BlockchainObserver>(context,
                                                                    webSocketClient,
                                                                    configuration,
                                                                    logger,
                                                                    currency);
        }

        if (observer) {
            _runningObservers.push_back(observer);
        }

        return observer;
    }
}
}
}
