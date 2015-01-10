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

#include "Defines.h"
#include "Water.h"

namespace Duel6
{
	namespace
	{
		Int16 wtAnim[24] = { 0, 5, 1, 5, 2, 5, 3, 5, 4, 5, 5, 5, 6, 5, 7, 5, 8, 5, 9, 5, -1, 0 };
	}

	class WaterBase : public Water
	{
	private:
		Sound::Sample splashSample;
		TextureManager::Texture textures;

	public:
		WaterBase(Sound& sound, TextureManager& textureManager, const std::string& sample, const Color& color)
		{
			splashSample = sound.loadSample(sample);

			TextureManager::SubstitutionTable subst;
			subst[Color(0, 182, 255)] = color;
			textures = textureManager.load(D6_TEXTURE_WATER_PATH, GL_NEAREST, true, subst);
		}

		Sound::Sample getSplashSound() const override
		{
			return splashSample;
		}

		void addSplash(SpriteList& spriteList, Float32 x, Float32 y) const override
		{
			Sprite waterSplash(wtAnim, textures);
			waterSplash.setPosition(x, y, 0.5f).setLooping(AnimationLooping::OnceAndRemove);
			spriteList.addSprite(waterSplash);
		}
	};

	class BlueWater : public WaterBase
	{
	public:
		BlueWater(Sound& sound, TextureManager& textureManager)
			: WaterBase(sound, textureManager, D6_FILE_WATER_BLUE, Color(0, 182, 255))
		{}

		Float32 getAirHit() const override
		{
			return 60.0f;
		}
	};

	class RedWater : public WaterBase
	{
	public:
		RedWater(Sound& sound, TextureManager& textureManager)
			: WaterBase(sound, textureManager, D6_FILE_WATER_RED, Color(197, 0, 0))
		{}

		Float32 getAirHit() const override
		{
			return 120.0f;
		}
	};

	class GreenWater : public WaterBase
	{
	public:
		GreenWater(Sound& sound, TextureManager& textureManager)
			: WaterBase(sound, textureManager, D6_FILE_WATER_GREEN, Color(0, 197, 0))
		{}

		Float32 getAirHit() const override
		{
			return 180.0f;
		}
	};

	Water::WaterSet Water::createWaterSet(Sound& sound, TextureManager& textureManager)
	{
		WaterSet set;
		set.insert(std::make_pair(Type::Blue, std::make_unique<BlueWater>(sound, textureManager)));
		set.insert(std::make_pair(Type::Red, std::make_unique<RedWater>(sound, textureManager)));
		set.insert(std::make_pair(Type::Green, std::make_unique<GreenWater>(sound, textureManager)));
		return set;
	}
}