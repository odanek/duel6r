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

#include <cmath>
#include "JsonWriter.h"

namespace Duel6 {
    namespace Json {
        class Appender {
        public:
            virtual void append(const std::string &str) = 0;
        };

        class StringAppender : public Appender {
        private:
            std::string value;

        public:
            virtual void append(const std::string &str) override {
                value += str;
            }

            std::string getValue() const {
                return value;
            }
        };

        class FileAppender : public Appender {
        private:
            File &file;

        public:
            FileAppender(File &file)
                    : file(file) {}

            virtual void append(const std::string &str) override {
                file.write(&str[0], 1, str.length());
            }
        };

        Writer::Writer(bool pretty)
                : pretty(pretty) {}

        std::string Writer::writeToString(const Value &value) {
            StringAppender appender;
            write(appender, value, 0);
            return appender.getValue();
        }

        void Writer::writeToFile(const std::string &path, const Value &value) {
            File file(path, File::Mode::Text, File::Access::Write);
            FileAppender appender(file);
            write(appender, value, 0);
        }

        void Writer::write(Appender &appender, const Value &value, Size indent) {
            switch (value.getType()) {
                case Value::Type::Null:
                    writeNull(appender);
                    break;
                case Value::Type::Boolean:
                    writeBoolean(appender, value.asBoolean());
                    break;
                case Value::Type::Number:
                    writeNumber(appender, value.asDouble());
                    break;
                case Value::Type::String:
                    writeString(appender, value.asString());
                    break;
                case Value::Type::Array:
                    writeArray(appender, value, indent);
                    break;
                case Value::Type::Object:
                    writeObject(appender, value, indent);
                    break;
            }
        }

        void Writer::writeNull(Appender &appender) {
            appender.append("null");
        }

        void Writer::writeBoolean(Appender &appender, bool value) {
            appender.append(value ? "true" : "false");
        }

        void Writer::writeNumber(Appender &appender, Float64 value) {
            appender.append(std::floor(value) == value ? std::to_string((Int32) value) : std::to_string(value));
        }

        void Writer::writeString(Appender &appender, const std::string &value) {
            appender.append('"' + value + '"');
        }

        void Writer::writeArray(Appender &appender, const Value &value, Size indent) {
            appender.append("[" + lineBreak());
            for (Size i = 0; i < value.getLength(); i++) {
                appender.append(space(indent + 1));
                write(appender, value.get(i), indent + 1);
                if (i + 1 != value.getLength()) {
                    appender.append(",");
                }
                appender.append(lineBreak());
            }
            appender.append(space(indent) + "]");
        }

        void Writer::writeObject(Appender &appender, const Value &value, Size indent) {
            appender.append("{" + lineBreak());
            auto propNames = value.getPropertyNames();
            for (Size i = 0; i < propNames.size(); i++) {
                const std::string &name = propNames[i];
                appender.append(space(indent + 1) + '"' + name + "\": ");
                write(appender, value.get(name), indent + 1);
                if (i + 1 != propNames.size()) {
                    appender.append(",");
                }
                appender.append(lineBreak());
            }
            appender.append(space(indent) + "}");
        }

        std::string Writer::space(Size indent) {
            return pretty ? std::string(2 * indent, ' ') : std::string();
        }

        std::string Writer::lineBreak() {
            return pretty ? std::string("\n") : std::string();
        }
    }
}