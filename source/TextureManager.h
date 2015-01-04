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

#include <unordered_map>
#include <string>
#include <SDL2/SDL_opengl.h>
#include "Type.h"
#include "Color.h"
#include "Util.h"

#define D6_TEXTURE_EXTENSION	".tga"

#define D6_TEXTURE_MAN_PATH     "textures/man/"
#define D6_TEXTURE_MAN_KEY		"man"
#define D6_TEXTURE_BCG_PATH		"textures/backgrounds/"
#define D6_TEXTURE_BCG_KEY		"bcg"
#define D6_TEXTURE_EXPL_PATH	"textures/explosion/"
#define D6_TEXTURE_EXPL_KEY		"expl"
#define D6_TEXTURE_MENU_PATH	"textures/menu/"
#define D6_TEXTURE_MENU_KEY		"menu"
#define D6_TEXTURE_BLOCK_PATH	"textures/blocks/"
#define D6_TEXTURE_BLOCK_KEY	"block"
#define D6_TEXTURE_WATER_PATH	"textures/water/"
#define D6_TEXTURE_WATER_B_KEY	"water_blue"
#define D6_TEXTURE_WATER_R_KEY	"water_red"
#define D6_TEXTURE_WATER_G_KEY	"water_green"
#define D6_TEXTURE_ELEVATOR_PATH "textures/elevator/"
#define D6_TEXTURE_ELEVATOR_KEY	"elev"
#define D6_TEXTURE_BONUS_PATH	"textures/bonus/"
#define D6_TEXTURE_BONUS_KEY	"bonus"
#define D6_TEXTURE_FIRE_PATH	"textures/fire/"

#define D6_TEXTURE_WPN_PATH		"textures/weapon/"

namespace Duel6
{
	class TextureManager
	{
	public:
		typedef std::vector<GLuint> TextureList;
		typedef std::unordered_map<Color, Color, ColorHash> SubstitutionTable;

	private:
		std::string textureFileExtension;
		std::unordered_map<std::string, TextureList> textureMap;

	public:
		TextureManager(const std::string& fileExtension)
			: textureFileExtension(fileExtension)
		{}

		~TextureManager()
		{
			freeAll();
		}

		void freeAll();

		const TextureList& load(const std::string& key, const std::string& path, GLint filtering, bool clamp);
		const TextureList& load(const std::string& key, const std::string& path, GLint filtering, bool clamp, const SubstitutionTable& substitutionTable);

		const TextureList& get(const std::string& key) const
		{
			return textureMap.at(key);
		}

		TextureManager& remove(const std::string& key)
		{
			textureMap.erase(key);
			return *this;
		}

	private:
		void freeTextureList(const TextureList& list);
		void substituteColors(Image& image, const SubstitutionTable& substitutionTable);
	};
}

#endif