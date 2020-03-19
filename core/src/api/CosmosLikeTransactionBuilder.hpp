// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from wallet.djinni

#ifndef DJINNI_GENERATED_COSMOSLIKETRANSACTIONBUILDER_HPP
#define DJINNI_GENERATED_COSMOSLIKETRANSACTIONBUILDER_HPP

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

class Amount;
class CosmosLikeMessage;
class CosmosLikeTransaction;
class CosmosLikeTransactionCallback;
struct Currency;

class LIBCORE_EXPORT CosmosLikeTransactionBuilder {
public:
    virtual ~CosmosLikeTransactionBuilder() {}

    /**
     * Set memo
     * @param memo the memo to set
     */
    virtual std::shared_ptr<CosmosLikeTransactionBuilder> setMemo(const std::string & memo) = 0;

    /**
     * Add a new message in the internal range of messages
     * @param msg a new message
     */
    virtual std::shared_ptr<CosmosLikeTransactionBuilder> addMessage(const std::shared_ptr<CosmosLikeMessage> & msg) = 0;

    /**
     * Set gas price
     * @param gas The gas to set
     */
    virtual std::shared_ptr<CosmosLikeTransactionBuilder> setGas(const std::shared_ptr<Amount> & gas) = 0;

    /**
     * Set fee
     * Here the fee represents the gas price multiplied by the gas used
     * @param fee The fee to set
     */
    virtual std::shared_ptr<CosmosLikeTransactionBuilder> setFee(const std::shared_ptr<Amount> & fee) = 0;

    /** Build a transaction from the given builder parameters. */
    virtual void build(const std::shared_ptr<CosmosLikeTransactionCallback> & callback) = 0;

    /**
     * Creates a clone of this builder.
     * @return A copy of the current builder instance.
     */
    virtual std::shared_ptr<CosmosLikeTransactionBuilder> clone() = 0;

    /** Reset the current instance to its initial state */
    virtual void reset() = 0;

    static std::shared_ptr<CosmosLikeTransaction> parseRawUnsignedTransaction(const Currency & currency, const std::string & rawTransaction);

    static std::shared_ptr<CosmosLikeTransaction> parseRawSignedTransaction(const Currency & currency, const std::string & rawTransaction);
};

} } }  // namespace ledger::core::api
#endif //DJINNI_GENERATED_COSMOSLIKETRANSACTIONBUILDER_HPP
