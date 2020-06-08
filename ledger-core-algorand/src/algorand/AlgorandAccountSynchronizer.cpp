/*
 *
 * AlgorandAccountSynchronizer
 *
 * Created by Hakim Aammar on 01/06/2020
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

#include <algorand/AlgorandAccountSynchronizer.hpp>
#include <algorand/AlgorandAccount.hpp>
#include <algorand/database/AlgorandBlockDatabaseHelper.hpp>

#include <core/wallet/AccountDatabaseHelper.hpp>
#include <core/utils/DateUtils.hpp>
#include <core/utils/DurationUtils.hpp>
#include <core/collections/Vector.hpp>
#include <core/debug/Benchmarker.hpp>
#include <core/api/Configuration.hpp>
#include <core/api/ConfigurationDefaults.hpp>

/*
#include <core/api/Configuration.hpp>
#include <core/api/ConfigurationDefaults.hpp>
#include <core/async/Future.hpp>
#include <core/async/Wait.hpp>
#include <core/collections/DynamicObject.hpp>
#include <core/debug/Benchmarker.hpp>
#include <core/events/ProgressNotifier.hpp>
#include <core/preferences/Preferences.hpp>
#include <core/utils/Unit.hpp>
#include <core/utils/DateUtils.hpp>
#include <core/utils/DurationUtils.hpp>
#include <core/wallet/AbstractWallet.hpp>
#include <core/wallet/AccountDatabaseHelper.hpp>
#include <core/wallet/BlockDatabaseHelper.hpp>

#include <algorithm>
#include <memory>
#include <mutex>
*/

// TODO Review ALL the functions

namespace ledger {
namespace core {
namespace algorand {

    // SIMPLE VERSION

    AccountSynchronizer::AccountSynchronizer(const std::shared_ptr<Services> & services,
                                             const std::shared_ptr<BlockchainExplorer> & explorer) :
        DedicatedContext(services->getDispatcher()->getThreadPoolExecutionContext("synchronizers")),
        _explorer(explorer)
    {
        //_explorer = explorer;
    }

    std::shared_ptr<ProgressNotifier<Unit>> AccountSynchronizer::synchronizeAccount(const std::shared_ptr<Account>& account) {
        std::lock_guard<std::mutex> lock(_lock);

        if (!_account) {
            _account = account;
            _notifier = std::make_shared<ProgressNotifier<Unit>>();
            auto self = shared_from_this();

            performSynchronization(account)
                .onComplete(getContext(), [self] (const Try<Unit> &result) {
                    std::lock_guard<std::mutex> l(self->_lock);
                    if (result.isFailure()) {
                        self->_notifier->failure(result.getFailure());
                    } else {
                        self->_notifier->success(unit);
                    }
                    self->_notifier = nullptr;
                    self->_account = nullptr;
                });

        } else if (account != _account) {
            throw make_exception(api::ErrorCode::RUNTIME_ERROR, "This synchronizer is already in use");
        }
        return _notifier;
    };

