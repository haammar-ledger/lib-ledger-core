/*
 *
 * BitcoinLikeWallet
 * ledger-core
 *
 * Created by Pierre Pollastri on 19/12/2016.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Ledger
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
#ifndef LEDGER_CORE_BITCOINLIKEWALLET_HPP
#define LEDGER_CORE_BITCOINLIKEWALLET_HPP

#include "../../api/BitcoinLikeWallet.hpp"
#include <memory>
#include "explorers/BitcoinLikeBlockchainExplorer.hpp"
#include "keychains/BitcoinLikeKeychain.hpp"
#include "observers/BitcoinLikeBlockchainObserver.hpp"
#include "synchronizers/BitcoinLikeAccountSynchronizer.hpp"
#include <src/wallet/common/AbstractWallet.hpp>
#include <src/api/BitcoinLikeNetworkParameters.hpp>
#include <wallet/bitcoin/factories/BitcoinLikeWalletFactory.hpp>
#include <wallet/bitcoin/database/BitcoinLikeWalletDatabase.h>
namespace ledger {
    namespace core {
        class BitcoinLikeWallet : public virtual api::BitcoinLikeWallet, public virtual AbstractWallet {
        public:
            BitcoinLikeWallet(
                const std::string& name,
                const std::shared_ptr<BitcoinLikeBlockchainObserver>& observer,
                const BitcoinLikeKeychainFactory& keychainFactory,
                const BitcoinLikeAccountSynchronizerFactory& synchronizerFactory,
                const std::shared_ptr<WalletPool>& pool,
                const api::Currency& network
            );

            void createNewAccount(int32_t index,
                                  const std::shared_ptr<api::BitcoinLikeExtendedPublicKeyProvider> &xpubProvider,
                                  const std::shared_ptr<api::AccountCallback> &callback) override;

            void createNextAccount(const std::shared_ptr<api::BitcoinLikeExtendedPublicKeyProvider> &xpubProvider,
                                   const std::shared_ptr<api::AccountCallback> &callback) override;

            // API methods

            void getAccount(int32_t index, const std::shared_ptr<api::AccountCallback> &callback) override;
            void getAccountCount(const std::shared_ptr<api::I32Callback> &callback) override;
            void getAccounts(int32_t offset, int32_t count,
                             const std::shared_ptr<api::AccountListCallback> &callback) override;
            bool isSynchronizing() override;
            std::shared_ptr<api::EventBus> synchronize() override;


        private:
            std::shared_ptr<BitcoinLikeBlockchainExplorer> _explorer;
            std::shared_ptr<BitcoinLikeBlockchainObserver> _observer;
            BitcoinLikeKeychainFactory _keychainFactory;
            BitcoinLikeAccountSynchronizerFactory _synchronizerFactory;
            api::BitcoinLikeNetworkParameters _network;
        };
    }
}


#endif //LEDGER_CORE_BITCOINLIKEWALLET_HPP
