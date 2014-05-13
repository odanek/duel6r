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

#ifndef DUEL6_WEAPON_H
#define DUEL6_WEAPON_H

#include "Type.h"
#include "Shot.h"
#include "Player.h"

#define D6_WEAPONS          17

// TODO: Split into Weapon (WeaponType) and something like ShotList
namespace Duel6
{
	class Player; // Forward declaration, TODO: Remove

	struct Weapon
	{
		bool enabled;
		Float32 bulletSpeed;
		bool Blood;
		bool explodes;
		Color explosionColor;
		Int32 Boom;
		Int32 Power;
		Float32 reloadSpeed;
		char Name[30];
		Int32 ShSound;
		Int32 BmSound;
		Float32 ExpGrow;
		bool shit; // TODO: Remove
		Int16 animation[16];
		Int16 shotAnimation[18];
		Int16 boomAnimation[14];
	};

	extern Weapon d6WpnDef[D6_WEAPONS];
	extern std::vector<GLuint> d6WpnTextures;

	void WPN_FreeTextures();
	void WPN_Init(const std::string& textureFile);
	void WPN_DeInit();
	void WPN_LevelInit();
	void WPN_AddShot(Player& player);
	void WPN_MoveShots(float elapsedTime);

	const Weapon& WPN_GetRandomWeapon();
}

#endif