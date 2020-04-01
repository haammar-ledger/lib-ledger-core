// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from wallet.djinni

#ifndef DJINNI_GENERATED_COSMOSLIKEREDELEGATIONENTRY_HPP
#define DJINNI_GENERATED_COSMOSLIKEREDELEGATIONENTRY_HPP

#include <chrono>
#include <memory>
#ifndef LIBCORE_EXPORT
    #if defined(_MSC_VER)
       #include <libcore_export.h>
    #else
       #define LIBCORE_EXPORT
    #endif
#endif

namespace ledger { namespace core { namespace api {

class BigInt;

class LIBCORE_EXPORT CosmosLikeRedelegationEntry {
public:
    virtual ~CosmosLikeRedelegationEntry() {}

    /** Block height of the begin redelegate request */
    virtual std::shared_ptr<BigInt> getCreationHeight() = 0;

    /** Timestamp of the redelegation completion */
    virtual std::chrono::system_clock::time_point getCompletionTime() = 0;

    /** Balance requested to redelegate */
    virtual std::shared_ptr<BigInt> getInitialBalance() = 0;

    /** Current amount being redelegated (i.e. less than initialBalance if slashed) */
    virtual std::shared_ptr<BigInt> getBalance() = 0;
};

} } }  // namespace ledger::core::api
#endif //DJINNI_GENERATED_COSMOSLIKEREDELEGATIONENTRY_HPP
