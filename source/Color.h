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

        void operator=(const Color & c){
            color[0] = c.color[0];
            color[1] = c.color[1];
            color[2] = c.color[2];
            color[3] = c.color[3];
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

        Color& add(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
            float alphaSrc = alpha / 255.0f;
            float alphaDst = getAlpha() / 255.0f;
            float a = alphaDst * (1.0f - alphaSrc);
            float A = alphaSrc + a;
            if (A > 1.0f || getAlpha() == 1) {
                A = 1.0f;
            }
            float R = ((red / 255.0f) * alphaSrc + (getRed() / 255.0f) * a) / A;
            float G = ((green / 255.0f) * alphaSrc + (getGreen() / 255.0f) * a) / A;
            float B = ((blue / 255.0f) * alphaSrc + (getBlue() / 255.0f) * a) / A;
            return set(R * 255.0f, G * 255.0f, B * 255.0f, A * 255.0f);
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

        Color scale(Float32 ratio) const;

        static Color fromString(const std::string &colStr);

        static Color random();
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
