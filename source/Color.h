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

#ifndef DUEL6_COLOR_H
#define DUEL6_COLOR_H

#include <string>
#include <functional>
#include "Type.h"

namespace Duel6 {
    class Color {
    private:
        Uint8 color[4];

    public:
        static const Color BLACK;
        static const Color WHITE;
        static const Color RED;
        static const Color GREEN;
        static const Color BLUE;
        static const Color CYAN;
        static const Color MAGENTA;
        static const Color YELLOW;

    public:
        Color()
                : Color(0, 0, 0, 255) {}

        explicit Color(Uint8 value)
                : Color(value, value, value, 255) {}

        Color(Uint8 red, Uint8 green, Uint8 blue)
                : Color(red, green, blue, 255) {}

        Color(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
            set(red, green, blue, alpha);
        }

        Color(const Color &color) {
            *this = color;
        }

        Color &operator=(const Color &color) {
            if (this != &color) {
                setRed(color.getRed());
                setGreen(color.getGreen());
                setBlue(color.getBlue());
                setAlpha(color.getAlpha());
            }

            return *this;
        }

        bool operator==(const Color &color) const {
            return (getRed() == color.getRed() &&
                    getGreen() == color.getGreen() &&
                    getBlue() == color.getBlue() &&
                    getAlpha() == color.getAlpha());
        }

        bool operator!=(const Color &color) const {
            return (getRed() != color.getRed() ||
                    getGreen() != color.getGreen() ||
                    getBlue() != color.getBlue() ||
                    getAlpha() != color.getAlpha());
        }

        Uint8 getRed() const {
            return color[0];
        }

        Uint8 getGreen() const {
            return color[1];
        }

        Uint8 getBlue() const {
            return color[2];
        }

        Uint8 getAlpha() const {
            return color[3];
        }

        Uint8 get(Size index) const {
            return color[index];
        }

        Color &set(Uint8 red, Uint8 green, Uint8 blue) {
            setRed(red);
            setGreen(green);
            setBlue(blue);
            return *this;
        }

        Color &set(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
            setRed(red);
            setGreen(green);
            setBlue(blue);
            setAlpha(alpha);
            return *this;
        }

        Color &setRed(Uint8 value) {
            color[0] = value;
            return *this;
        }

        Color &setGreen(Uint8 value) {
            color[1] = value;
            return *this;
        }

        Color &setBlue(Uint8 value) {
            color[2] = value;
            return *this;
        }

        Color &setAlpha(Uint8 value) {
            color[3] = value;
            return *this;
        }

        Color withAlpha(Uint8 value) const {
            return Color(color[0], color[1], color[2], value);
        }

        Color &set(Size index, Uint8 value) {
            color[index] = value;
            return *this;
        }

        static Color fromString(const std::string &colStr) {
            return Color(hexToNum(colStr.substr(0, 2)), hexToNum(colStr.substr(2, 2)), hexToNum(colStr.substr(4, 2)));
        }

    private:
        static Int32 hexToNum(const std::string &hexStr) {
            return hexToNum(hexStr[0]) * 16 + hexToNum(hexStr[1]);
        }

        static Int32 hexToNum(char hex) {
            return hex >= 'a' ? hex - 'a' + 10 : hex - '0';
        }
    };

    class ColorHash {
    public:
        size_t operator()(const Color &color) const {
            std::hash<Uint32> hashFunction;
            return hashFunction(
                    (color.getAlpha() << 24) + (color.getRed() << 16) + (color.getGreen() << 8) + color.getBlue());
        }
    };
}

#endif