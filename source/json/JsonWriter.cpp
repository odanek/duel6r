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

#include <c++/cmath>
#include "JsonWriter.h"

namespace Duel6
{
	namespace Json
	{
		class TextWriter
		{
		public:
			virtual void write(const std::string& str) = 0;
		};

		class StringWriter : public TextWriter
		{
		private:
			std::string value;

		public:
			virtual void write(const std::string& str) override
			{
				value += str;
			}

			std::string getValue() const
			{
				return value;
			}
		};

		class FileWriter : public TextWriter
		{
		private:
			File& file;

		public:
			FileWriter(File& file)
				: file(file)
			{}

			virtual void write(const std::string& str) override
			{
				file.write(&str[0], 1, str.length());
			}
		};

		Writer::Writer(bool pretty)
			: pretty(pretty)
		{}

		std::string Writer::writeToString(const Value& value)
		{
			StringWriter writer;
			write(writer, value, 0);
			return writer.getValue();
		}

		void Writer::writeToFile(const std::string& path, const Value& value)
		{
			File file(path, "wt");
			FileWriter writer(file);
			write(writer, value, 0);
		}

		void Writer::write(TextWriter& writer, const Value& value, Size indent)
		{
			switch (value.getType())
			{
				case Value::Type::Null:
					writeNull(writer, indent);
					break;
				case Value::Type::Boolean:
					writeBoolean(writer, value.asBoolean(), indent);
					break;
				case Value::Type::Number:
					writeNumber(writer, value.asDouble(), indent);
					break;
				case Value::Type::String:
					writeString(writer, value.asString(), indent);
					break;
				case Value::Type::Array:
					writeArray(writer, value, indent);
					break;
				case Value::Type::Object:
					writeObject(writer, value, indent);
					break;
			}
		}

		void Writer::writeNull(TextWriter& writer, Size indent)
		{
			writer.write("null");
		}

		void Writer::writeBoolean(TextWriter& writer, bool value, Size indent)
		{
			writer.write(value ? "true" : "false");
		}

		void Writer::writeNumber(TextWriter& writer, Float64 value, Size indent)
		{
			writer.write(std::floor(value) == value ? std::to_string((Int32)value) : std::to_string(value));
		}

		void Writer::writeString(TextWriter& writer, const std::string& value, Size indent)
		{
			writer.write('"' + value + '"');
		}

		void Writer::writeArray(TextWriter& writer, const Value& value, Size indent)
		{
			writer.write("[ ");
			for (Size i = 0; i < value.getLength(); i++)
			{
				write(writer, value.get(i), indent);
				if (i + 1 != value.getLength())
				{
					writer.write(", ");
				}
			}
			writer.write(" ]");
		}

		void Writer::writeObject(TextWriter& writer, const Value& value, Size indent)
		{
			writer.write("{ ");
			auto propNames = value.getPropertyNames();
			for (Size i = 0; i < propNames.size(); i++)
			{
				const std::string& name = propNames[i];
				writer.write('"' + name + "\": ");
				write(writer, value.get(name), indent);
				if (i + 1 != propNames.size())
				{
					writer.write(", ");
				}
			}
			writer.write(" }");
		}
	}
}