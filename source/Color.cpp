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

#include "math/Math.h"
#include "Color.h"

namespace Duel6 {
    namespace {
        Uint8 hexToNum(char hex) {
            return hex >= 'a' ? Uint8(hex - 'a' + 10) : Uint8(hex - '0');
        }

        Uint8 hexToNum(const std::string &hexStr) {
            return hexToNum(hexStr[0]) * Uint8(16) + hexToNum(hexStr[1]);
        }
    }

    const Color Color::BLACK(0, 0, 0);
    const Color Color::WHITE(255, 255, 255);
    const Color Color::RED(255, 0, 0);
    const Color Color::GREEN(0, 255, 0);
    const Color Color::BLUE(0, 0, 255);
    const Color Color::CYAN(0, 255, 255);
    const Color Color::MAGENTA(255, 0, 255);
    const Color Color::YELLOW(255, 255, 0);

    Color Color::scale(Float32 ratio) const {
        Uint8 red = std::min(Uint8(255), Uint8(color[0] * ratio));
        Uint8 green = std::min(Uint8(255), Uint8(color[1] * ratio));
        Uint8 blue = std::min(Uint8(255), Uint8(color[2] * ratio));
        return Color(red, green, blue, color[3]);
    }

    Color Color::fromString(const std::string &colStr) {
        Uint8 red = hexToNum(colStr.substr(0, 2));
        Uint8 green = hexToNum(colStr.substr(2, 2));
        Uint8 blue = hexToNum(colStr.substr(4, 2));
        return Color(red, green, blue);
    }

    Color Color::random() {
        return Color(Uint8(Math::random(0, 255)), Uint8(Math::random(0, 255)), Uint8(Math::random(0, 255)));
    }
}