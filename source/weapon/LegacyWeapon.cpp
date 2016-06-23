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

#include <unordered_set>
#include "../World.h"
#include "LegacyWeapon.h"
#include "LegacyShot.h"
#include "impl/ShitThrower.h"
#include "impl/Pistol.h"
#include "impl/Shotgun.h"
#include "impl/Uzi.h"
#include "impl/Bazooka.h"
#include "impl/Plasma.h"
#include "impl/Laser.h"
#include "impl/Lightning.h"

namespace Duel6
{
	namespace
	{
		const LegacyWeapon::Definition LEGACY_WEAPON[] =
		{
			{ 9.15f, true, false, false, Color(0, 0, 0), 0, 30, 0.98f, "pistol", "pistol.wav", "", 0, { 1, 5, 2, 5, 3, 5, 4, 5, 5, 5, 6, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
			{ 6.1f, false, true, true, Color(255, 0, 0), 3, 100, 3.28f, "bazooka", "bazooka.wav", "bmbazook.wav", 0.01f, { 1, 5, 2, 5, 3, 5, 4, 5, 5, 5, 6, 5, 0, 50, -1, 0 }, { 0, 10, 1, 10, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
			{ 12.2f, false, true, false, Color(0, 255, 255), 0, 100, 4.1f, "lightning", "blesk.wav", "bmblesk.wav", 0, { 1, 5, 2, 5, 3, 5, 4, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 10, 1, 10, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
			{ 9.15f, true, false, false, Color(0, 0, 0), 0, 60, 2.29f, "shotgun", "shotgun.wav", "", 0, { 1, 5, 2, 5, 3, 5, 2, 5, 1, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
			{ 12.2f, true, false, false, Color(0, 0, 0), 0, 25, 0.33f, "plasma", "plasma.wav", "", 0, { 1, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
			{ 15.25f, true, false, false, Color(0, 0, 0), 0, 35, 0.25f, "laser", "laser.wav", "", 0, { 1, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
			{ 9.15f, true, false, false, Color(0, 0, 0), 0, 40, 0.66f, "machine gun", "kulomet.wav", "", 0, { 1, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
			{ 6.1f, false, true, true, Color(255, 255, 0), 4, 200, 6.56f, "triton", "triton.wav", "bmbazook.wav", 0.04f, { 1, 5, 2, 5, 3, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 8, 1, 8, 2, 8, 3, 8, 4, 8, 3, 8, 2, 8, 1, 8, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
			{ 10.98f, true, false, false, Color(0, 0, 0), 0, 15, 0.26f, "uzi", "pistol.wav", "", 0, { 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
			{ 12.2f, true, false, false, Color(0, 0, 0), 0, 25, 0.82f, "bow", "luk.wav", "", 0, { 1, 10, 2, 10, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
			{ 7.93f, true, false, false, Color(0, 0, 0), 0, 50, 1.31f, "slime", "sliz.wav", "", 0, { 1, 10, 2, 10, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
			{ 12.2f, true, true, true, Color(255, 255, 0), 2, 80, 3.44f, "double laser", "laser.wav", "bmbazook.wav", 0.005f, { 1, 10, 0, 10, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
			{ 4.27f, false, true, true, Color(255, 0, 255), 0, 100, 3.11f, "kiss of death", "kiss2.wav", "kiss.wav", 0.01f, { 0, 8, 1, 8, 2, 8, 1, 8, 0, 8, 0, 8, 0, 8, -1, 0 }, { 0, 10, 1, 10, 2, 10, 1, 10, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
			{ 4.88f, false, false, false, Color(0, 0, 0), 0, 20, 0.66f, "spray", "spray.wav", "", 0, { 0, 8, 1, 8, 0, 8, 1, 8, 0, 8, 0, 8, 0, 8, -1, 0 }, { 0, 5, 1, 5, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
			{ 6.71f, true, false, false, Color(0, 0, 0), 0, 15, 0.82f, "sling", "prak.wav", "", 0, { 0, 5, 1, 5, 0, 5, 2, 5, 0, 5, 0, 5, 0, 5, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
			{ 7.93f, true, false, false, Color(0, 0, 0), 0, 10, 0.9f, "stopper gun", "spunt.wav", "", 0, { 0, 5, 1, 5, 1, 5, 2, 5, 2, 5, 0, 5, 0, 5, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
			{ 5.49f, false, false, true, Color(0, 0, 0), 2, 0, 1.97f, "shit thrower", "shit.wav", "shit-hit.wav", 0.04f, { 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, -1, 0 }, { 0, 10, 1, 10, 2, 10, 1, 10, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
		};

		const std::unordered_set<const LegacyWeapon::Definition*> NEAREST_FILTER_BOOM = {
			&LEGACY_WEAPON[5], &LEGACY_WEAPON[6], &LEGACY_WEAPON[9], &LEGACY_WEAPON[13],
			&LEGACY_WEAPON[14], &LEGACY_WEAPON[15], &LEGACY_WEAPON[16]
		};

		const Rectangle SHOT_COLLISION_RECT = Rectangle::fromCornerAndSize(Vector(0.0f, 0.65f), Vector(0.65f, 0.35f));
	}

	LegacyWeapon::LegacyWeapon(Sound& sound, TextureManager& textureManager, const Definition& definition, Size index)
		: WeaponBase(definition.name, definition.reloadSpeed), definition(definition)
	{
		const std::string wpnPath = Format("{0}{1,3|0}") << D6_TEXTURE_WPN_PATH << index;
		auto filterType = NEAREST_FILTER_BOOM.find(&definition) != NEAREST_FILTER_BOOM.end() ? TextureFilter::NEAREST : TextureFilter::LINEAR;
		textures.boom = textureManager.load(Format("{0}/boom/") << wpnPath, filterType, true);
		textures.gun = textureManager.load(Format("{0}/gun/") << wpnPath, TextureFilter::NEAREST, true);
		textures.shot = textureManager.load(Format("{0}/shot/") << wpnPath, TextureFilter::NEAREST, true);

		if (!definition.shotSound.empty())
		{
			samples.shot = sound.loadSample(std::string(D6_FILE_WEAPON_SOUNDS) + definition.shotSound);
		}
		if (!definition.boomSound.empty())
		{
			samples.boom = sound.loadSample(std::string(D6_FILE_WEAPON_SOUNDS) + definition.boomSound);
		}
	}

	void LegacyWeapon::shoot(Player& player, Orientation orientation, World& world) const
	{
		Sprite shotSprite(definition.shotAnimation, textures.shot);
		auto spriteIterator = world.getSpriteList().addSprite(shotSprite);
		world.getShotList().addShot(makeShot(player, orientation, spriteIterator));
		samples.shot.play();
	}

	std::unique_ptr<Shot> LegacyWeapon::makeShot(Player& player, Orientation orientation, SpriteList::Iterator spriteIterator) const
	{
		return std::make_unique<LegacyShot>(player, *this, orientation, spriteIterator, getShotCollisionRectangle());
	}

	Rectangle LegacyWeapon::getShotCollisionRectangle() const
	{
		return SHOT_COLLISION_RECT;
	}

	Sprite& LegacyWeapon::makeSprite(Sprite& sprite) const
	{
		return sprite.setAnimation(definition.animation).setTextures(textures.gun).setFrame(6).setLooping(AnimationLooping::OnceAndStop);
	}

	Texture LegacyWeapon::getBonusTexture() const
	{
		return textures.gun.at(definition.animation[12]);
	}

	const LegacyWeapon::Definition& LegacyWeapon::getDefinition() const
	{
		return definition;
	}

	const LegacyWeapon::WeaponTextures& LegacyWeapon::getTextures() const
	{
		return textures;
	}

	const LegacyWeapon::WeaponSamples& LegacyWeapon::getSamples() const
	{
		return samples;
	}

	std::unique_ptr<WeaponImpl> LegacyWeapon::create(Sound& sound, TextureManager& textureManager, Size index)
	{
		const Definition& definition = LEGACY_WEAPON[index];
		if (index == 0)
		{
			return std::make_unique<Pistol>(sound, textureManager);
		}
		if (index == 1)
		{
			return std::make_unique<Bazooka>(sound, textureManager);
		}
		if (index == 2)
		{
			return std::make_unique<Lightning>(sound, textureManager);
		}
		if (index == 3)
		{
			return std::make_unique<Shotgun>(sound, textureManager);
		}
		if (index == 4)
		{
			return std::make_unique<Plasma>(sound, textureManager);
		}
		if (index == 5)
		{
			return std::make_unique<Laser>(sound, textureManager);
		}
		if (index == 8)
		{
			return std::make_unique<Uzi>(sound, textureManager);
		}
		if (index == 16)
		{
			return std::make_unique<ShitThrower>(sound, textureManager);
		}
		return std::make_unique<LegacyWeapon>(sound, textureManager, definition, index);
	}
}