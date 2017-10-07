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

#ifndef DUEL6_MATERIAL_H
#define DUEL6_MATERIAL_H

#include "Color.h"
#include "Texture.h"

namespace Duel6 {
    struct Material {
    private:
        Texture texture;
        Color color;
        bool masked;

    public:
        explicit Material(const Texture &texture, const Color &color = Color::WHITE, bool masked = false)
                : texture(texture), color(color), masked(masked) {}

        Material &operator=(const Material &m) = default;

        const Texture &getTexture() const {
            return texture;
        }

        bool isColored() const {
            return &color != &Color::WHITE;
        }

        const Color &getColor() const {
            return color;
        }

        bool isMasked() const {
            return masked;
        }

        static Material makeTexture(const Texture &texture) {
            return Material(texture);
        }

        static Material makeColoredTexture(const Texture &texture, const Color &color) {
            return Material(texture, color);
        }

        static Material makeMaskedTexture(const Texture &texture) {
            return Material(texture, Color::WHITE, true);
        }

        static Material makeMaskedColoredTexture(const Texture &texture, const Color &color) {
            return Material(texture, color, true);
        }
    };
}

#endif