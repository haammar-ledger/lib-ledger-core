// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from wallet.djinni

#ifndef DJINNI_GENERATED_COSMOSLIKEACCOUNT_HPP
#define DJINNI_GENERATED_COSMOSLIKEACCOUNT_HPP

#include <memory>
#include <string>
#ifndef LIBCORE_EXPORT
    #if defined(_MSC_VER)
       #include <libcore_export.h>
    #else
       #define LIBCORE_EXPORT
    #endif
#endif

namespace ledger { namespace core { namespace api {

class AmountCallback;
class BigIntCallback;
class CosmosLikeTransaction;
class CosmosLikeTransactionBuilder;
class CosmosLikeValidatorCallback;
class CosmosLikeValidatorListCallback;
class StringCallback;

/**Class representing a Cosmos account */
class LIBCORE_EXPORT CosmosLikeAccount {
public:
    virtual ~CosmosLikeAccount() {}

    virtual void broadcastRawTransaction(const std::string & transaction, const std::shared_ptr<StringCallback> & callback) = 0;

    virtual void broadcastTransaction(const std::shared_ptr<CosmosLikeTransaction> & transaction, const std::shared_ptr<StringCallback> & callback) = 0;

    virtual std::shared_ptr<CosmosLikeTransactionBuilder> buildTransaction() = 0;

    /** Get estimated gas limit to set so the transaction will succeed */
    virtual void getEstimatedGasLimit(const std::shared_ptr<CosmosLikeTransaction> & transaction, const std::shared_ptr<BigIntCallback> & callback) = 0;

    /** Get the latest active validator set */
    virtual void getLatestValidatorSet(const std::shared_ptr<CosmosLikeValidatorListCallback> & callback) = 0;

    /** Get information about one validator */
    virtual void getValidatorInfo(const std::string & validatorAddress, const std::shared_ptr<CosmosLikeValidatorCallback> & callback) = 0;

    /** Get Total balance of account. Sum of spendable, delegated, pending rewards, and pending unbondings */
    virtual void getTotalBalance(const std::shared_ptr<AmountCallback> & callback) = 0;

    /** Get Total amount in delegation of account. */
    virtual void getDelegatedBalance(const std::shared_ptr<AmountCallback> & callback) = 0;

    /** Get Total pending rewards of account. */
    virtual void getPendingRewardsBalance(const std::shared_ptr<AmountCallback> & callback) = 0;

    /** Get Total unbondings funds of account. */
    virtual void getUnbondingBalance(const std::shared_ptr<AmountCallback> & callback) = 0;

    /** Get Total spendable balance of account. */
    virtual void getSpendableBalance(const std::shared_ptr<AmountCallback> & callback) = 0;
};

} } }  // namespace ledger::core::api
#endif //DJINNI_GENERATED_COSMOSLIKEACCOUNT_HPP
