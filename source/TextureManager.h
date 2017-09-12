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

#ifndef DUEL6_TEXTUREMANAGER_H
#define DUEL6_TEXTUREMANAGER_H

#include <memory>
#include <unordered_map>
#include <string>
#include "Type.h"
#include "Color.h"
#include "Image.h"
#include "Texture.h"

#define D6_TEXTURE_EXTENSION     ".tga"

#define D6_TEXTURE_MAN_PATH      "textures/man/"
#define D6_TEXTURE_BCG_PATH      "textures/backgrounds/"
#define D6_TEXTURE_EXPL_PATH     "textures/explosion/"
#define D6_TEXTURE_MENU_PATH     "textures/menu/"
#define D6_TEXTURE_BLOCK_PATH    "textures/blocks/"
#define D6_TEXTURE_WATER_PATH    "textures/water/"
#define D6_TEXTURE_ELEVATOR_PATH "textures/elevator/"
#define D6_TEXTURE_BONUS_PATH    "textures/bonus/"
#define D6_TEXTURE_FIRE_PATH     "textures/fire/"
#define D6_TEXTURE_WPN_PATH      "textures/weapon/"

namespace Duel6 {
    class TextureManager {
    public:
        typedef std::vector<Texture> TextureArray;
        typedef std::unordered_map<Color, Color, ColorHash> SubstitutionTable;

    private:
        Int32 nextId;
        std::string textureFileExtension;
        std::unordered_map<Int32, std::unique_ptr<TextureArray>> textureMap;

    public:
        explicit TextureManager(const std::string &fileExtension);

        ~TextureManager();

        Size size() { return textureMap.size(); }

        void dispose(TextureList &textures);

        void disposeAll();

        const TextureList load(const std::string &path, TextureFilter filtering, bool clamp);

        const TextureList
        load(const std::string &path, TextureFilter filtering, bool clamp, const SubstitutionTable &substitutionTable);

    private:
        void dispose(const Int32 key);

    private:
        void releaseTextureIds(const TextureArray &list);

        void substituteColors(Image &image, const SubstitutionTable &substitutionTable);
    };
}

#endif