// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from callback.djinni

#ifndef DJINNI_GENERATED_NJSBINARYCALLBACK_HPP
#define DJINNI_GENERATED_NJSBINARYCALLBACK_HPP


#include "../../../core/src/api/../utils/optional.hpp"
#include "../../../core/src/api/Error.hpp"
#include <cstdint>
#include <vector>

#include <nan.h>
#include <node.h>
#include "../../../core/src/api/BinaryCallback.hpp"

using namespace v8;
using namespace node;
using namespace std;
using namespace ledger::core::api;

class NJSBinaryCallback: public Nan::ObjectWrap, public ledger::core::api::BinaryCallback {
public:

    static void Initialize(Local<Object> target);
    ~NJSBinaryCallback()
    {
        njs_impl.Reset();
        pers_resolver.Reset();
    };
    NJSBinaryCallback(Local<Object> njs_implementation){njs_impl.Reset(njs_implementation);};

    void onCallback(const std::experimental::optional<std::vector<uint8_t>> & result, const std::experimental::optional<Error> & error);
    void SetPromise(Local<Promise::Resolver> resolver)
    {
        pers_resolver.Reset(resolver);
    }

private:
    static NAN_METHOD(New);

    static NAN_METHOD(addRef);
    static NAN_METHOD(removeRef);
    Nan::Persistent<Object> njs_impl;
    Nan::Persistent<Promise::Resolver> pers_resolver;
};
#endif //DJINNI_GENERATED_NJSBINARYCALLBACK_HPP
