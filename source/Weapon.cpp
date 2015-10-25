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

#include <stdlib.h>
#include <set>
#include "Sound.h"
#include "Weapon.h"

namespace Duel6
{
	Weapon d6WpnDef[D6_WEAPONS] =
	{
		{ 0, true, 9.15f, true, false, Color(0, 0, 0), 0, 30, 0.98f, "pistol", "pistol.wav", "", 0, false, { 1, 5, 2, 5, 3, 5, 4, 5, 5, 5, 6, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 1, true, 6.1f, false, true, Color(255, 0, 0), 3, 100, 3.28f, "bazooka", "bazooka.wav", "bmbazook.wav", 0.01f, false, { 1, 5, 2, 5, 3, 5, 4, 5, 5, 5, 6, 5, 0, 50, -1, 0 }, { 0, 10, 1, 10, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 2, true, 12.2f, false, true, Color(0, 255, 255), 0, 100, 4.1f, "lightning gun", "blesk.wav", "bmblesk.wav", 0, false, { 1, 5, 2, 5, 3, 5, 4, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 10, 1, 10, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 3, true, 9.15f, true, false, Color(0, 0, 0), 0, 60, 2.29f, "shotgun", "shotgun.wav", "", 0, false, { 1, 5, 2, 5, 3, 5, 2, 5, 1, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 4, true, 12.2f, true, false, Color(0, 0, 0), 0, 25, 0.33f, "plasma gun", "plasma.wav", "", 0, false, { 1, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 5, true, 15.25f, true, false, Color(0, 0, 0), 0, 35, 0.25f, "laser", "laser.wav", "", 0, false, { 1, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ 6, true, 9.15f, true, false, Color(0, 0, 0), 0, 40, 0.66f, "machine gun", "kulomet.wav", "", 0, false, { 1, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ 7, true, 6.1f, false, true, Color(255, 255, 0), 4, 200, 6.56f, "triton", "triton.wav", "bmbazook.wav", 0.04f, false, { 1, 5, 2, 5, 3, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 8, 1, 8, 2, 8, 3, 8, 4, 8, 3, 8, 2, 8, 1, 8, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 8, true, 10.98f, true, false, Color(0, 0, 0), 0, 15, 0.26f, "uzi", "pistol.wav", "", 0, false, { 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 9, true, 12.2f, true, false, Color(0, 0, 0), 0, 25, 0.82f, "bow", "luk.wav", "", 0, false, { 1, 10, 2, 10, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ 10, true, 7.93f, true, false, Color(0, 0, 0), 0, 50, 1.31f, "slime gun", "sliz.wav", "", 0, false, { 1, 10, 2, 10, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 11, true, 12.2f, true, true, Color(255, 255, 0), 2, 80, 3.44f, "double laser", "laser.wav", "bmbazook.wav", 0.005f, false, { 1, 10, 0, 10, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 12, false, 4.27f, false, true, Color(255, 0, 255), 0, 100, 3.11f, "kiss of death", "kiss2.wav", "kiss.wav", 0.01f, false, { 0, 8, 1, 8, 2, 8, 1, 8, 0, 8, 0, 8, 0, 8, -1, 0 }, { 0, 10, 1, 10, 2, 10, 1, 10, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 13, false, 4.88f, false, false, Color(0, 0, 0), 0, 20, 0.66f, "spray", "spray.wav", "", 0, false, { 0, 8, 1, 8, 0, 8, 1, 8, 0, 8, 0, 8, 0, 8, -1, 0 }, { 0, 5, 1, 5, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ 14, false, 6.71f, true, false, Color(0, 0, 0), 0, 15, 0.82f, "sling", "prak.wav", "", 0, false, { 0, 5, 1, 5, 0, 5, 2, 5, 0, 5, 0, 5, 0, 5, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ 15, false, 7.93f, true, false, Color(0, 0, 0), 0, 10, 0.9f, "stopper gun", "spunt.wav", "", 0, false, { 0, 5, 1, 5, 1, 5, 2, 5, 2, 5, 0, 5, 0, 5, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ 16, false, 5.49f, false, false, Color(0, 0, 0), 2, 0, 1.97f, "shit thrower", "shit.wav", "shit-hit.wav", 0.04f, true, { 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, -1, 0 }, { 0, 10, 1, 10, 2, 10, 1, 10, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
	};

	std::unique_ptr<PlayerSkin> d6BrownSkin;

	void WPN_Init(TextureManager& textureManager, Sound& sound, Console& console)
	{
		std::set<Int32> nearestFilterBoom = { 5, 6, 9, 13, 14, 15, 16 };

		console.printLine("\n===Weapon initialization===");
		for (Weapon& weap : d6WpnDef)
		{
			const std::string wpnPath = Format("{0}{1,3|0}") << D6_TEXTURE_WPN_PATH << weap.index;
			weap.textures.boom = textureManager.load(Format("{0}/boom/") << wpnPath, nearestFilterBoom.find(weap.index) != nearestFilterBoom.end() ? GL_NEAREST : GL_LINEAR, true);
			weap.textures.gun = textureManager.load(Format("{0}/gun/") << wpnPath, GL_NEAREST, true);
			weap.textures.shot = textureManager.load(Format("{0}/shot/") << wpnPath, GL_NEAREST, true);

			if (!weap.shotSound.empty())
			{
				weap.shotSample = sound.loadSample(std::string(D6_FILE_WEAPON_SOUNDS) + weap.shotSound);
			}
			if (!weap.boomSound.empty())
			{
				weap.boomSample = sound.loadSample(std::string(D6_FILE_WEAPON_SOUNDS) + weap.boomSound);
			}
		}

		Color brownColor(83, 44, 0);
		PlayerSkinColors skinColors(brownColor);
		d6BrownSkin = std::make_unique<PlayerSkin>("textures/man/", skinColors, textureManager);
	}

	void WPN_DeInit()
	{
		d6BrownSkin.reset();
	}

	const Weapon& WPN_GetRandomWeapon()
	{
		Size enabledWeaponCount = 0;

		for (const Weapon& weapon : d6WpnDef)
		{
			if (weapon.enabled)
			{
				enabledWeaponCount++;
			}
		}

		Size weaponIndex = rand() % enabledWeaponCount;
		for (const Weapon& weapon : d6WpnDef)
		{
			if (weapon.enabled)
			{
				if (weaponIndex-- == 0)
				{
					return weapon;
				}
			}
		}

		return d6WpnDef[0];
	}
}