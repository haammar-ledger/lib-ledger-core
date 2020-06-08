/*
 * AlgorandAccountSynchronizer
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


#ifndef LEDGER_CORE_ALGORANDACCOUNTSYNCHRONIZER_H
#define LEDGER_CORE_ALGORANDACCOUNTSYNCHRONIZER_H

#include <algorand/AlgorandAddress.hpp>
#include <algorand/AlgorandBlockchainExplorer.hpp>

#include <core/Services.hpp>
#include <core/wallet/AbstractWallet.hpp>
#include <core/events/ProgressNotifier.hpp>

#include <string>
#include <map>
#include <vector>

namespace ledger {
namespace core {
namespace algorand {

        struct BatchSavedState {
            //std::string blockHash;
            uint32_t blockHeight;

            BatchSavedState() : blockHeight(0) {}

            template<class Archive>
            void serialize(Archive & archive) {
                archive(/*blockHash, */blockHeight); // serialize things by passing them to the archive
            };
        };

        struct SavedState {
            uint32_t halfBatchSize;
            std::vector<BatchSavedState> batches;
            std::map<std::string, std::string> pendingTxsHash;

            SavedState(): halfBatchSize(0) {}

            template<class Archive>
            void serialize(Archive & archive) {
                archive(halfBatchSize, batches, pendingTxsHash); // serialize things by passing them to the archive
            }
        };

    class Account;

        class AccountSynchronizer : public DedicatedContext,
                                    public std::enable_shared_from_this<AccountSynchronizer> {

        public:

            AccountSynchronizer(const std::shared_ptr<Services> &services,
                                const std::shared_ptr<BlockchainExplorer> &explorer);

            std::shared_ptr<ProgressNotifier<Unit>> synchronizeAccount(const std::shared_ptr<Account>& account);

        private:

            struct SynchronizationBuddy {
                std::shared_ptr<Preferences> preferences;
                std::shared_ptr<spdlog::logger> logger;
                std::chrono::system_clock::time_point startDate;
                std::shared_ptr<AbstractWallet> wallet;
                std::shared_ptr<DynamicObject> configuration;
                uint32_t halfBatchSize;
                //std::shared_ptr<Keychain> keychain;
                Option<SavedState> savedState;
                Option<void *> token;
                std::shared_ptr<Account> account;
                std::map<std::string, std::string> transactionsToDrop;
            };


            Future<Unit> performSynchronization(const std::shared_ptr<Account> &account);

            // Synchronize batches.
            //
            // This function will synchronize all batches by iterating over batches and transactions
            // bulks. The input buddy can be used to customize the behavior of the synchronization.
            Future<Unit> synchronizeBatches(std::shared_ptr<SynchronizationBuddy> buddy,
                                            const uint32_t currentBatchIndex);

            // Synchronize a transactions batch.
            //
            // The currentBatchIndex is the currently synchronized batch. buddy is the
            // synchronization object used to accumulate a state. hadTransactions is used to check
            // whether more data is needed. If a block doesn’t have any transaction, it means that
            // we must stop.
            Future<bool> synchronizeBatch(std::shared_ptr<SynchronizationBuddy> buddy,
                                          const uint32_t currentBatchIndex,
                                          const bool hadTransactions = false);

            void updateCurrentBlock(std::shared_ptr<SynchronizationBuddy> &buddy,
                                    const std::shared_ptr<api::ExecutionContext> &context);

            void updateOperationsToDrop(soci::session &sql,
                                        std::shared_ptr<SynchronizationBuddy> &buddy,
                                        const std::string &accountUid);

            std::shared_ptr<Account> _account;
            std::shared_ptr<BlockchainExplorer> _explorer;
            std::shared_ptr<Preferences> _internalPreferences;
            std::shared_ptr<ProgressNotifier<Unit>> _notifier;
            std::mutex _lock;

        };

} // namespace algorand
} // namespace core
} // namespace ledger

#endif // LEDGER_CORE_ALGORANDACCOUNTSYNCHRONIZER_H
