/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <vector>
#include <unordered_map>
#include <memory>
#include "JsonValue.h"

namespace Duel6 {
    namespace Json {
        class ValueImpl {
        public:
            virtual ~ValueImpl() {}

            virtual Value::Type getType() const = 0;

            virtual std::shared_ptr<ValueImpl> &get(const std::string &propertyName) = 0;

            virtual const std::shared_ptr<ValueImpl> &get(const std::string &propertyName) const = 0;

            virtual std::shared_ptr<ValueImpl> &
            getOrDefault(const std::string &propertyName, std::shared_ptr<ValueImpl> &defaultValue) = 0;

            virtual const std::shared_ptr<ValueImpl> &
            getOrDefault(const std::string &propertyName, const std::shared_ptr<ValueImpl> &defaultValue) const = 0;

            virtual void set(const std::string &propertyName, std::shared_ptr<ValueImpl> &value) = 0;

            virtual std::vector<std::string> getPropertyNames() const = 0;

            virtual std::shared_ptr<ValueImpl> &get(Size index) = 0;

            virtual const std::shared_ptr<ValueImpl> &get(Size index) const = 0;

            virtual void set(Size index, std::shared_ptr<ValueImpl> &value) = 0;

            virtual void add(std::shared_ptr<ValueImpl> &value) = 0;

            virtual Size getLength() const = 0;

            virtual std::string asString() const = 0;

            virtual Int32 asInt() const = 0;

            virtual Float64 asDouble() const = 0;

            virtual bool asBoolean() const = 0;
        };


        class ValueImplBase : public ValueImpl {
        protected:
            virtual std::string getTypeName() const = 0;

        public:
            std::shared_ptr<ValueImpl> &get(const std::string &propertyName) override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: Object, got: ") + getTypeName());
            }

            const std::shared_ptr<ValueImpl> &get(const std::string &propertyName) const override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: Object, got: ") + getTypeName());
            }

