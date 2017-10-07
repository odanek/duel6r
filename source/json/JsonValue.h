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

#ifndef DUEL6_JSON_JSONVALUE_H
#define DUEL6_JSON_JSONVALUE_H

#include <string>
#include <unordered_set>
#include <memory>
#include "../Type.h"
#include "../File.h"
#include "JsonException.h"

namespace Duel6 {
    namespace Json {
        class ValueImpl;

        class Value {
        public:
            enum class Type {
                Null,
                Object,
                Array,
                Number,
                String,
                Boolean
            };

        private:
            std::shared_ptr<ValueImpl> value;

        private:
            Value(std::shared_ptr<ValueImpl> value);

        public:
            Value();

            Value(const Value &value);

            Value &operator=(const Value &value);

            Type getType() const;

            Value get(const std::string &propertyName);

            const Value get(const std::string &propertyName) const;

            Value getOrDefault(const std::string &propertyName, Value defaultValue);

            const Value getOrDefault(const std::string &propertyName, const Value defaultValue) const;

            Value &set(const std::string &propertyName, Value value);

            std::vector<std::string> getPropertyNames() const;

            Value get(const Size index);

            const Value get(const Size index) const;

            Value &set(const Size index, Value value);

            Value &add(Value value);

            Size getLength() const;

            std::string asString() const;

            Int32 asInt() const;

            Float64 asDouble() const;

            bool asBoolean() const;

        public:
            static Value makeString(const std::string &val);

            static Value makeNumber(Int32 val);

            static Value makeNumber(Float64 val);

            static Value makeBoolean(bool val);

            static Value makeArray();

            static Value makeObject();

            static Value makeNull();
        };
    }
}

#endif
