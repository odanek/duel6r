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

#ifndef DUEL6_BONUS_H
#define DUEL6_BONUS_H

#include <SDL/SDL_opengl.h>
#include "Type.h"

#define D6_BONUS_COUNT      10

namespace Duel6
{
	struct Weapon; // Forward declaration, TODO: Remove

	extern int d6BonusArt[D6_BONUS_COUNT];

	enum BonusType
	{
		D6_BONUS_LIFEP = 19,
		D6_BONUS_LIFEM,
		D6_BONUS_LIFEF,
		D6_BONUS_SHOTS,
		D6_BONUS_SHOTP,
		D6_BONUS_INVUL,
		D6_BONUS_BULLT,
		D6_BONUS_GUESS,
		D6_BONUS_SPEED = 46,
		D6_BONUS_INVIS = 53
	};

	class Bonus
	{
	private:
		Int32 x;
		Int32 y;
		Size type;
		bool weapon;
		const Weapon* weaponType;
		Int32 bullets; // Number of bullets for weapon bonuses

	public:
		Bonus(Int32 x, Int32 y, Size type)
			: x(x), y(y), type(type), weapon(false), weaponType(nullptr), bullets(0)
		{}

		Bonus(Int32 x, Int32 y, const Weapon& weaponType, Size bullets)
			: x(x), y(y), type(0), weapon(true), weaponType(&weaponType), bullets(bullets)
		{}

		Size getType() const
		{
			return type;
		}

		bool isWeapon() const
		{
			return weapon;
		}

		const Weapon& getWeaponType() const
		{
			return *weaponType;
		}

		Int32 getBullets() const
		{
			return bullets;
		}

		Int32 getX() const
		{
			return x;
		}

		Int32 getY() const
		{
			return y;
		}

		Float32 getScreenX() const;
		Float32 getScreenY() const;
		void render() const;

	private:
		GLuint getTexture() const;
	};
}

#endif