// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from configuration.djinni

#ifndef DJINNI_GENERATED_ALGORANDBLOCKCHAINEXPLORERENGINES_HPP_JNI_
#define DJINNI_GENERATED_ALGORANDBLOCKCHAINEXPLORERENGINES_HPP_JNI_

#include "../../api/AlgorandBlockchainExplorerEngines.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

class AlgorandBlockchainExplorerEngines final : ::djinni::JniInterface<::ledger::core::api::AlgorandBlockchainExplorerEngines, AlgorandBlockchainExplorerEngines> {
public:
    using CppType = std::shared_ptr<::ledger::core::api::AlgorandBlockchainExplorerEngines>;
    using CppOptType = std::shared_ptr<::ledger::core::api::AlgorandBlockchainExplorerEngines>;
    using JniType = jobject;

    using Boxed = AlgorandBlockchainExplorerEngines;

    ~AlgorandBlockchainExplorerEngines();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<AlgorandBlockchainExplorerEngines>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<AlgorandBlockchainExplorerEngines>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    AlgorandBlockchainExplorerEngines();
    friend ::djinni::JniClass<AlgorandBlockchainExplorerEngines>;
    friend ::djinni::JniInterface<::ledger::core::api::AlgorandBlockchainExplorerEngines, AlgorandBlockchainExplorerEngines>;

};

}  // namespace djinni_generated
#endif //DJINNI_GENERATED_ALGORANDBLOCKCHAINEXPLORERENGINES_HPP_JNI_
