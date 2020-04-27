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
            : font(nullptr), renderer(renderer),
              fontCache(renderer, 100), fontCacheOutlined(renderer, 100) {
        charHeight = 16;
    }

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
        print(Float32(x), Float32(y), 0.0f, color, str, 16);
    }

    void Font::printWrapped(Int32 x, Int32 y, const Color &color, const std::string &str, Uint32 maxWidth, Vector &resultSize) const {
        printWrapped(Float32(x), Float32(y), 0.0f, color, str, 16, maxWidth, resultSize);
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

    void Font::printWrapped(Float32 x, Float32 y, Float32 z,
                            const Color &color, const std::string &str,
                            Float32 fontHeight, Uint32 maxWidth, Vector &resultSize) const {
        if (str.length() < 1) {
            return;
        }
        Float32 scale = fontHeight / getCharHeight();
        Texture texture = getTexture(str, maxWidth, resultSize);
        resultSize.x *= scale;
        resultSize.y *= scale;
        Material material = Material::makeColoredTexture(texture, color);

        Float32 width = resultSize.x;
        Float32 height = resultSize.y;
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

    Texture Font::getTexture(const std::string &text, Uint32 maxWidth, Vector &resultSize) const{
        if (fontCache.has(text)) {
            return fontCache.get(text, resultSize.x, resultSize.y);
        }
       Texture texture = renderTextWrapped(text, maxWidth, resultSize);
       fontCache.add(text, texture, resultSize.x, resultSize.y);
       return texture;
    }

    Texture Font::renderText(const std::string &text) const {
        TTF_SetFontOutline(font, 2);
        SDL_Surface *outline_surface = TTF_RenderText_Blended(font, text.c_str(), SDL_Color{1, 1, 1, 255});
        TTF_SetFontOutline(font, 0);
        SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), SDL_Color{255, 255, 255, 0});
        SDL_Rect rect = {0, 0 , surface->w, surface->h};
        SDL_Rect rect2 = {1, 1, outline_surface->w - 2, outline_surface->h - 2};
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
        SDL_BlitScaled(surface, &rect, outline_surface, &rect2);

        SDL_FreeSurface(surface);
        Image image = Image::fromSurface(outline_surface);
        SDL_FreeSurface(outline_surface);

        Texture texture = renderer.createTexture(image, TextureFilter::Linear, true);

        return texture;
    }

    Texture Font::renderTextWrapped(const std::string &text, Uint32 maxWidth, Vector &resultSize) const {
        TTF_SetFontOutline(font, 2);
        SDL_Surface *outline_surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), SDL_Color{1, 1, 1, 255}, maxWidth);
        TTF_SetFontOutline(font, 0);
        SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), SDL_Color{255, 255, 255, 0}, maxWidth);
        SDL_Rect rect = {0, 0 , surface->w  , surface->h};
        SDL_Rect rect2 = {2, 2, outline_surface->w, outline_surface->h};

        /* blit text onto its outline */
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
        SDL_BlitScaled(surface, &rect, outline_surface, &rect2);

        SDL_FreeSurface(surface);

        Image image = Image::fromSurface(outline_surface);
        resultSize.set(image.getWidth(), image.getHeight(), image.getDepth());

        SDL_FreeSurface(outline_surface);
        Texture texture = renderer.createTexture(image, TextureFilter::Linear, true);

        return texture;
    }
}
