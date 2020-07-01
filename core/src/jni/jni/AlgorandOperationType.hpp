// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from idl.djinni

#ifndef DJINNI_GENERATED_ALGORANDOPERATIONTYPE_HPP_JNI_
#define DJINNI_GENERATED_ALGORANDOPERATIONTYPE_HPP_JNI_

#include "../../api/AlgorandOperationType.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

class AlgorandOperationType final : ::djinni::JniEnum {
public:
    using CppType = ::ledger::core::api::AlgorandOperationType;
    using JniType = jobject;

    using Boxed = AlgorandOperationType;

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return static_cast<CppType>(::djinni::JniClass<AlgorandOperationType>::get().ordinal(jniEnv, j)); }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, CppType c) { return ::djinni::JniClass<AlgorandOperationType>::get().create(jniEnv, static_cast<jint>(c)); }

private:
    AlgorandOperationType() : JniEnum("co/ledger/core/AlgorandOperationType") {}
    friend ::djinni::JniClass<AlgorandOperationType>;
};

}  // namespace djinni_generated
#endif //DJINNI_GENERATED_ALGORANDOPERATIONTYPE_HPP_JNI_