    Future<Unit> AccountSynchronizer::performSynchronization(const std::shared_ptr<Account> & account) {

        // FIXME call _explorer->getAccount() first?

        // FIXME Benchmark?
        //auto benchmark = std::make_shared<Benchmarker>(fmt::format("Synchronize batch {}", currentBatchIndex), buddy->logger);
        //benchmark->start();

        return synchronizeBatch(account, Option<uint64_t>(), false)
            .template flatMap<Unit>(account->getContext(), [&] (const bool hadTransactions) -> Future<Unit> {
                // FIXME Benchmark?
                //benchmark->stop();

                //buddy->preferences->editor()->template putObject<SavedState>("state", buddy->savedState.getValue())->commit();

                return Future<Unit>::successful(unit);

            }).recoverWith(ImmediateExecutionContext::INSTANCE, [&] (const Exception & exception) -> Future<Unit> {

                // FIXME logger?
                //buddy->logger->info("Recovering from failing synchronization : {}", exception.getMessage());

                return Future<Unit>::failure(exception);

                // FIXME Do we need a recovery mechanism?
                /*
                //A block reorganization happened
                if (exception.getErrorCode() == api::ErrorCode::BLOCK_NOT_FOUND && buddy->savedState.nonEmpty()) {
                    buddy->logger->info("Recovering from reorganization");

                    //Get its block/block height
                    auto& failedBatch = buddy->savedState.getValue().batches[currentBatchIndex];
                    auto failedBlockHeight = failedBatch.blockHeight;
                    //auto failedBlockHash = failedBatch.blockHash;

                    if (failedBlockHeight > 0) {
                        //Delete data related to failedBlock (and all blocks above it)
                        buddy->logger->info("Deleting blocks above block height: {}", failedBlockHeight);

                        soci::session sql(buddy->wallet->getDatabase()->getPool());
                        sql << "DELETE FROM blocks where height >= :failedBlockHeight", soci::use(failedBlockHeight);

                        //Get last block not part from reorg
                        auto lastBlock = BlockDatabaseHelper::getLastBlock(sql, buddy->wallet->getCurrency().name);

                        //Resync from the "beginning" if no last block in DB
                        int64_t lastBlockHeight = 0;
                        //std::string lastBlockHash;
                        if (lastBlock.nonEmpty()) {
                            lastBlockHeight = lastBlock.getValue().height;
                            //lastBlockHash = lastBlock.getValue().blockHash;
                        }

                        //Update savedState's batches
                        for (auto &batch : buddy->savedState.getValue().batches) {
                            if (batch.blockHeight > lastBlockHeight) {
                                batch.blockHeight = (uint32_t)lastBlockHeight;
                                //batch.blockHash = lastBlockHash;
                            }
                        }

                        //Save new savedState
                        buddy->preferences->editor()->template putObject<SavedState>("state", buddy->savedState.getValue())->commit();

                        //Synchronize same batch now with an existing block (of height lastBlockHeight)
                        //if failedBatch was not the deepest block part of that reorg, this recursive call
                        //will ensure to get (and delete from DB) to the deepest failed block (part of reorg)
                        buddy->logger->info("Relaunch synchronization after recovering from reorganization");

                        return self->synchronizeBatches(buddy, currentBatchIndex);
                    }
                } else {
                    return Future<Unit>::failure(exception);
                }

                return Future<Unit>::successful(unit);
                */
            });
    }

    Future<bool> AccountSynchronizer::synchronizeBatch(const std::shared_ptr<Account> & account,
                                                       const Option<uint64_t> & beforeRound,
                                                       const bool hadTransactions) {
        auto self = shared_from_this();
        return _explorer->getTransactionsForAddress(account->getAddress(), beforeRound)
            .template flatMap<bool>(getContext(), [&, account, self](const model::TransactionsBulk& bulk) -> Future<bool> {

                soci::session sql(account->getWallet()->getDatabase()->getPool());
                soci::transaction tr(sql);

                // FIXME logger?
                //buddy->logger->info("Got {} txs", bulk.transactions.size());
                auto minTxRound = std::numeric_limits<uint64_t>::max();
                for (const auto& tx : bulk.transactions) {

                    // A lot of things could happen here, better wrap it
                    auto tryPutTx = Try<int>::from([&] () {
                        return account->putTransaction(sql, tx);
                    });

                    // Record the lowest round, to continue fetching txs below it if needed
                    minTxRound = std::min(minTxRound, tx.header.round.getValueOr(minTxRound));

                    // FIXME logger?
                    //auto txId = tx.header.id.getValueOr("");
                    //if (tryPutTx.isFailure()) {
                    //    buddy->logger->error("Failed to put transaction {} for account {}, reason: {}", txId, buddy->account->getAccountUid(), tryPutTx.getFailure().getMessage());
                    //}
                }

                tr.commit();

                auto hadTX = hadTransactions || bulk.transactions.size() > 0;
                if (bulk.hasNext) {
                    return self->synchronizeBatch(account, minTxRound, hadTX);
                } else {
                    return Future<bool>::successful(hadTX);
                }
            });
    }









    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~é
#if 0
    static void initializeSavedState(Option<SavedState> &savedState, int32_t halfBatchSize) {

        if (savedState.isEmpty()) {
            savedState = Option<SavedState>(SavedState());
            savedState.getValue().halfBatchSize = (uint32_t) halfBatchSize;

        } else if (savedState.getValue().halfBatchSize != halfBatchSize) {
            BatchSavedState batchSS;
            batchSS.blockHeight = 1U << 31U;

            for (auto &batchState : savedState.getValue().batches) {
                if (batchState.blockHeight < batchSS.blockHeight) {
                    batchSS = batchState;
                }
            }

            auto newBatchCount = (savedState.getValue().batches.size() * savedState.getValue().halfBatchSize) / halfBatchSize;
            if ((savedState.getValue().batches.size() * savedState.getValue().halfBatchSize) / halfBatchSize != 0) {
                newBatchCount += 1;
            }

            savedState.getValue().batches.clear();
            savedState.getValue().halfBatchSize = (uint32_t) halfBatchSize;

            for (auto i = 0; i <= newBatchCount; i++) {
                BatchSavedState s;
                //s.blockHash = batchSS.blockHash;
                s.blockHeight = batchSS.blockHeight;
                savedState.getValue().batches.push_back(s);
            }
        }
    }

