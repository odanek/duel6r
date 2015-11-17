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

#include <iterator>
#include "Sound.h"
#include "Weapon.h"
#include "GameSettings.h"
#include "weapon/LegacyWeapon.h"

namespace Duel6
{
	namespace
	{
		class NoneWeapon : public WeaponImpl
		{
		public:
			std::string getName() const { return ""; }
			Float32 getReloadInterval() const { return 0.0f; }
			void shoot(Player& player, Orientation orientation, World& world) const {}
			Sprite& makeSprite(Sprite& sprite) const { return sprite; }
			Texture getBonusTexture() const { return Texture(); }
		};

		NoneWeapon NO_WEAPON;
	}

	std::vector<Weapon::WeaponImplPtr> Weapon::implementations;
	std::vector<Weapon> Weapon::weapons;

	Weapon::Weapon()
		: impl(&NO_WEAPON)
	{}

	Weapon::Weapon(WeaponImpl *impl)
		: impl(impl)
	{}

	Weapon Weapon::add(WeaponImplPtr&& impl)
	{
		Weapon weapon(impl.get());
		implementations.push_back(std::forward<WeaponImplPtr>(impl));
		weapons.push_back(weapon);
		return weapon;
	}

	std::string Weapon::getName() const
	{
		return impl->getName();
	}

	Float32 Weapon::getReloadInterval() const
	{
		return impl->getReloadInterval();
	}

	void Weapon::shoot(Player& player, Orientation orientation, World& world) const
	{
		impl->shoot(player, orientation, world);
	}

	Sprite& Weapon::makeSprite(Sprite& sprite) const
	{
		return impl->makeSprite(sprite);
	}

	Texture Weapon::getBonusTexture() const
	{
		return impl->getBonusTexture();
	}

	bool Weapon::operator==(const Weapon& weapon) const
	{
		return impl == weapon.impl;
	}

	bool Weapon::operator!=(const Weapon& weapon) const
	{
		return impl != weapon.impl;
	}

	void Weapon::initialize(Sound& sound, TextureManager& textureManager)
	{
		for (Size i = 0; i < 17; i++)
		{
			add(LegacyWeapon::create(sound, textureManager, i));
		}
	}

	const std::vector<Weapon>& Weapon::values()
	{
		return weapons;
	}

	const Weapon& Weapon::getRandomEnabled(const GameSettings& settings)
	{
		auto& enabledWeapons = settings.getEnabledWeapons();
		Size randomIndex = rand() % enabledWeapons.size();
		auto randomWeapon = enabledWeapons.cbegin();
		std::advance(randomWeapon, randomIndex);
		return *randomWeapon;
	}
}