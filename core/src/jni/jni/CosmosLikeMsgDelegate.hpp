// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from messages.djinni

#ifndef DJINNI_GENERATED_COSMOSLIKEMSGDELEGATE_HPP_JNI_
#define DJINNI_GENERATED_COSMOSLIKEMSGDELEGATE_HPP_JNI_

#include "../../api/CosmosLikeMsgDelegate.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

class CosmosLikeMsgDelegate final {
public:
    using CppType = ::ledger::core::api::CosmosLikeMsgDelegate;
    using JniType = jobject;

    using Boxed = CosmosLikeMsgDelegate;

    ~CosmosLikeMsgDelegate();

    static CppType toCpp(JNIEnv* jniEnv, JniType j);
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c);

private:
    CosmosLikeMsgDelegate();
    friend ::djinni::JniClass<CosmosLikeMsgDelegate>;

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("CosmosLikeMsgDelegate") };
    const jmethodID jconstructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(Ljava/lang/String;Ljava/lang/String;LCosmosLikeAmount;)V") };
    const jfieldID field_delegatorAddress { ::djinni::jniGetFieldID(clazz.get(), "delegatorAddress", "Ljava/lang/String;") };
    const jfieldID field_validatorAddress { ::djinni::jniGetFieldID(clazz.get(), "validatorAddress", "Ljava/lang/String;") };
    const jfieldID field_amount { ::djinni::jniGetFieldID(clazz.get(), "amount", "LCosmosLikeAmount;") };
};

}  // namespace djinni_generated
#endif //DJINNI_GENERATED_COSMOSLIKEMSGDELEGATE_HPP_JNI_