    AccountSynchronizer::AccountSynchronizer(const std::shared_ptr<Services> & services,
                                             const std::shared_ptr<BlockchainExplorer> & explorer) :
        DedicatedContext(services->getDispatcher()->getThreadPoolExecutionContext("synchronizers")),
        _explorer(explorer)
    {
        //_explorer = explorer;
    }

    std::shared_ptr<ProgressNotifier<Unit>> AccountSynchronizer::synchronizeAccount(const std::shared_ptr<Account>& account) {
        std::lock_guard<std::mutex> lock(_lock);
        if (!_account) {
            _account = account;
            _notifier = std::make_shared<ProgressNotifier<Unit>>();
            auto self = shared_from_this();

            performSynchronization(account).onComplete(getContext(), [self] (const Try<Unit> &result) {
                std::lock_guard<std::mutex> l(self->_lock);
                if (result.isFailure()) {
                    self->_notifier->failure(result.getFailure());
                } else {
                    self->_notifier->success(unit);
                }
                self->_notifier = nullptr;
                self->_account = nullptr;
            });

        } else if (account != _account) {
            throw make_exception(api::ErrorCode::RUNTIME_ERROR, "This synchronizer is already in use");
        }
        return _notifier;
    };


    Future<Unit> AccountSynchronizer::performSynchronization(const std::shared_ptr<Account> &account) {
        auto buddy = std::make_shared<SynchronizationBuddy>();

        buddy->account = account;
        buddy->preferences = account->getInternalPreferences()->getSubPreferences("AlgorandAccountSynchronizer");
        buddy->logger = account->logger();
        buddy->startDate = DateUtils::now();
        buddy->wallet = account->getWallet();
        buddy->configuration = account->getWallet()->getConfig();
        //buddy->keychain = account->getKeychain();
        buddy->savedState = buddy->preferences->template getObject<SavedState>("state");
        buddy->halfBatchSize = (uint32_t) buddy->configuration
                                               ->getInt(api::Configuration::SYNCHRONIZATION_HALF_BATCH_SIZE)
                                               .value_or(api::ConfigurationDefaults::KEYCHAIN_DEFAULT_OBSERVABLE_RANGE);
        buddy->logger->info("Starting synchronization for account#{} ({}) of wallet {} at {}",
                            account->getIndex(),
                            account->getAddress(),
                            account->getWallet()->getName(),
                            DateUtils::toJSON(buddy->startDate));

        // Check if reorganization happened
        soci::session sql(buddy->wallet->getDatabase()->getPool());
        if (buddy->savedState.nonEmpty()) {

            // Get deepest block saved in batches to be part of reorg
            auto sortedBatches = buddy->savedState.getValue().batches;
            std::sort(sortedBatches.begin(), sortedBatches.end(),
                [](const BatchSavedState &lhs, const BatchSavedState &rhs) -> bool {
                    return lhs.blockHeight < rhs.blockHeight;
                });

            auto currencyName = buddy->wallet->getCurrency().name;
            size_t index = 0;
            // Reorg can't happen until genesis block, safely initialize with 0
            uint64_t deepestFailedBlockHeight = 0;
            while (index < sortedBatches.size() && !BlockDatabaseHelper::blockExists(sql, sortedBatches[index].blockHeight, currencyName)) {
                deepestFailedBlockHeight = sortedBatches[index].blockHeight;
                index ++;
            }

            // Case of reorg, update savedState's batches
            if (deepestFailedBlockHeight > 0) {
                // Get last block (in DB) which contains current account's operations
                auto previousBlock = AccountDatabaseHelper::getLastBlockWithOperations(sql, buddy->account->getAccountUid());
                for (auto& batch : buddy->savedState.getValue().batches) {
                    if (batch.blockHeight >= deepestFailedBlockHeight) {
                        batch.blockHeight = previousBlock.nonEmpty() ? (uint32_t)previousBlock.getValue().height : 0;
                        //batch.blockHash = previousBlock.nonEmpty() ? previousBlock.getValue().blockHash : "";
                    }
                }
            }
        }

        initializeSavedState(buddy->savedState, buddy->halfBatchSize);

        updateOperationsToDrop(sql, buddy, account->getAccountUid());

        // FIXME Needed??
        //updateCurrentBlock(buddy, account->getContext());

        auto self = shared_from_this();
        return _explorer->startSession()
            .template map<Unit>(account->getContext(), [buddy] (void * const& t) -> Unit {

                buddy->logger->info("Synchronization token obtained");
                buddy->token = Option<void *>(t);
                return unit;

            }).template flatMap<Unit>(account->getContext(), [buddy, self] (const Unit&) {

                // This is where it's done
                return self->synchronizeBatches(buddy, 0);

            }).template flatMap<Unit>(account->getContext(), [self, buddy] (const Unit&) {

                auto tryKillSession = Try<Future<Unit>>::from([=](){
                    return self->_explorer->killSession(buddy->token.getValue());
                });

                if (tryKillSession.isFailure()) {
                    buddy->logger->warn("Failed to delete synchronization token {} for account#{} of wallet {}",
                                        static_cast<char *>(buddy->token.getValue()), buddy->account->getIndex(),
                                        buddy->account->getWallet()->getName());
                    // We return a successful Unit because deleting the sync token should not be a "failure"
                    // Note: in a near future we'll try to get rid of sync token mechanism
                    return Future<Unit>::successful(unit);
                }

                return tryKillSession.getValue();

            }).template map<Unit>(ImmediateExecutionContext::INSTANCE, [self, buddy] (const Unit&) {

                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        (DateUtils::now() - buddy->startDate.time_since_epoch()).time_since_epoch());

                buddy->logger->info("End synchronization for account#{} of wallet {} in {}", buddy->account->getIndex(),
                                    buddy->account->getWallet()->getName(), DurationUtils::formatDuration(duration));

                //Delete dropped txs from DB
                soci::session sql(buddy->wallet->getDatabase()->getPool());
                for (auto& tx : buddy->transactionsToDrop) {
                    //Check if tx is pending
                    auto it = buddy->savedState.getValue().pendingTxsHash.find(tx.first);
                    if (it == buddy->savedState.getValue().pendingTxsHash.end()) {
                        buddy->logger->info("Drop transaction {}", tx.first);
                        buddy->logger->info("Deleting operation from DB {}", tx.second);
                        //delete tx.second from DB (from operations)
                        sql << "DELETE FROM operations WHERE uid = :uid", soci::use(tx.second);
                    }
                }

                self->_account = nullptr;
                return unit;

            }).recover(ImmediateExecutionContext::INSTANCE, [buddy] (const Exception& ex) -> Unit {

                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        (DateUtils::now() - buddy->startDate.time_since_epoch()).time_since_epoch());
                buddy->logger->error("Error during during synchronization for account#{} of wallet {} in {} ms", buddy->account->getIndex(),
                                        buddy->account->getWallet()->getName(), duration.count());
                buddy->logger->error("Due to {}, {}", api::to_string(ex.getErrorCode()), ex.getMessage());
                return unit;
            });
    };

