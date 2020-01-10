/*
 *
 * CosmosLikeAccount
 *
 * Created by El Khalil Bellakrid on 14/06/2019.
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


#include <cosmos/CosmosLikeAccount.hpp>

#include <soci.h>

#include <cosmos/api/CosmosLikeAddress.hpp>
#include <cosmos/CosmosLikeWallet.hpp>
#include <cosmos/api_impl/CosmosLikeTransactionApi.hpp>
#include <cosmos/database/CosmosLikeAccountDatabaseHelper.hpp>
#include <cosmos/database/CosmosLikeTransactionDatabaseHelper.hpp>
#include <cosmos/explorers/CosmosLikeBlockchainExplorer.hpp>
#include <cosmos/transaction_builders/CosmosLikeTransactionBuilder.hpp>

#include <core/database/SociNumber.hpp>
#include <core/database/SociDate.hpp>
#include <core/database/SociOption.hpp>
#include <core/database/query/ConditionQueryFilter.h>
#include <core/async/Future.hpp>
#include <core/events/Event.hpp>
#include <core/math/Base58.hpp>
#include <core/utils/Option.hpp>
#include <core/utils/DateUtils.hpp>
#include <core/collections/Vector.hpp>
#include <core/wallet/common/database/OperationDatabaseHelper.hpp>
#include <core/wallet/common/database/BlockDatabaseHelper.hpp>
#include <core/wallet/common/synchronizers/AbstractBlockchainExplorerAccountSynchronizer.hpp>
#include <core/wallet/pool/database/CurrenciesDatabaseHelper.hpp>

using namespace soci;

namespace ledger {
    namespace core {

        CosmosLikeAccount::CosmosLikeAccount(const std::shared_ptr<AbstractWallet> &wallet,
                                           int32_t index,
                                           const std::shared_ptr<CosmosLikeBlockchainExplorer> &explorer,
                                           const std::shared_ptr<CosmosLikeBlockchainObserver> &observer,
                                           const std::shared_ptr<CosmosLikeAccountSynchronizer> &synchronizer,
                                           const std::shared_ptr<CosmosLikeKeychain> &keychain) : AbstractAccount(wallet, index) {
            _explorer = explorer;
            _observer = observer;
            _synchronizer = synchronizer;
            _keychain = keychain;
            _accountAddress = keychain->getAddress()->toString();
        }

        std::shared_ptr<api::CosmosLikeAccount> CosmosLikeAccount::asCosmosLikeAccount() {
            return std::dynamic_pointer_cast<CosmosLikeAccount>(shared_from_this());
        }

        void CosmosLikeAccount::inflateOperation(Operation &out,
                                                const std::shared_ptr<const AbstractWallet> &wallet,
                                                const cosmos::Transaction &tx) {
            // TODO COSMOS Implement inflateOperation
            out.accountUid = getAccountUid();
            out.block = tx.block;
            out.cosmosTransaction = Option<cosmos::Transaction>(tx);
            out.currencyName = getWallet()->getCurrency().name;
            out.walletType = getWalletType();
            out.walletUid = wallet->getWalletUid();
            out.date = tx.timestamp;
            if (out.block.nonEmpty())
                out.block.getValue().currencyName = wallet->getCurrency().name;
            out.cosmosTransaction.getValue().block = out.block;
            out.trust = std::make_shared<TrustIndicator>();
        }

        int CosmosLikeAccount::putTransaction(soci::session &sql,
                                             const cosmos::Transaction &tx) {
            auto wallet = getWallet();
            if (wallet == nullptr) {
                throw Exception(api::ErrorCode::RUNTIME_ERROR, "Wallet reference is dead.");
            }

             if (tx.block.nonEmpty()) {
                 putBlock(sql, tx.block.getValue());
             }

            int result = FLAG_TRANSACTION_IGNORED;
            auto address = getKeychain()->getAddress()->toBech32();
             CosmosLikeTransactionDatabaseHelper::putTransaction(sql, getAccountUid(), tx);

//             Operation operation;
//             inflateOperation(operation, getWallet(), tx);
//
//             for (const auto& msg : tx.messages) {
//                 operation.senders = {msg.sender};
//                 operation.recipients = {msg.recipient};
//                 operation.amount = msg.amount;
//                 operation.fees = Option<BigInt>(msg.fees);
//
//                 if (msg.sender == address) {
//                     // Do the send operation
//                     operation.type = api::OperationType::SEND;
//                     operation.refreshUid();
//                     if (OperationDatabaseHelper::putOperation(sql, operation)) {
//                         emitNewOperationEvent(operation);
//                     }
//                     result = static_cast<int>(operation.type);
//                 }
//
//                 if (msg.recipient == address) {
//                     // Do the receive operation
//                     operation.type = api::OperationType::RECEIVE;
//                     operation.refreshUid();
//                     if (OperationDatabaseHelper::putOperation(sql, operation)) {
//                         emitNewOperationEvent(operation);
//                     }
//                     result = static_cast<int>(operation.type);
//                 }
//
//             }

            // Operation operation;
            // inflateOperation(operation, wallet, transaction);
            // std::vector<std::string> senders{transaction.sender};
            // operation.senders = std::move(senders);
            // std::vector<std::string> receivers{transaction.receiver};
            // operation.recipients = std::move(receivers);
            // operation.fees = transaction.gasLimit * transaction.gasPrice;
            // operation.trust = std::make_shared<TrustIndicator>();
            // operation.date = transaction.receivedAt;

            // if (_accountAddress == transaction.sender) {
            //     operation.amount = transaction.value;
            //     operation.type = api::OperationType::SEND;
            //     operation.refreshUid();
            //     if (OperationDatabaseHelper::putOperation(sql, operation)) {
            //         emitNewOperationEvent(operation);
            //     }
            //     result = static_cast<int>(operation.type);
            // }

            // if (_accountAddress == transaction.receiver) {
            //     operation.amount = transaction.value;
            //     operation.type = api::OperationType::RECEIVE;
            //     operation.refreshUid();
            //     if (OperationDatabaseHelper::putOperation(sql, operation)) {
            //         emitNewOperationEvent(operation);
            //     }
            //     result = static_cast<int>(operation.type);
            // }

            // return result;
            // TODO COSMOS Implement putTransaction
            return result;
        }

        bool CosmosLikeAccount::putBlock(soci::session &sql,
                                         const CosmosLikeBlockchainExplorer::Block &block) {
            Block abstractBlock;
            abstractBlock.hash = block.hash;
            abstractBlock.currencyName = getWallet()->getCurrency().name;
            abstractBlock.height = block.height;
            abstractBlock.time = block.time;
            if (BlockDatabaseHelper::putBlock(sql, abstractBlock)) {
                emitNewBlockEvent(abstractBlock);
                return true;
            }
            return false;
        }

        std::shared_ptr<CosmosLikeKeychain> CosmosLikeAccount::getKeychain() const {
            return _keychain;
        }

        FuturePtr<Amount> CosmosLikeAccount::getBalance() {
            auto currency = getWallet()->getCurrency();
//            return _explorer->getAccount(_keychain->getAddress()->toBech32()).mapPtr<Amount>(getContext(), [currency](const cosmos::Account &balance) {
//                //TODO: handle balanceS
//                return std::make_shared<Amount>(currency, 0, balance.balances.size() > 0 ? balance.balances[0] : BigInt::ZERO);
//            });
        }

        std::shared_ptr<api::OperationQuery> CosmosLikeAccount::queryOperations() {
            auto headFilter = api::QueryFilter::accountEq(getAccountUid());
            auto query = std::make_shared<OperationQuery>(
                    headFilter,
                    getWallet()->getDatabase(),
                    getWallet()->getContext(),
                    getWallet()->getMainExecutionContext()
            );
            query->registerAccount(shared_from_this());
            return query;
        }

        void CosmosLikeAccount::getEstimatedGasLimit(const std::shared_ptr<api::CosmosLikeTransaction> &transaction, const std::shared_ptr<api::BigIntCallback> & callback) {
//            _explorer->getEstimatedGasLimit(transaction).mapPtr<api::BigInt>(getContext(), [] (const std::shared_ptr<BigInt> &gasLimit) -> std::shared_ptr<api::BigInt> {
//                return std::make_shared<api::BigIntImpl>(*gasLimit);
//            }).callback(getContext(), callback);
        }

        Future<AbstractAccount::AddressList> CosmosLikeAccount::getFreshPublicAddresses() {
            auto keychain = getKeychain();
            return async<AbstractAccount::AddressList>([=]() -> AbstractAccount::AddressList {
                AbstractAccount::AddressList result{keychain->getAddress()};
                return result;
            });
        }

        Future<std::vector<std::shared_ptr<api::Amount>>>
        CosmosLikeAccount::getBalanceHistory(const std::string &start,
                                            const std::string &end,
                                            api::TimePeriod precision) {
            auto self = std::dynamic_pointer_cast<CosmosLikeAccount>(shared_from_this());
            return async<std::vector<std::shared_ptr<api::Amount>>>([=]() -> std::vector<std::shared_ptr<api::Amount>> {

                auto startDate = DateUtils::fromJSON(start);
                auto endDate = DateUtils::fromJSON(end);
                if (startDate >= endDate) {
                    throw make_exception(api::ErrorCode::INVALID_DATE_FORMAT,
                                         "Start date should be strictly greater than end date");
                }

                const auto &uid = self->getAccountUid();
                soci::session sql(self->getWallet()->getDatabase()->getPool());
                std::vector<Operation> operations;

                auto keychain = self->getKeychain();
                std::function<bool(const std::string &)> filter = [&keychain](const std::string addr) -> bool {
                    return keychain->contains(addr);
                };

                //Get operations related to an account
                OperationDatabaseHelper::queryOperations(sql, uid, operations, filter);

                auto lowerDate = startDate;
                auto upperDate = DateUtils::incrementDate(startDate, precision);

                std::vector<std::shared_ptr<api::Amount>> amounts;
                std::size_t operationsCount = 0;
                BigInt sum;
                while (lowerDate <= endDate && operationsCount < operations.size()) {

                    auto operation = operations[operationsCount];
                    while (operation.date > upperDate && lowerDate < endDate) {
                        lowerDate = DateUtils::incrementDate(lowerDate, precision);
                        upperDate = DateUtils::incrementDate(upperDate, precision);
                        amounts.emplace_back(
                                std::make_shared<ledger::core::Amount>(self->getWallet()->getCurrency(), 0, sum));
                    }

                    if (operation.date <= upperDate) {
                        switch (operation.type) {
                            case api::OperationType::RECEIVE: {
                                sum = sum + operation.amount;
                                break;
                            }
                            case api::OperationType::SEND: {
                                sum = sum - (operation.amount + operation.fees.getValueOr(BigInt::ZERO));
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    operationsCount += 1;
                }

                while (lowerDate < endDate) {
                    lowerDate = DateUtils::incrementDate(lowerDate, precision);
                    amounts.emplace_back(
                            std::make_shared<ledger::core::Amount>(self->getWallet()->getCurrency(), 0, sum));
                }

                return amounts;
            });
        }

        Future<api::ErrorCode> CosmosLikeAccount::eraseDataSince(const std::chrono::system_clock::time_point &date) {
            auto log = logger();
            log->debug(" Start erasing data of account : {}", getAccountUid());
            soci::session sql(getWallet()->getDatabase()->getPool());
            //Update account's internal preferences (for synchronization)
            auto savedState = getInternalPreferences()->getSubPreferences("BlockchainExplorerAccountSynchronizer")->getObject<BlockchainExplorerAccountSynchronizationSavedState>("state");
            if (savedState.nonEmpty()) {
                //Reset batches to blocks mined before given date
                auto previousBlock = BlockDatabaseHelper::getPreviousBlockInDatabase(sql,
                                                                                     getWallet()->getCurrency().name,
                                                                                     date);
                for (auto &batch : savedState.getValue().batches) {
                    if (previousBlock.nonEmpty() && batch.blockHeight > previousBlock.getValue().height) {
                        batch.blockHeight = (uint32_t) previousBlock.getValue().height;
                        batch.blockHash = previousBlock.getValue().blockHash;
                    } else if (!previousBlock.nonEmpty()) {//if no previous block, sync should go back from genesis block
                        batch.blockHeight = 0;
                        batch.blockHash = "";
                    }
                }
                getInternalPreferences()->getSubPreferences("BlockchainExplorerAccountSynchronizer")->editor()->putObject<BlockchainExplorerAccountSynchronizationSavedState>("state", savedState.getValue())->commit();
            }
            auto accountUid = getAccountUid();
            sql << "DELETE FROM operations WHERE account_uid = :account_uid AND date >= :date ", soci::use(
                    accountUid), soci::use(date);
            log->debug(" Finish erasing data of account : {}", accountUid);
            return Future<api::ErrorCode>::successful(api::ErrorCode::FUTURE_WAS_SUCCESSFULL);

        }

        bool CosmosLikeAccount::isSynchronizing() {
            std::lock_guard<std::mutex> lock(_synchronizationLock);
            return _currentSyncEventBus != nullptr;
        }

        std::shared_ptr<api::EventBus> CosmosLikeAccount::synchronize() {
            std::lock_guard<std::mutex> lock(_synchronizationLock);
            if (_currentSyncEventBus)
                return _currentSyncEventBus;
            auto eventPublisher = std::make_shared<EventPublisher>(getContext());

            _currentSyncEventBus = eventPublisher->getEventBus();
            auto future = _synchronizer->synchronize(
                    std::static_pointer_cast<CosmosLikeAccount>(shared_from_this()))->getFuture();
            auto self = std::static_pointer_cast<CosmosLikeAccount>(shared_from_this());

            //Update current block height (needed to compute trust level)
            _explorer->getCurrentBlock().onComplete(getContext(),
                                                    [self](const TryPtr<CosmosLikeBlockchainExplorer::Block> &block) mutable {
                                                        if (block.isSuccess()) {
                                                            self->_currentBlockHeight = block.getValue()->height;
                                                        }
                                                    });

            auto startTime = DateUtils::now();
            eventPublisher->postSticky(
                    std::make_shared<Event>(api::EventCode::SYNCHRONIZATION_STARTED, api::DynamicObject::newInstance()),
                    0);
            future.onComplete(getContext(), [eventPublisher, self, startTime](const Try<Unit> &result) {
                api::EventCode code;
                auto payload = std::make_shared<DynamicObject>();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        DateUtils::now() - startTime).count();
                payload->putLong(api::Account::EV_SYNC_DURATION_MS, duration);
                if (result.isSuccess()) {
                    code = api::EventCode::SYNCHRONIZATION_SUCCEED;
                } else {
                    code = api::EventCode::SYNCHRONIZATION_FAILED;
                    payload->putString(api::Account::EV_SYNC_ERROR_CODE,
                                       api::to_string(result.getFailure().getErrorCode()));
                    payload->putInt(api::Account::EV_SYNC_ERROR_CODE_INT, (int32_t) result.getFailure().getErrorCode());
                    payload->putString(api::Account::EV_SYNC_ERROR_MESSAGE, result.getFailure().getMessage());
                }
                eventPublisher->postSticky(std::make_shared<Event>(code, payload), 0);
                std::lock_guard<std::mutex> lock(self->_synchronizationLock);
                self->_currentSyncEventBus = nullptr;

            });
            return eventPublisher->getEventBus();
        }

        std::shared_ptr<CosmosLikeAccount> CosmosLikeAccount::getSelf() {
            return std::dynamic_pointer_cast<CosmosLikeAccount>(shared_from_this());
        }

        void CosmosLikeAccount::startBlockchainObservation() {
            _observer->registerAccount(getSelf());
        }

        void CosmosLikeAccount::stopBlockchainObservation() {
            _observer->unregisterAccount(getSelf());
        }

        bool CosmosLikeAccount::isObservingBlockchain() {
            return _observer->isRegistered(getSelf());
        }

        std::string CosmosLikeAccount::getRestoreKey() {
            return _keychain->getRestoreKey();
        }

        void CosmosLikeAccount::broadcastRawTransaction(const std::string &transaction,
                                                        const std::shared_ptr<api::StringCallback> &callback) {
            std::vector<uint8_t> tx{transaction.begin(), transaction.end()};
//            _explorer->pushTransaction(tx).map<std::string>(getContext(),
//                                                                     [](const String &seq) -> std::string {
//                                                                         //TODO: optimistic update
//                                                                         return seq.str();
//                                                                     }).callback(getContext(), callback);
        }

        void CosmosLikeAccount::broadcastTransaction(const std::shared_ptr<api::CosmosLikeTransaction> &transaction,
                                                    const std::shared_ptr<api::StringCallback> &callback) {
            broadcastRawTransaction(transaction->serialize(), callback);
        }

        std::shared_ptr<api::CosmosLikeTransactionBuilder> CosmosLikeAccount::buildTransaction() {
            return buildTransaction(std::dynamic_pointer_cast<CosmosLikeAddress>(getKeychain()->getAddress())->toString());
        }

        std::shared_ptr<api::CosmosLikeTransactionBuilder> CosmosLikeAccount::buildTransaction(const std::string &senderAddress) {
            auto self = std::dynamic_pointer_cast<CosmosLikeAccount>(shared_from_this());
            auto buildFunction = [self, senderAddress](const CosmosLikeTransactionBuildRequest &request,
                                        const std::shared_ptr<CosmosLikeBlockchainExplorer> &explorer) {
                auto currency = self->getWallet()->getCurrency();
                auto tx = std::make_shared<CosmosLikeTransactionApi>(self->getWallet()->getCurrency());
                tx->setFee(request.fee);
                tx->setGas(request.gas);
                tx->setMessages(request.messages);
                tx->setSigningPubKey(self->getKeychain()->getPublicKey());
                tx->setSequence(request.sequence);
                tx->setMemo(request.memo);
                return Future<std::shared_ptr<api::CosmosLikeTransaction>>::successful(tx);
            };
            return std::make_shared<CosmosLikeTransactionBuilder>(getContext(),
                                                                 getWallet()->getCurrency(),
                                                                 _explorer,
                                                                 logger(),
                                                                 buildFunction);
        }

    }
}
