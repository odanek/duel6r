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

#ifndef DUEL6_JSON_JSON_H
#define DUEL6_JSON_JSON_H

#include <string>
#include <unordered_set>
#include <memory>
#include "../Type.h"
#include "../File.h"
#include "JsonException.h"

namespace Duel6
{
	namespace Json
	{
		class Value
		{
		public:
			enum class Type
			{
				Null,
				Object,
				Array,
				Number,
				String,
				Boolean
			};

		public:
			virtual ~Value()
			{}

			virtual Type getType() const = 0;
			virtual const Value& get(const std::string& propertyName) const = 0;
			virtual const Value& get(const Size index) const = 0;
			virtual Size getLength() const = 0;
			virtual std::string asString() const = 0;
			virtual Int32 asInt() const = 0;
			virtual Float64 asDouble() const = 0;
			virtual bool asBoolean() const = 0;
		};

		class Parser
		{
		private:
			static std::unordered_set<Uint8> stringSentinel;
			static std::unordered_set<Uint8> numberChars;
			std::unique_ptr<Value> root;

		private:
			Uint8 peekNextCharacter(File& file) const;
			void readExpected(File& file, const std::string& expected) const;
			void readExpected(File& file, char expected) const;
			void readWhitespaceAndExpected(File& file, char expected) const;
			std::string readUntil(File& file, const std::unordered_set<Uint8>& sentinels) const;
			std::string readWhile(File& file, const std::unordered_set<Uint8>& allowed) const;
			Value::Type determineValueType(Uint8 firstByte) const;
						
			std::unique_ptr<Value> parseValue(File& file) const;
			std::unique_ptr<Value> parseNull(File& file) const;
			std::unique_ptr<Value> parseObject(File& file) const;
			std::unique_ptr<Value> parseArray(File& file) const;
			std::unique_ptr<Value> parseNumber(File& file) const;
			std::unique_ptr<Value> parseString(File& file) const;
			std::unique_ptr<Value> parseBoolean(File& file) const;

		public:
			const Value& parse(const std::string& fileName);
		};
	}
}

#endif
