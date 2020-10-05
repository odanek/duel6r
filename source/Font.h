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

#ifndef DUEL6_FONT_H
#define DUEL6_FONT_H

#include <vector>

#include <SDL2/SDL_ttf.h>
#include "Type.h"
#include "Color.h"
#include "Format.h"
#include "renderer/RendererTypes.h"
#include "renderer/Renderer.h"
#include "FontCache.h"

namespace Duel6 {
    class Console;

    class Font {
    private:
        TTF_Font *font;
        Renderer &renderer;
        FontCache fontCache;
        FontCache fontCacheOutlined;
        Int32 charHeight;
    public:
        Font(Renderer &renderer);

        ~Font();

        void load(const std::string &fontFile, Console &console);

        void print(Int32 x, Int32 y, const Color &color, const std::string &str, bool outline = false) const;

        void printWrapped(Int32 x, Int32 y, const Color &color, const std::string &str, Uint32 maxWidth, Vector &resultSize, bool outline = false) const;

        void print(Float32 x, Float32 y, Float32 z, const Color &color, const std::string &str, Float32 fontHeight, bool outline = false) const;

        void printWrapped(Float32 x, Float32 y, Float32 z, const Color &color, const std::string &str, Float32 fontHeight, Uint32 , Vector &resultSize, bool outline = false) const;

        Float32 getTextWidth(const std::string &str, Float32 height) const;

        Int32 getTextWidth(const std::string &str, Int32 height) const;

        Int32 getCharWidth() const {
            return 8;
        }

        Int32 getCharHeight() const {
            return charHeight;
        }

    private:
        Texture getTexture(const std::string &text, bool outline) const;

        Texture getTexture(const std::string &text, bool outline, Uint32 maxWidth, Vector &resultSize) const;

        Texture renderText(const std::string &text, bool outline) const;

        Texture renderTextWrapped(const std::string &text, bool outline, Uint32 maxWidth, Vector &resultSize) const;
    };
}

#endif