    // Synchronize batches.
    // This function will synchronize all batches by iterating over batches and transactions
    // bulks. The input buddy can be used to customize the behavior of the synchronization.
    Future<Unit> AccountSynchronizer::synchronizeBatches(std::shared_ptr<SynchronizationBuddy> buddy,
                                                         const uint32_t currentBatchIndex) {
        buddy->logger->info("SYNC BATCHES");

        auto hasMultipleAddresses = buddy->wallet->hasMultipleAddresses();
        auto done = currentBatchIndex >= buddy->savedState.getValue().batches.size() - 1;
        if (currentBatchIndex >= buddy->savedState.getValue().batches.size()) {
            buddy->savedState.getValue().batches.push_back(BatchSavedState());
        }

        auto self = shared_from_this();
        auto& batchState = buddy->savedState.getValue().batches[currentBatchIndex];

        auto benchmark = std::make_shared<Benchmarker>(fmt::format("Synchronize batch {}", currentBatchIndex), buddy->logger);
        benchmark->start();

        // FIXME Capture by ref instead of by copy???
        return synchronizeBatch(buddy, currentBatchIndex)
            .template flatMap<Unit>(buddy->account->getContext(), [=] (const bool& hadTransactions) -> Future<Unit> {
                benchmark->stop();

                buddy->preferences->editor()->template putObject<SavedState>("state", buddy->savedState.getValue())->commit();

                //Sync stops if there are no more batches in savedState and last batch has no transactions
                //But we may want to force sync of accounts within KEYCHAIN_OBSERVABLE_RANGE
                auto discoveredAddresses = currentBatchIndex * buddy->halfBatchSize;
                auto lastDiscoverableAddress = buddy->configuration->getInt(api::Configuration::KEYCHAIN_OBSERVABLE_RANGE).value_or(buddy->halfBatchSize);
                if (hasMultipleAddresses && (!done || (done && hadTransactions) || lastDiscoverableAddress > discoveredAddresses)) {
                    return self->synchronizeBatches(buddy, currentBatchIndex + 1);
                }

                return Future<Unit>::successful(unit);

            }).recoverWith(ImmediateExecutionContext::INSTANCE, [=] (const Exception &exception) -> Future<Unit> {
                buddy->logger->info("Recovering from failing synchronization : {}", exception.getMessage());

                //A block reorganization happened
                if (exception.getErrorCode() == api::ErrorCode::BLOCK_NOT_FOUND && buddy->savedState.nonEmpty()) {
                    buddy->logger->info("Recovering from reorganization");

                    //Get its block/block height
                    auto& failedBatch = buddy->savedState.getValue().batches[currentBatchIndex];
                    auto failedBlockHeight = failedBatch.blockHeight;
                    //auto failedBlockHash = failedBatch.blockHash;

                    if (failedBlockHeight > 0) {
                        //Delete data related to failedBlock (and all blocks above it)
                        buddy->logger->info("Deleting blocks above block height: {}", failedBlockHeight);

                        soci::session sql(buddy->wallet->getDatabase()->getPool());
                        sql << "DELETE FROM blocks where height >= :failedBlockHeight", soci::use(failedBlockHeight);

                        //Get last block not part from reorg
                        auto lastBlock = BlockDatabaseHelper::getLastBlock(sql, buddy->wallet->getCurrency().name);

                        //Resync from the "beginning" if no last block in DB
                        int64_t lastBlockHeight = 0;
                        //std::string lastBlockHash;
                        if (lastBlock.nonEmpty()) {
                            lastBlockHeight = lastBlock.getValue().height;
                            //lastBlockHash = lastBlock.getValue().blockHash;
                        }

                        //Update savedState's batches
                        for (auto &batch : buddy->savedState.getValue().batches) {
                            if (batch.blockHeight > lastBlockHeight) {
                                batch.blockHeight = (uint32_t)lastBlockHeight;
                                //batch.blockHash = lastBlockHash;
                            }
                        }

                        //Save new savedState
                        buddy->preferences->editor()->template putObject<SavedState>("state", buddy->savedState.getValue())->commit();

                        //Synchronize same batch now with an existing block (of height lastBlockHeight)
                        //if failedBatch was not the deepest block part of that reorg, this recursive call
                        //will ensure to get (and delete from DB) to the deepest failed block (part of reorg)
                        buddy->logger->info("Relaunch synchronization after recovering from reorganization");

                        return self->synchronizeBatches(buddy, currentBatchIndex);
                    }
                } else {
                    return Future<Unit>::failure(exception);
                }

                return Future<Unit>::successful(unit);
            });
    };

