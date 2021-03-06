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

#include <list>
#include <unordered_map>
#include "console/Console.h"
#include "Font.h"
#include "FontException.h"
#include "Video.h"

namespace Duel6 {
    Font::Font(Renderer &renderer)
            : font(nullptr), renderer(renderer), fontCache(renderer, 100) {}

    Font::~Font() {
        if (font != nullptr) {
            TTF_CloseFont(font);
            font = nullptr;
        }
    }

    void Font::load(const std::string &fontFile, Console &console) {
        console.printLine(Format("...loading font: {0}") << fontFile);
        font = TTF_OpenFont(fontFile.c_str(), 32);
        if (font == nullptr) {
            D6_THROW(FontException, Format("Unable to load font {0} due to error: {1}") << fontFile << TTF_GetError());
        }
    }

    Float32 Font::getTextWidth(const std::string &str, Float32 height) const {
        return str.size() * (height / 2.0f);
    }

    Int32 Font::getTextWidth(const std::string &str, Int32 height) const {
        return Int32(str.size() * height) / 2;
    }

    void Font::print(Int32 x, Int32 y, const Color &color, const std::string &str) const {
        print(Float32(x), Float32(y), 0.0f, color, str, Float32(getCharHeight()));
    }

    void
    Font::print(Float32 x, Float32 y, Float32 z, const Color &color, const std::string &str, Float32 height) const {
        if (str.length() < 1) {
            return;
        }

        Texture texture = getTexture(str);
        Material material = Material::makeColoredTexture(texture, color);
        Float32 width = getTextWidth(str, height);

        renderer.setBlendFunc(BlendFunc::SrcAlpha);
        renderer.quadXY(Vector(x, y, z), Vector(width, height), Vector(0, 1), Vector(1, -1), material);
        renderer.setBlendFunc(BlendFunc::None);
    }

    Texture Font::getTexture(const std::string &text) const {
        if (fontCache.has(text)) {
            return fontCache.get(text);
        }
        Texture texture = renderText(text);
        fontCache.add(text, texture);
        return texture;
    }

    Texture Font::renderText(const std::string &text) const {
        SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), SDL_Color{255, 255, 255, 255});
        Image image = Image::fromSurface(surface);
        SDL_FreeSurface(surface);

        Texture texture = renderer.createTexture(image, TextureFilter::Linear, true);

        return texture;
    }
}