/*
 *
 * dynamics_tests
 * ledger-core
 *
 * Created by Pierre Pollastri on 09/03/2017.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Ledger
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <gtest/gtest.h>
#include <rapidjson/document.h>

#include <core/api/DynamicArray.hpp>
#include <core/api/DynamicObject.hpp>
#include <core/collections/DynamicObject.hpp>
#include <core/collections/DynamicValue.hpp>
#include <core/utils/Optional.hpp>

using namespace ledger::core::api;

TEST(Dynamics, Value) {
    int64_t i64 = 9364936249LL;

    EXPECT_EQ(*ledger::core::DynamicValue(std::string("foo")).get<std::string>(), "foo");
    EXPECT_EQ(*ledger::core::DynamicValue(3946).get<int32_t>(), 3946);
    EXPECT_EQ(*ledger::core::DynamicValue(i64).get<int64_t>(), i64);
    EXPECT_EQ(*ledger::core::DynamicValue(3.141592F).get<double>(), 3.141592F);
    EXPECT_EQ(*ledger::core::DynamicValue(true).get<bool>(), true);
}

TEST(Dynamics, ValueSerialization) {
    // serialize
    std::stringstream is;
    ::cereal::PortableBinaryOutputArchive oarchive(is);
    oarchive(ledger::core::DynamicValue(std::string("foobarzoo")));

    // deserialize
    ledger::core::DynamicValue v;
    ::cereal::PortableBinaryInputArchive iarchive(is);
    iarchive(v);

    EXPECT_EQ(*v.get<std::string>(), "foobarzoo");
}

TEST(Dynamics, Array) {
    auto array = DynamicArray::newInstance();
    array
    ->pushBoolean(true)
    ->pushDouble(12.6)
    ->pushString("Hello World")
    ->pushInt(12)
    ->pushLong(16)
    ->pushData({0x09, 0x03});

    EXPECT_EQ(array->size(), 6);
    EXPECT_EQ(array->getBoolean(0).value(), true);
    EXPECT_EQ(array->getDouble(1).value(), 12.6);
    EXPECT_EQ(array->getString(2).value(), "Hello World");
    EXPECT_EQ(array->getInt(3).value(), 12);
    EXPECT_EQ(array->getLong(4).value(), 16);
    EXPECT_EQ(array->getData(5).value(), std::vector<uint8_t>({0x09, 0x03}));

    array->remove(2);
    EXPECT_EQ(array->getBoolean(0).value(), true);
    EXPECT_EQ(array->getDouble(1).value(), 12.6);
    EXPECT_EQ(array->getInt(2).value(), 12);
    EXPECT_EQ(array->getLong(3).value(), 16);
    EXPECT_EQ(array->getData(4).value(), std::vector<uint8_t>({0x09, 0x03}));
}

TEST(Dynamics, ArraySerialization) {
    std::vector<uint8_t> serialized;
    {
        auto array = DynamicArray::newInstance();
        array
        ->pushBoolean(true)
        ->pushDouble(12.6)
        ->pushString("Hello World")
        ->pushInt(12)
        ->pushLong(16)
        ->pushData({0x09, 0x03});
        serialized = array->serialize();
    }
    {
        auto array = DynamicArray::load(serialized);
        EXPECT_EQ(array->size(), 6);
        EXPECT_EQ(array->getBoolean(0).value(), true);
        EXPECT_EQ(array->getDouble(1).value(), 12.6);
        EXPECT_EQ(array->getString(2).value(), "Hello World");
        EXPECT_EQ(array->getInt(3).value(), 12);
        EXPECT_EQ(array->getLong(4).value(), 16);
        EXPECT_EQ(array->getData(5).value(), std::vector<uint8_t>({0x09, 0x03}));

        std::cout << array->dump() << std::endl;
    }
}

TEST(Dynamics, Object) {
    auto object = DynamicObject::newInstance();
    object
        ->putBoolean("boolean", true)
        ->putDouble("double", 12.6)
        ->putString("string", "Hello World")
        ->putInt("int", 12)
        ->putLong("long", 16)
        ->putData("data", {0x09, 0x03});
    EXPECT_EQ(object->size(), 6);
    EXPECT_EQ(object->getBoolean("boolean").value(), true);
    EXPECT_EQ(object->getDouble("double").value(), 12.6);
    EXPECT_EQ(object->getString("string").value(), "Hello World");
    EXPECT_EQ(object->getInt("int").value(), 12);
    EXPECT_EQ(object->getLong("long").value(), 16);
    EXPECT_EQ(object->getData("data").value(), std::vector<uint8_t>({0x09, 0x03}));
    std::cout << object->dump() << std::endl;
}

TEST(Dynamics, ObjectSerialization) {
    std::vector<uint8_t> serialized;
    {
        auto object = DynamicObject::newInstance();
        object
        ->putBoolean("boolean", true)
        ->putDouble("double", 12.6)
        ->putString("string", "Hello World")
        ->putInt("int", 12)
        ->putLong("long", 16)
        ->putData("data", {0x09, 0x03});
       serialized = object->serialize();
    }
    {
        auto object = DynamicObject::load(serialized);
        object
        ->putBoolean("boolean", true)
        ->putDouble("double", 12.6)
        ->putString("string", "Hello World")
        ->putInt("int", 12)
        ->putLong("long", 16)
        ->putData("data", {0x09, 0x03});
        EXPECT_EQ(object->size(), 6);
        EXPECT_EQ(object->getBoolean("boolean").value(), true);
        EXPECT_EQ(object->getDouble("double").value(), 12.6);
        EXPECT_EQ(object->getString("string").value(), "Hello World");
        EXPECT_EQ(object->getInt("int").value(), 12);
        EXPECT_EQ(object->getLong("long").value(), 16);
        EXPECT_EQ(object->getData("data").value(), std::vector<uint8_t>({0x09, 0x03}));
        std::cout << object->dump() << std::endl;
    }
}

TEST(Dynamics, ArrayWithObjects) {
    std::vector<uint8_t> serialized;
    {
        auto array = DynamicArray::newInstance();
        auto object = DynamicObject::newInstance();
        object
        ->putBoolean("boolean", true)
        ->putDouble("double", 12.6)
        ->putString("string", "Hello World")
        ->putInt("int", 12)
        ->putLong("long", 16)
        ->putData("data", {0x09, 0x03});
        array->pushBoolean(false);
        array->pushObject(object);
        serialized = array->serialize();
    }
    {
        auto array = DynamicArray::load(serialized);
        auto object = array->getObject(1);
        EXPECT_EQ(object->size(), 6);
        EXPECT_EQ(object->getBoolean("boolean").value(), true);
        EXPECT_EQ(object->getDouble("double").value(), 12.6);
        EXPECT_EQ(object->getString("string").value(), "Hello World");
        EXPECT_EQ(object->getInt("int").value(), 12);
        EXPECT_EQ(object->getLong("long").value(), 16);
        EXPECT_EQ(object->getData("data").value(), std::vector<uint8_t>({0x09, 0x03}));
        std::cout << array->dump() << std::endl;
    }
}

TEST(Dynamics, ObjectWithArray) {
    std::vector<uint8_t> serialized;
    {
        auto object = DynamicObject::newInstance();
        auto array = DynamicArray::newInstance();
        array
        ->pushBoolean(true)
        ->pushDouble(12.6)
        ->pushString("Hello World")
        ->pushInt(12)
        ->pushLong(16)
        ->pushData({0x09, 0x03});
        object->putArray("array", array);
        serialized = object->serialize();
    }
    {
        auto object = DynamicObject::load(serialized);
        auto array = object->getArray("array");
        EXPECT_EQ(array->size(), 6);
        EXPECT_EQ(array->getBoolean(0).value(), true);
        EXPECT_EQ(array->getDouble(1).value(), 12.6);
        EXPECT_EQ(array->getString(2).value(), "Hello World");
        EXPECT_EQ(array->getInt(3).value(), 12);
        EXPECT_EQ(array->getLong(4).value(), 16);
        EXPECT_EQ(array->getData(5).value(), std::vector<uint8_t>({0x09, 0x03}));
        std::cout << object->dump() << std::endl;
    }
}

TEST(Dynamics, OverwriteDynamicObject) {
    auto object = std::make_shared<ledger::core::DynamicObject>();
    object->putBoolean("boolean", true)
            ->putDouble("double", 12.6)
            ->putString("string", "Hello World")
            ->putInt("int", 12)
            ->putLong("long", 16)
            ->putData("data", {0x09, 0x03});

    auto replacement = std::make_shared<ledger::core::DynamicObject>();
    replacement->putString("string", "Hello")
            ->putInt("int", 1)
            ->putData("data", {0x19, 0x90});

    object->updateWithConfiguration(replacement);

    EXPECT_EQ(object->size(), 6);
    EXPECT_EQ(object->getBoolean("boolean").value(), true);
    EXPECT_EQ(object->getDouble("double").value(), 12.6);
    EXPECT_EQ(object->getString("string").value(), "Hello");
    EXPECT_EQ(object->getInt("int").value(), 1);
    EXPECT_EQ(object->getLong("long").value(), 16);
    EXPECT_EQ(object->getData("data").value(), std::vector<uint8_t>({0x19, 0x90}));
}
TEST(Dynamics, JsonFromDynamicObject) {
    using namespace rapidjson;

    Document document;
    document.SetObject();

    auto innerArrayObject = std::make_shared<ledger::core::DynamicObject>();
    innerArrayObject->putBoolean("boolean", false);

    auto boolArray = std::make_shared<ledger::core::DynamicArray>();
    boolArray->pushBoolean(false)
             ->pushBoolean(true);

    auto doubleArray = std::make_shared<ledger::core::DynamicArray>();
    doubleArray->pushDouble(21.8)
             ->pushDouble(14.500001)
             ->pushDouble(-48);

    auto intArray = std::make_shared<ledger::core::DynamicArray>();
    intArray->pushInt(30);

    auto longArray = std::make_shared<ledger::core::DynamicArray>();
    longArray->pushLong(21474836470)
             ->pushLong(-21474836470);

    auto stringArray = std::make_shared<ledger::core::DynamicArray>();
    stringArray->pushString("foobarzoo")
               ->pushString("test");

    auto dataArray = std::make_shared<ledger::core::DynamicArray>();
    dataArray->pushData({'y', 'e', 's'})
             ->pushData({'n', 'o'});

    auto innerObject = std::make_shared<ledger::core::DynamicObject>();
    innerObject->putArray("boolean", boolArray);
    innerObject->putArray("double", doubleArray);
    innerObject->putArray("int", intArray);
    innerObject->putArray("long", longArray);
    innerObject->putArray("string", stringArray);
    innerObject->putArray("data", dataArray);

    auto object = std::make_shared<ledger::core::DynamicObject>();
    object->putBoolean("boolean", true)
          ->putDouble("double", 42.1)
          ->putString("string", "Hello, World")
          ->putInt("int", 14)
          ->putLong("long", 21474836470)
          ->putData("data", {'t', 'e', 's', 't'})
          ->putObject("object", innerObject);

    auto value = object->toJson(document.GetAllocator());
    EXPECT_EQ(value["boolean"].GetType(), kTrueType);
    EXPECT_TRUE(value["double"].IsNumber());
    EXPECT_EQ(value["double"].GetDouble(), 42.1);
    EXPECT_TRUE(value["string"].IsString());
    EXPECT_STREQ(value["string"].GetString(), "Hello, World");
    EXPECT_TRUE(value["int"].IsNumber());
    EXPECT_EQ(value["int"].GetInt(), 14);
    EXPECT_TRUE(value["long"].IsNumber());
    EXPECT_EQ(value["long"].GetInt64(), 21474836470);
    EXPECT_TRUE(value["data"].IsString());
    EXPECT_STREQ(value["data"].GetString(), "test");
    EXPECT_TRUE(value["object"].IsObject());

    value = value["object"].GetObject();
    EXPECT_TRUE(value["boolean"].IsArray());
    EXPECT_TRUE(value["double"].IsArray());
    EXPECT_TRUE(value["int"].IsArray());
    EXPECT_TRUE(value["long"].IsArray());
    EXPECT_TRUE(value["string"].IsArray());
    EXPECT_TRUE(value["data"].IsArray());

    auto array = value["boolean"].GetArray();
    EXPECT_EQ(array.Size(), 2);
    EXPECT_TRUE(array[0].IsBool());
    EXPECT_EQ(array[0].GetType(), kFalseType);
    EXPECT_EQ(array[1].GetType(), kTrueType);

    array = value["double"].GetArray();
    EXPECT_EQ(array.Size(), 3);
    EXPECT_TRUE(array[0].IsDouble());
    EXPECT_EQ(array[0].GetDouble(), 21.8);
    EXPECT_EQ(array[1].GetDouble(), 14.500001);
    EXPECT_EQ(array[2].GetDouble(), -48);

   array = value["int"].GetArray();
    EXPECT_EQ(array.Size(), 1);
    EXPECT_TRUE(array[0].IsInt());
    EXPECT_EQ(array[0].GetInt(), 30);

    array = value["long"].GetArray();
    EXPECT_EQ(array.Size(), 2);
    EXPECT_TRUE(array[0].IsInt64());
    EXPECT_EQ(array[0].GetInt64(), 21474836470);
    EXPECT_EQ(array[1].GetInt64(), -21474836470);

    array = value["string"].GetArray();
    EXPECT_EQ(array.Size(), 2);
    EXPECT_TRUE(array[0].IsString());
    EXPECT_STREQ(array[0].GetString(), "foobarzoo");
    EXPECT_STREQ(array[1].GetString(), "test");

    array = value["data"].GetArray();
    EXPECT_EQ(array.Size(), 2);
    EXPECT_TRUE(array[0].IsString());
    EXPECT_STREQ(array[0].GetString(), "yes");
    EXPECT_STREQ(array[1].GetString(), "no");
}
