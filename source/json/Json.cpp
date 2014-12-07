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
#include "Json.h"
#include "../File.h"

namespace Duel6
{
	namespace Json
	{
		std::unordered_set<Uint8> Parser::stringSentinel = { '"' };
		std::unordered_set<Uint8> Parser::numberChars = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.', '-', 'e' };

		class ValueBase : public Value
		{
		protected:
			virtual std::string getTypeName() const = 0;

		public:
			const Value& get(const std::string& propertyName) const override
			{
				D6_THROW(JsonException, std::string("Invalid JSON value type - expected: Object, got: ") + getTypeName());
			}

			const Value& get(const Size index) const override
			{
				D6_THROW(JsonException, std::string("Invalid JSON value type - expected: Array, got: ") + getTypeName());
			}

			virtual Size getLength() const override
			{
				D6_THROW(JsonException, std::string("Invalid JSON value type - expected: Array, got: ") + getTypeName());
			}

			virtual std::string asString() const override
			{
				D6_THROW(JsonException, std::string("Invalid JSON value type - expected: String, got: ") + getTypeName());
			}

			virtual Int32 asInt() const override
			{
				D6_THROW(JsonException, std::string("Invalid JSON value type - expected: Number, got: ") + getTypeName());
			}

			virtual Float64 asDouble() const override
			{
				D6_THROW(JsonException, std::string("Invalid JSON value type - expected: Number, got: ") + getTypeName());
			}

			virtual bool asBoolean() const override
			{
				D6_THROW(JsonException, std::string("Invalid JSON value type - expected: Boolean, got: ") + getTypeName());
			}
		};

		class NullValue : public ValueBase
		{
		protected:
			std::string getTypeName() const override
			{
				return "Null";
			}

		public:
			NullValue()
			{}

			Type getType() const override
			{
				return Type::Null;
			}
		};

		class NumberValue : public ValueBase
		{
		private:
			Float64 val;

		protected:
			std::string getTypeName() const override
			{
				return "Number";
			}

		public:
			NumberValue(const Float64& val)
				: val(val)
			{}

			Int32 asInt() const override
			{
				return (Int32)val;
			}

			Float64 asDouble() const override
			{
				return val;
			}

			Type getType() const override
			{
				return Type::Number;
			}
		};

		class StringValue : public ValueBase
		{
		private:
			std::string val;

		protected:
			std::string getTypeName() const override
			{
				return "String";
			}

		public:
			StringValue(const std::string& val)
				: val(val)
			{}

			std::string asString() const override
			{
				return val;
			}

			Type getType() const override
			{
				return Type::String;
			}
		};

		class BooleanValue : public ValueBase
		{
		private:
			bool val;

		protected:
			std::string getTypeName() const override
			{
				return "Boolean";
			}

		public:
			BooleanValue(const bool& val)
				: val(val)
			{}

			bool asBoolean() const override
			{
				return val;
			}

			Type getType() const override
			{
				return Type::Boolean;
			}
		};

		class ArrayValue : public ValueBase
		{
		public:
			typedef std::vector<std::unique_ptr<Value>> ValueArray;

		private:
			 ValueArray values;

		protected:
			std::string getTypeName() const override
			{
				return "Array";
			}

		public:
			ArrayValue(ValueArray&& values)
				: values(std::forward<ValueArray>(values))
			{}

			const Value& get(const Size index) const override
			{
				return *values[index];
			}

			virtual Size getLength() const override
			{
				return values.size();
			}

			Type getType() const override
			{
				return Type::Array;
			}
		};

		class ObjectValue : public ValueBase
		{
		public:
			typedef std::unordered_map<std::string, std::unique_ptr<Value>> PropertyMap;

		private:
			PropertyMap properties;

		protected:
			std::string getTypeName() const override
			{
				return "Object";
			}

		public:
			ObjectValue(PropertyMap&& properties)
				: properties(std::forward<PropertyMap>(properties))
			{}

			const Value& get(const std::string& propertyName) const override
			{
				return *properties.at(propertyName);
			}

			Type getType() const override
			{
				return Type::Object;
			}
		};

		const Value& Parser::parse(const std::string& fileName)
		{
			File file(fileName, "rb");
			root.reset();
			root = parseValue(file);
			return *root;
		}

		std::unique_ptr<Value> Parser::parseValue(File& file) const
		{
			Uint8 byte = peekNextCharacter(file);
			Value::Type type = determineValueType(byte);

			switch (type)
			{
			case Value::Type::Null: return parseNull(file);
			case Value::Type::Object: return parseObject(file);
			case Value::Type::Array: return parseArray(file);
			case Value::Type::Number: return parseNumber(file);
			case Value::Type::String: return parseString(file);
			case Value::Type::Boolean: return parseBoolean(file);
			}

			D6_THROW(JsonException, "Unhandled type: " + std::to_string((Int32)type));
		}

		std::unique_ptr<Value> Parser::parseNull(File& file) const
		{
			readExpected(file, "null");
			return std::make_unique<NullValue>();
		}

