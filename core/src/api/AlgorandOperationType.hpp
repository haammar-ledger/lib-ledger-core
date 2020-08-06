// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from idl.djinni

#ifndef DJINNI_GENERATED_ALGORANDOPERATIONTYPE_HPP
#define DJINNI_GENERATED_ALGORANDOPERATIONTYPE_HPP

#include <functional>
#include <iostream>
#include <string>
#ifndef LIBCORE_EXPORT
    #if defined(_MSC_VER)
       #include <libcore_export.h>
    #else
       #define LIBCORE_EXPORT
    #endif
#endif

namespace ledger { namespace core { namespace api {

enum class AlgorandOperationType : int {
    PAYMENT,
    ACCOUNT_CLOSE,
    ACCOUNT_REGISTER_ONLINE,
    ACCOUNT_REGISTER_OFFLINE,
    ASSET_CREATE,
    ASSET_RECONFIGURE,
    ASSET_DESTROY,
    ASSET_OPT_IN,
    ASSET_OPT_OUT,
    ASSET_TRANSFER,
    ASSET_REVOKE,
    ASSET_FREEZE,
    UNSUPPORTED,
};
LIBCORE_EXPORT  std::string to_string(const AlgorandOperationType& algorandOperationType);
LIBCORE_EXPORT  std::ostream &operator<<(std::ostream &os, const AlgorandOperationType &o);

} } }  // namespace ledger::core::api

namespace std {

template <>
struct hash<::ledger::core::api::AlgorandOperationType> {
    size_t operator()(::ledger::core::api::AlgorandOperationType type) const {
        return std::hash<int>()(static_cast<int>(type));
    }
};

}  // namespace std
#endif //DJINNI_GENERATED_ALGORANDOPERATIONTYPE_HPP