    // Synchronize a transactions batch.
    //
    // The currentBatchIndex is the currently synchronized batch. buddy is the
    // synchronization object used to accumulate a state. hadTransactions is used to check
    // whether more data is needed. If a block doesn’t have any transaction, it means that
    // we must stop.
    Future<bool> AccountSynchronizer::synchronizeBatch(std::shared_ptr<SynchronizationBuddy> buddy,
                                                        const uint32_t currentBatchIndex,
                                                        const bool hadTransactions) {
        buddy->logger->info("SYNC BATCH {}", currentBatchIndex);

        auto self = shared_from_this();
        const auto& batchState = buddy->savedState.getValue().batches[currentBatchIndex];

        auto derivationBenchmark = std::make_shared<Benchmarker>("Batch derivation", buddy->logger);
        derivationBenchmark->start();

        derivationBenchmark->stop();

        auto benchmark = std::make_shared<Benchmarker>("Get batch", buddy->logger);
        benchmark->start();
        return _explorer
            ->getTransactionsForAddress(buddy->account->getAddress(), batchState.blockHeight)
            // FIXME Capture by ref instead of by copy???
            .template flatMap<bool>(buddy->account->getContext(), [self, currentBatchIndex, buddy, hadTransactions, benchmark] (const model::TransactionsBulk& bulk) -> Future<bool> {
                benchmark->stop();

                auto insertionBenchmark = std::make_shared<Benchmarker>("Transaction computation", buddy->logger);
                insertionBenchmark->start();

                auto& batchState = buddy->savedState.getValue().batches[currentBatchIndex];
                soci::session sql(buddy->wallet->getDatabase()->getPool());
                soci::transaction tr(sql);

                buddy->logger->info("Got {} txs", bulk.transactions.size());
                for (const auto& tx : bulk.transactions) {

                    // A lot of things could happen here, better to wrap it
                    auto tryPutTx = Try<int>::from([&] () {
                        return buddy->account->putTransaction(sql, tx);
                    });

                    auto txId = tx.header.id.getValueOr("");
                    if (tryPutTx.isFailure()) {
                        buddy->logger->error("Failed to put transaction {} for account {}, reason: {}", txId, buddy->account->getAccountUid(), tryPutTx.getFailure().getMessage());
                    }

                    // Update first pendingTxHash in savedState
                    auto it = buddy->transactionsToDrop.find(txId);
                    if (it != buddy->transactionsToDrop.end()) {
                        // If tx round is set, it is no longer pending
                        if (tx.header.round.nonEmpty() && *tx.header.round > 0) {
                            buddy->savedState.getValue().pendingTxsHash.erase(it->first);
                        } else { // Otherwise tx is in mempool but pending
                            buddy->savedState.getValue().pendingTxsHash.insert(std::pair<std::string, std::string>(it->first, it->second));
                        }
                    }

                    // Remove from tx to drop
                    buddy->transactionsToDrop.erase(txId);
                }

                tr.commit();
                buddy->account->emitEventsNow();

                // Get the last block
                if (bulk.transactions.size() > 0) {
                    auto &lastBlockHeight = bulk.transactions.back().header.round;

                    if (lastBlockHeight.nonEmpty()) {
                        batchState.blockHeight = lastBlockHeight;
                        //batchState.blockHash = lastBlock.getValue().blockHash;
                    }
                }

                insertionBenchmark->stop();

                auto hadTX = hadTransactions || bulk.transactions.size() > 0;
                if (bulk.hasNext) {
                    return self->synchronizeBatch(buddy, currentBatchIndex, hadTX);
                } else {
                    return Future<bool>::successful(hadTX);
                }
            });
    };

