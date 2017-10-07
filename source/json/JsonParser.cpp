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

#include "JsonParser.h"

namespace Duel6 {
    namespace Json {
        namespace {
            std::unordered_set<Uint8> stringSentinel = {'"'};
            std::unordered_set<Uint8> numberChars = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.', '-',
                                                     'e'};
        }

        Value Parser::parse(const std::string &fileName) const {
            File file(fileName, File::Mode::Binary, File::Access::Read);
            return parseValue(file);
        }

        Value Parser::parseValue(File &file) const {
            Uint8 byte = peekNextCharacter(file);
            Value::Type type = determineValueType(byte);

            switch (type) {
                case Value::Type::Null:
                    return parseNull(file);
                case Value::Type::Object:
                    return parseObject(file);
                case Value::Type::Array:
                    return parseArray(file);
                case Value::Type::Number:
                    return parseNumber(file);
                case Value::Type::String:
                    return parseString(file);
                case Value::Type::Boolean:
                    return parseBoolean(file);
            }

            D6_THROW(JsonException, "Unhandled type: " + std::to_string((Int32) type));
        }

        Value Parser::parseNull(File &file) const {
            readExpected(file, "null");
            return Value::makeNull();
        }

        Value Parser::parseObject(File &file) const {
            Value value = Value::makeObject();

            readExpected(file, '{');
            Uint8 next = peekNextCharacter(file);
            if (next == '}') {
                readExpected(file, '}');
            } else {
                do {
                    readWhitespaceAndExpected(file, '"');
                    std::string propName = readUntil(file, stringSentinel);
                    readExpected(file, '"');
                    readWhitespaceAndExpected(file, ':');
                    value.set(propName, parseValue(file));

                    next = peekNextCharacter(file);

                    if (next != ',' && next != '}') {
                        D6_THROW(JsonException,
                                 std::string("Expected next property or end of object, got: ") + (char) next);
                    }

                    readExpected(file, (char) next);
                } while (next == ',');
            }

            return value;
        }

        Value Parser::parseArray(File &file) const {
            Value value = Value::makeArray();

            readExpected(file, '[');
            Uint8 next = peekNextCharacter(file);
            if (next == ']') {
                readExpected(file, ']');
            } else {
                do {
                    value.add(parseValue(file));
                    next = peekNextCharacter(file);

                    if (next != ',' && next != ']') {
                        D6_THROW(JsonException, std::string("Expect next item or end of array, got: ") + (char) next);
                    }

                    readExpected(file, (char) next);
                } while (next == ',');
            }

            return value;
        }

        Value Parser::parseString(File &file) const {
            readExpected(file, '"');
            std::string val = readUntil(file, stringSentinel);
            readExpected(file, '"');
            return Value::makeString(val);
        }

        Value Parser::parseNumber(File &file) const {
            std::string val = readWhile(file, numberChars);
            return Value::makeNumber(std::stod(val));
        }

        Value Parser::parseBoolean(File &file) const {
            Uint8 byte = peekNextCharacter(file);
            bool val = (byte == 't');
            readExpected(file, val ? "true" : "false");
            return Value::makeBoolean(val);
        }

        Uint8 Parser::peekNextCharacter(File &file) const {
            while (!file.isEof()) {
                Uint8 byte;
                file.read(&byte, 1, 1);

                if (byte != ' ' && byte != '\t' && byte != '\n' && byte != '\r') {
                    file.seek(-1, File::Seek::Cur);
                    return byte;
                }
            }

            D6_THROW(JsonException, "Unexpected end of input stream while skipping whitespace");
        }

        Value::Type Parser::determineValueType(Uint8 firstByte) const {
            if (firstByte == '{') {
                return Value::Type::Object;
            } else if (firstByte == '[') {
                return Value::Type::Array;
            } else if (firstByte == '"') {
                return Value::Type::String;
            } else if (firstByte == 't' || firstByte == 'f') {
                return Value::Type::Boolean;
            } else if ((firstByte >= '0' && firstByte <= '9') || firstByte == '-' || firstByte == '.') {
                return Value::Type::Number;
            } else if (firstByte == 'n') {
                return Value::Type::Null;
            }

            D6_THROW(JsonException, std::string("Invalid value type found, starting with: ") + (char) firstByte);
        }

        void Parser::readExpected(File &file, const std::string &expected) const {
            for (char chr : expected) {
                readExpected(file, chr);
            }
        }

        void Parser::readExpected(File &file, char expected) const {
            Uint8 byte;
            file.read(&byte, 1, 1);
            if (expected != byte) {
                D6_THROW(JsonException, std::string("Parsing error - expected: ") + expected + ", got: " + (char) byte);
            }
        }

        void Parser::readWhitespaceAndExpected(File &file, char expected) const {
            peekNextCharacter(file);
            readExpected(file, expected);
        }

        std::string Parser::readUntil(File &file, const std::unordered_set<Uint8> &sentinels) const {
            std::string result;

            while (!file.isEof()) {
                Uint8 byte;
                file.read(&byte, 1, 1);
                if (sentinels.find(byte) != sentinels.end()) {
                    file.seek(-1, File::Seek::Cur);
                    return result;
                } else {
                    result += (char) byte;
                }
            }

            D6_THROW(JsonException, "Unexpected end of input stream while looking for sentinel");
        }

        std::string Parser::readWhile(File &file, const std::unordered_set<Uint8> &allowed) const {
            std::string result;

            while (!file.isEof()) {
                Uint8 byte;
                file.read(&byte, 1, 1);
                if (allowed.find(byte) == allowed.end()) {
                    file.seek(-1, File::Seek::Cur);
                    break;
                } else {
                    result += (char) byte;
                }
            }

            return result;
        }
    }
}