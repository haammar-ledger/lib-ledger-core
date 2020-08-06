// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from idl.djinni

#include "AlgorandAssetTransferInfo.hpp"  // my header
#include "Marshal.hpp"

namespace djinni_generated {

AlgorandAssetTransferInfo::AlgorandAssetTransferInfo() = default;

AlgorandAssetTransferInfo::~AlgorandAssetTransferInfo() = default;

auto AlgorandAssetTransferInfo::fromCpp(JNIEnv* jniEnv, const CppType& c) -> ::djinni::LocalRef<JniType> {
    const auto& data = ::djinni::JniClass<AlgorandAssetTransferInfo>::get();
    auto r = ::djinni::LocalRef<JniType>{jniEnv->NewObject(data.clazz.get(), data.jconstructor,
                                                           ::djinni::get(::djinni::String::fromCpp(jniEnv, c.assetId)),
                                                           ::djinni::get(::djinni::Optional<std::experimental::optional, ::djinni::String>::fromCpp(jniEnv, c.amount)),
                                                           ::djinni::get(::djinni::String::fromCpp(jniEnv, c.recipientAddress)),
                                                           ::djinni::get(::djinni::Optional<std::experimental::optional, ::djinni::String>::fromCpp(jniEnv, c.closeAddress)),
                                                           ::djinni::get(::djinni::Optional<std::experimental::optional, ::djinni::String>::fromCpp(jniEnv, c.clawedBackAddress)),
                                                           ::djinni::get(::djinni::Optional<std::experimental::optional, ::djinni::String>::fromCpp(jniEnv, c.closeAmount)))};
    ::djinni::jniExceptionCheck(jniEnv);
    return r;
}

auto AlgorandAssetTransferInfo::toCpp(JNIEnv* jniEnv, JniType j) -> CppType {
    ::djinni::JniLocalScope jscope(jniEnv, 7);
    assert(j != nullptr);
    const auto& data = ::djinni::JniClass<AlgorandAssetTransferInfo>::get();
    return {::djinni::String::toCpp(jniEnv, (jstring)jniEnv->GetObjectField(j, data.field_assetId)),
            ::djinni::Optional<std::experimental::optional, ::djinni::String>::toCpp(jniEnv, (jstring)jniEnv->GetObjectField(j, data.field_amount)),
            ::djinni::String::toCpp(jniEnv, (jstring)jniEnv->GetObjectField(j, data.field_recipientAddress)),
            ::djinni::Optional<std::experimental::optional, ::djinni::String>::toCpp(jniEnv, (jstring)jniEnv->GetObjectField(j, data.field_closeAddress)),
            ::djinni::Optional<std::experimental::optional, ::djinni::String>::toCpp(jniEnv, (jstring)jniEnv->GetObjectField(j, data.field_clawedBackAddress)),
            ::djinni::Optional<std::experimental::optional, ::djinni::String>::toCpp(jniEnv, (jstring)jniEnv->GetObjectField(j, data.field_closeAmount))};
}

}  // namespace djinni_generated