		std::unique_ptr<Value> Parser::parseObject(File& file) const
		{
			ObjectValue::PropertyMap properties;			

			readExpected(file, '{');
			Uint8 next = peekNextCharacter(file);
			if (next == '}')
			{
				readExpected(file, '}');
			}
			else
			{
				do
				{
					readWhitespaceAndExpected(file, '"');
					std::string propName = readUntil(file, stringSentinel);
					readExpected(file, '"');
					readWhitespaceAndExpected(file, ':');
					std::unique_ptr<Value> val = parseValue(file);
					properties[propName] = std::move(val);

					next = peekNextCharacter(file);

					if (next != ',' && next != '}')
					{
						D6_THROW(JsonException, std::string("Expect next property or end of object, got: ") + (char)next);
					}

					readExpected(file, (char)next);
				} while (next == ',');
			}

			return std::make_unique<ObjectValue>(std::move(properties));
		}

		std::unique_ptr<Value> Parser::parseArray(File& file) const
		{
			ArrayValue::ValueArray values;

			readExpected(file, '[');
			Uint8 next = peekNextCharacter(file);
			if (next == ']')
			{
				readExpected(file, ']');				
			}
			else
			{
				do
				{
					values.push_back(parseValue(file));
					next = peekNextCharacter(file);

					if (next != ',' && next != ']')
					{
						D6_THROW(JsonException, std::string("Expect next item or end of array, got: ") + (char)next);
					}

					readExpected(file, (char)next);
				} while (next == ',');
			}

			return std::make_unique<ArrayValue>(std::move(values));
		}

		std::unique_ptr<Value> Parser::parseString(File& file) const
		{
			readExpected(file, '"');			
			std::string val = readUntil(file, stringSentinel);
			readExpected(file, '"');
			return std::make_unique<StringValue>(val);
		}

		std::unique_ptr<Value> Parser::parseNumber(File& file) const
		{			
			std::string val = readWhile(file, numberChars);
			return std::make_unique<NumberValue>(std::stod(val));
		}

		std::unique_ptr<Value> Parser::parseBoolean(File& file) const
		{
			Uint8 byte = peekNextCharacter(file);
			bool val = (byte == 't');
			readExpected(file, val ? "true" : "false");
			return std::make_unique<BooleanValue>(val);
		}

		Uint8 Parser::peekNextCharacter(File& file) const
		{
			while (!file.isEof())
			{
				Uint8 byte;
				file.read(&byte, 1, 1);

				if (byte != ' ' && byte != '\t' && byte != '\n' && byte != '\r')
				{
					file.seek(-1, File::Seek::Cur);
					return byte;
				}
			}

			D6_THROW(JsonException, "Unexpected end of input stream while skipping whitespace");
		}

		Value::Type Parser::determineValueType(Uint8 firstByte) const
		{
			if (firstByte == '{')
			{
				return Value::Type::Object;
			}
			else if (firstByte == '[')
			{
				return Value::Type::Array;
			}
			else if (firstByte == '"')
			{
				return Value::Type::String;
			}
			else if (firstByte == 't' || firstByte == 'f')
			{
				return Value::Type::Boolean;
			}
			else if ((firstByte >= '0' && firstByte <= '9') || firstByte == '-' || firstByte == '.')
			{
				return Value::Type::Number;
			}
			else if (firstByte == 'n')
			{
				return Value::Type::Null;
			}

			D6_THROW(JsonException, std::string("Invalid value type found, starting with: ") + (char)firstByte);
		}

		void Parser::readExpected(File& file, const std::string& expected) const
		{
			for (char chr : expected)
			{
				readExpected(file, chr);
			}
		}

		void Parser::readExpected(File& file, char expected) const
		{
			Uint8 byte;
			file.read(&byte, 1, 1);
			if (expected != byte)
			{
				D6_THROW(JsonException, std::string("Parsing error - expected: ") + expected + ", got: " + (char)byte);
			}
		}

		void Parser::readWhitespaceAndExpected(File& file, char expected) const
		{
			peekNextCharacter(file);
			readExpected(file, expected);
		}

		std::string Parser::readUntil(File& file, const std::unordered_set<Uint8>& sentinels) const
		{
			std::string result;

			while (!file.isEof())
			{
				Uint8 byte;
				file.read(&byte, 1, 1);
				if (sentinels.find(byte) != sentinels.end())
				{
					file.seek(-1, File::Seek::Cur);
					return result;
				}
				else
				{
					result += (char)byte;
				}
			}

			D6_THROW(JsonException, "Unexpected end of input stream while looking for sentinel");
		}

		std::string Parser::readWhile(File& file, const std::unordered_set<Uint8>& allowed) const
		{
			std::string result;

			while (!file.isEof())
			{
				Uint8 byte;
				file.read(&byte, 1, 1);
				if (allowed.find(byte) == allowed.end())
				{
					file.seek(-1, File::Seek::Cur);
					break;
				}
				else
				{
					result += (char)byte;
				}
			}

			return result;
		}
	}
}