            std::shared_ptr<ValueImpl> &
            getOrDefault(const std::string &propertyName, std::shared_ptr<ValueImpl> &defaultValue) override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: Object, got: ") + getTypeName());
            }

            const std::shared_ptr<ValueImpl> &getOrDefault(const std::string &propertyName,
                                                           const std::shared_ptr<ValueImpl> &defaultValue) const override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: Object, got: ") + getTypeName());
            }

            void set(const std::string &propertyName, std::shared_ptr<ValueImpl> &value) override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: Object, got: ") + getTypeName());
            }

            std::vector<std::string> getPropertyNames() const override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: Object, got: ") + getTypeName());
            }

            std::shared_ptr<ValueImpl> &get(Size index) override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: Array, got: ") + getTypeName());
            }

            const std::shared_ptr<ValueImpl> &get(Size index) const override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: Array, got: ") + getTypeName());
            }

            void set(Size index, std::shared_ptr<ValueImpl> &value) override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: Array, got: ") + getTypeName());
            }

            void add(std::shared_ptr<ValueImpl> &value) override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: Array, got: ") + getTypeName());
            }

            virtual Size getLength() const override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: Array, got: ") + getTypeName());
            }

            virtual std::string asString() const override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: String, got: ") + getTypeName());
            }

            virtual Int32 asInt() const override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: Number, got: ") + getTypeName());
            }

            virtual Float64 asDouble() const override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: Number, got: ") + getTypeName());
            }

            virtual bool asBoolean() const override {
                D6_THROW(JsonException,
                         std::string("Invalid JSON value type - expected: Boolean, got: ") + getTypeName());
            }
        };

        class NullValue : public ValueImplBase {
        protected:
            std::string getTypeName() const override {
                return "Null";
            }

        public:
            NullValue() {}

            Value::Type getType() const override {
                return Value::Type::Null;
            }
        };

        class NumberValue : public ValueImplBase {
        private:
            Float64 val;

        protected:
            std::string getTypeName() const override {
                return "Number";
            }

        public:
            NumberValue(const Float64 &val)
                    : val(val) {}

            Int32 asInt() const override {
                return (Int32) val;
            }

            Float64 asDouble() const override {
                return val;
            }

            Value::Type getType() const override {
                return Value::Type::Number;
            }
        };

        class StringValue : public ValueImplBase {
        private:
            std::string val;

        protected:
            std::string getTypeName() const override {
                return "String";
            }

        public:
            StringValue(const std::string &val)
                    : val(val) {}

            std::string asString() const override {
                return val;
            }

            Value::Type getType() const override {
                return Value::Type::String;
            }
        };

        class BooleanValue : public ValueImplBase {
        private:
            bool val;

        protected:
            std::string getTypeName() const override {
                return "Boolean";
            }

        public:
            BooleanValue(const bool &val)
                    : val(val) {}

            bool asBoolean() const override {
                return val;
            }

            Value::Type getType() const override {
                return Value::Type::Boolean;
            }
        };

        class ArrayValue : public ValueImplBase {
        public:
            typedef std::vector<std::shared_ptr<ValueImpl>> ValueArray;

        private:
            ValueArray values;

        protected:
            std::string getTypeName() const override {
                return "Array";
            }

        public:
            ArrayValue() {}

            std::shared_ptr<ValueImpl> &get(Size index) override {
                return values[index];
            }

            const std::shared_ptr<ValueImpl> &get(Size index) const override {
                return values[index];
            }

            void set(Size index, std::shared_ptr<ValueImpl> &value) override {
                values[index] = value;
            }

            void add(std::shared_ptr<ValueImpl> &value) override {
                values.push_back(value);
            }

            virtual Size getLength() const override {
                return values.size();
            }

            Value::Type getType() const override {
                return Value::Type::Array;
            }
        };

        class ObjectValue : public ValueImplBase {
        private:
            typedef std::unordered_map<std::string, std::shared_ptr<ValueImpl>> PropertyMap;

        private:
            PropertyMap properties;

        protected:
            std::string getTypeName() const override {
                return "Object";
            }

        public:
            ObjectValue() {}

            std::shared_ptr<ValueImpl> &get(const std::string &propertyName) override {
                auto val = properties.find(propertyName);
                if (val == properties.end()) {
                    D6_THROW(JsonException, std::string("Property ") + propertyName + " not found");
                }
                return val->second;
            }

            const std::shared_ptr<ValueImpl> &get(const std::string &propertyName) const override {
                auto val = properties.find(propertyName);
                if (val == properties.end()) {
                    D6_THROW(JsonException, std::string("Property ") + propertyName + " not found");
                }
                return val->second;
            }

            std::shared_ptr<ValueImpl> &
            getOrDefault(const std::string &propertyName, std::shared_ptr<ValueImpl> &defaultValue) override {
                auto val = properties.find(propertyName);
                return (val == properties.end()) ? defaultValue : val->second;
            }

            const std::shared_ptr<ValueImpl> &getOrDefault(const std::string &propertyName,
                                                           const std::shared_ptr<ValueImpl> &defaultValue) const override {
                auto val = properties.find(propertyName);
                return (val == properties.end()) ? defaultValue : val->second;
            }

            void set(const std::string &propertyName, std::shared_ptr<ValueImpl> &value) override {
                properties[propertyName] = value;
            }

            std::vector<std::string> getPropertyNames() const override {
                std::vector<std::string> propNames;
                for (const auto &iter : properties) {
                    propNames.push_back(iter.first);
                }
                return propNames;
            }

            Value::Type getType() const override {
                return Value::Type::Object;
            }
        };

        Value::Value()
                : value(std::make_shared<NullValue>()) {
        }

        Value::Value(std::shared_ptr<ValueImpl> value)
                : value(value) {
        }

        Value::Value(const Value &value)
                : value(value.value) {
        }

        Value &Value::operator=(const Value &value) {
            this->value = value.value;
            return *this;
        }

        Value::Type Value::getType() const {
            return value->getType();
        }

        Value Value::get(const std::string &propertyName) {
            return Value(value->get(propertyName));
        }

        const Value Value::get(const std::string &propertyName) const {
            return Value(value->get(propertyName));
        }

        Value Value::getOrDefault(const std::string &propertyName, Value defaultValue) {
            return Value(value->getOrDefault(propertyName, defaultValue.value));
        }

        const Value Value::getOrDefault(const std::string &propertyName, const Value defaultValue) const {
            return Value(value->getOrDefault(propertyName, defaultValue.value));
        }

        std::vector<std::string> Value::getPropertyNames() const {
            return value->getPropertyNames();
        }

        Value &Value::set(const std::string &propertyName, Value value) {
            this->value->set(propertyName, value.value);
            return *this;
        }

        Value Value::get(const Size index) {
            return Value(value->get(index));
        }

        const Value Value::get(const Size index) const {
            return Value(value->get(index));
        }

        Value &Value::set(const Size index, Value value) {
            this->value->set(index, value.value);
            return *this;
        }

        Value &Value::add(Value value) {
            this->value->add(value.value);
            return *this;
        }

        Size Value::getLength() const {
            return value->getLength();
        }

        std::string Value::asString() const {
            return value->asString();
        }

        Int32 Value::asInt() const {
            return value->asInt();
        }

        Float64 Value::asDouble() const {
            return value->asDouble();
        }

        bool Value::asBoolean() const {
            return value->asBoolean();
        }

        Value Value::makeString(const std::string &val) {
            return Value(std::make_shared<StringValue>(val));
        }

        Value Value::makeNumber(Int32 val) {
            return Value(std::make_shared<NumberValue>(val));
        }

        Value Value::makeNumber(Float64 val) {
            return Value(std::make_shared<NumberValue>(val));
        }

        Value Value::makeBoolean(bool val) {
            return Value(std::make_shared<BooleanValue>(val));
        }

        Value Value::makeArray() {
            return Value(std::make_shared<ArrayValue>());
        }

        Value Value::makeObject() {
            return Value(std::make_shared<ObjectValue>());
        }

        Value Value::makeNull() {
            return Value(std::make_shared<NullValue>());
        }
    }
}
