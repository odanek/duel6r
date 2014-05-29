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

namespace Duel6
{
	class TextureManager
	{
	public:
		typedef std::vector<GLuint> TextureList;
		typedef std::unordered_map<Color, Color, ColorHash> SubstitutionTable;

	private:
		std::unordered_map<std::string, TextureList> textureMap;

	public:
		~TextureManager()
		{
			freeAll();
		}

		void freeAll();

		void load(const std::string& key, const std::string& path, GLint filtering);
		void load(const std::string& key, const std::string& path, GLint filtering, const SubstitutionTable& substitutionTable);

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
		bool nameEndsWith(const std::string& name, const std::string& suffix) const;
	};
}

#endif