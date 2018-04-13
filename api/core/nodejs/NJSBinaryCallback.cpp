// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from callback.djinni

#include "NJSBinaryCallback.hpp"

using namespace v8;
using namespace node;
using namespace std;

void NJSBinaryCallback::onCallback(const std::experimental::optional<std::vector<uint8_t>> & result, const std::experimental::optional<Error> & error)
{
    Nan::HandleScope scope;
    //Wrap parameters
    Local<Array> arg_0 = Nan::New<Array>();
    for(size_t arg_0_id = 0; arg_0_id < (*result).size(); arg_0_id++)
    {
        auto arg_0_elem = Nan::New<Uint32>((*result)[arg_0_id]);
        arg_0->Set((int)arg_0_id,arg_0_elem);
    }

    auto arg_1 = Nan::New<Object>();
    auto arg_1_1 = Nan::New<Integer>((int)(*error).code);
    Nan::DefineOwnProperty(arg_1, Nan::New<String>("code").ToLocalChecked(), arg_1_1);
    auto arg_1_2 = Nan::New<String>((*error).message).ToLocalChecked();
    Nan::DefineOwnProperty(arg_1, Nan::New<String>("message").ToLocalChecked(), arg_1_2);

    auto local_resolver = Nan::New<Promise::Resolver>(pers_resolver);
    if(error)
    {
        auto rejected = local_resolver->Reject(Nan::GetCurrentContext(), arg_1);
        rejected.FromJust();
    }
    else
    {
        auto resolve = local_resolver->Resolve(Nan::GetCurrentContext(), arg_0);
        resolve.FromJust();
    }
}

NAN_METHOD(NJSBinaryCallback::addRef) {

    NJSBinaryCallback *obj = Nan::ObjectWrap::Unwrap<NJSBinaryCallback>(info.This());
    obj->Ref();
}

NAN_METHOD(NJSBinaryCallback::removeRef) {

    NJSBinaryCallback *obj = Nan::ObjectWrap::Unwrap<NJSBinaryCallback>(info.This());
    obj->Unref();
}

NAN_METHOD(NJSBinaryCallback::New) {
    //Only new allowed
    if(!info.IsConstructCall())
    {
        return Nan::ThrowError("NJSBinaryCallback function can only be called as constructor (use New)");
    }

    NJSBinaryCallback *node_instance = nullptr;
    if(info[0]->IsObject())
    {
        node_instance = new NJSBinaryCallback(info[0]->ToObject());
    }
    else
    {
        return Nan::ThrowError("NJSBinaryCallback::New requires an implementation from node");
    }

    if(node_instance)
    {
        //Wrap and return node instance
        node_instance->Wrap(info.This());
        node_instance->Ref();
        info.GetReturnValue().Set(info.This());
    }
}

void NJSBinaryCallback::Initialize(Local<Object> target) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> func_template = Nan::New<FunctionTemplate>(NJSBinaryCallback::New);
    Local<ObjectTemplate> objectTemplate = func_template->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(1);

    func_template->SetClassName(Nan::New<String>("NJSBinaryCallback").ToLocalChecked());
    Nan::SetPrototypeMethod(func_template,"addRef", addRef);
    Nan::SetPrototypeMethod(func_template,"removeRef", removeRef);

    //Add template to target
    target->Set(Nan::New<String>("NJSBinaryCallback").ToLocalChecked(), func_template->GetFunction());
}
