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

#include "PlayerSkin.h"
#include "TextureManager.h"
#include "Globals.h"

namespace Duel6
{
	std::unordered_set<Size> PlayerSkin::skinIds;

	PlayerSkin PlayerSkin::create(const std::string& texturePath, const PlayerSkinColors& colors)
	{
		PlayerSkin skin(getNewKey(), texturePath, colors);
		return skin;
	}

	void PlayerSkin::freeAll()
	{
		for (Size id : skinIds)
		{
			d6TextureManager.remove(getKey(id));
		}
		skinIds.clear();
	}

	std::string PlayerSkin::getNewKey()
	{
		Size id = 0;
		while (skinIds.find(id) != skinIds.end())
		{
			++id;
		}

		skinIds.insert(id);
		return getKey(id);
	}

	std::string PlayerSkin::getKey(Size id)
	{
		return std::string(D6_TEXTURE_MAN_KEY) + std::to_string(id);
	}

	PlayerSkin::PlayerSkin(const std::string& key, const std::string& texturePath, const PlayerSkinColors& colors)
		: key(key)
	{
		d6Console.print(D6_L("Loading player textures\n"));

		TextureManager::SubstitutionTable substTable;
		substTable[Color(255, 255, 0)] = colors.get(PlayerSkinColors::HairTop);
		substTable[Color(222,218,0)] = colors.get(PlayerSkinColors::HairBottom);
		substTable[Color(0, 0, 172)] = colors.get(PlayerSkinColors::BodyOuter);
		substTable[Color(0, 0, 255)] = colors.get(PlayerSkinColors::BodyInner);
		substTable[Color(0, 182, 0)] = colors.get(PlayerSkinColors::HandOuter);
		substTable[Color(0, 255, 0)] = colors.get(PlayerSkinColors::HandInner);
		substTable[Color(139, 0, 0)] = colors.get(PlayerSkinColors::Trousers);
		substTable[Color(180, 182, 0)] = colors.get(PlayerSkinColors::Shoes);
		substTable[Color(255, 145, 172)] = colors.get(PlayerSkinColors::Face);

		d6TextureManager.load(key, texturePath, GL_NEAREST, substTable);
		textures = &d6TextureManager.get(key);
	}
}