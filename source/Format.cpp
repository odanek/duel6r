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

#include "Format.h"
#include "FormatException.h"

namespace Duel6 {
    void Format::insertParam(Size index, const std::string &value) {
        Placeholder placeholder;
        while (getPlaceholder(index, placeholder)) {
            if (!placeholder.hasWidth || value.length() >= (size_t) std::abs(placeholder.width)) // No padding
            {
                format.replace(placeholder.offset, placeholder.length, value);
            } else if (placeholder.width > 0) // Right align
            {
                std::string paddedValue =
                        std::string(placeholder.width - value.length(), placeholder.paddingCharacter) + value;
                format.replace(placeholder.offset, placeholder.length, paddedValue);
            } else // Left align
            {
                std::string paddedValue =
                        value + std::string(-placeholder.width - value.length(), placeholder.paddingCharacter);
                format.replace(placeholder.offset, placeholder.length, paddedValue);
            }
        }
    }

    bool Format::getPlaceholder(Size index, Placeholder &placeholder) {
        std::string prefix = '{' + std::to_string(index);
        size_t start = format.find(prefix + '}');

        if (start != std::string::npos) {
            placeholder.offset = start;
            placeholder.length = prefix.size() + 1;
            placeholder.hasWidth = false;
            return true;
        }

        start = format.find(prefix + ',');
        if (start == std::string::npos) {
            return false;
        }

        size_t end = format.find('}', start + prefix.length());
        if (end == std::string::npos) {
            D6_THROW(FormatException, "Unclosed parameter placeholder in: " + format);
        }

        placeholder.offset = start;
        placeholder.length = end - start + 1;
        placeholder.hasWidth = true;

        size_t widthStart = start + prefix.length() + 1;
        std::string widthSpec = format.substr(widthStart, end - widthStart);
        size_t padDelimitPos = widthSpec.find('|');
        if (padDelimitPos != std::string::npos && padDelimitPos + 2 == widthSpec.length()) {
            placeholder.paddingCharacter = format[end - 1];
            widthSpec = widthSpec.substr(0, padDelimitPos);
        } else {
            placeholder.paddingCharacter = ' ';
        }
        placeholder.width = std::stoi(widthSpec);
        return true;
    }
}