    void AccountSynchronizer::updateCurrentBlock(std::shared_ptr<SynchronizationBuddy> &buddy,
                                                 const std::shared_ptr<api::ExecutionContext> &context) {
        // TODO or not?
    }

    void AccountSynchronizer::updateOperationsToDrop(soci::session &sql,
                                                     std::shared_ptr<SynchronizationBuddy> &buddy,
                                                     const std::string &accountUid) {
        //Get all operations in DB that may be dropped (txs without block_uid)
        soci::rowset<soci::row> rows = (sql.prepare
                << "SELECT op.uid, alg_op.transaction_hash FROM operations AS op "
                   "LEFT OUTER JOIN algorand_operations AS alg_op ON alg_op.uid = op.uid "
                   "WHERE op.block_uid IS NULL AND op.account_uid = :uid ", soci::use(accountUid));

        for (auto &row : rows) {
            if (row.get_indicator(0) != soci::i_null && row.get_indicator(1) != soci::i_null) {
                buddy->transactionsToDrop.insert(
                        std::pair<std::string, std::string>(row.get<std::string>(1), row.get<std::string>(0)));
            }
        }
    }
#endif
/*
    std::shared_ptr<AccountSynchronizer> AccountSynchronizer::getSharedFromThis() {
        return shared_from_this();
    }

    std::shared_ptr<api::ExecutionContext> AccountSynchronizer::getSynchronizerContext() {
        return getContext();
    }
*/
} // namespace algorand
} // namespace core
} // namespace ledger
