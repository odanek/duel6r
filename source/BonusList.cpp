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
#include "Sound.h"
#include "BonusList.h"
#include "Weapon.h"
#include "World.h"
#include "InfoMessageQueue.h"
#include "Globals.h"

namespace Duel6
{
	static std::list<Bonus> d6Bonuses;

	void BONUS_Init()
	{
		d6Bonuses.clear();
	}

	void BONUS_DrawAll()
	{
		glEnable(GL_ALPHA_TEST);

		for (const Bonus& bonus : d6Bonuses)
		{
			bonus.render();
		}

		glDisable(GL_ALPHA_TEST);
	}

	void BONUS_AddNew(const World& world)
	{
		bool weapon = ((rand() % 2) == 1);
		Int32 x = rand() % world.getSizeX();
		Int32 y = rand() % world.getSizeY();

		if (weapon && !world.isWall(x, y - 1, true)) // Weapons must be on the ground
		{
			return;
		}

		if (!world.isWall(x, y, true))
		{
			if (weapon)
			{
				d6Bonuses.push_back(Bonus(x, y, WPN_GetRandomWeapon(), rand() % 10 + 10));
			}
			else
			{
				d6Bonuses.push_back(Bonus(x, y, rand() % D6_BONUS_COUNT));				
			}
		}
	}

	void BONUS_AddDeadManGun(Int32 x, Int32 y, Player& player)
	{
		d6Bonuses.push_back(Bonus(x, y, player.getWeapon(), player.getAmmo()));
	}

	static bool BONUS_IsApplicable(const Bonus& bonus, const Player& player, bool weapon)
	{
		if (weapon != bonus.isWeapon())
		{
			return false;
		}

		if (!weapon && (bonus.getType() == D6_BONUS_LIFEM && player.isInvulnerable()))
		{
			return false;
		}

		// TODO: Coord
		return (fabs(Float32(bonus.getX()) - player.getX()) < 0.5f && 
			    fabs(Float32(bonus.getY()) - player.getY()) < 0.5f);
	}

	static void BONUS_Apply(const Bonus& bonus, Player& player)
	{
		Int32 duration = 13 + rand() % 17;
		Int32 hit = (Int32(D6_MAX_LIFE) / 7) + rand() % (Int32(D6_MAX_LIFE) / 2);
		Size type = (bonus.getType() == D6_BONUS_GUESS) ? rand() % (D6_BONUS_COUNT - 1) : bonus.getType();

		switch (type)
		{
		case D6_BONUS_INVIS:
			player.setBonus(type, duration).setAlpha(0.2f);
			d6MessageQueue.add(player, Format(D6_L("Invisibility for {0} seconds")) << duration);
			break;

		case D6_BONUS_SPEED:
			player.setBonus(type, duration).setAlpha(1.0f);
			d6MessageQueue.add(player, Format(D6_L("Fast movement for {0} seconds")) << duration);
			break;

		case D6_BONUS_LIFEP:
			player.adjustLife(Float32(hit));
			d6MessageQueue.add(player, Format(D6_L("Life +{0}")) << hit);
			break;

		case D6_BONUS_LIFEM:
			player.hit(Float32(hit));
			d6MessageQueue.add(player, Format(D6_L("Life -{0}")) << hit);
			break;

		case D6_BONUS_LIFEF:
			player.setFullLife();
			d6MessageQueue.add(player, D6_L("Full life"));
			break;

		case D6_BONUS_SHOTS:
			player.setBonus(type, duration).setAlpha(1.0f);
			d6MessageQueue.add(player, Format(D6_L("Fast reload for {0} seconds")) << duration);
			break;

		case D6_BONUS_SHOTP:
			player.setBonus(type, duration).setAlpha(1.0f);
			d6MessageQueue.add(player, Format(D6_L("Powerful shots for {0} seconds")) << duration);
			break;

		case D6_BONUS_INVUL:
			player.setBonus(type, duration).setAlpha(1.0f);
			d6MessageQueue.add(player, Format(D6_L("Invulnerability for {0} seconds")) << duration);
			break;

		case D6_BONUS_BULLT:
			hit = 5 + rand() % 12;
			player.pickAmmo(hit);
			d6MessageQueue.add(player, Format(D6_L("Bullets +{0}")) << hit);
			break;
		}
	}

	void BONUS_Check(Player& player)
	{
		auto bonusIter = d6Bonuses.begin();
		while (bonusIter != d6Bonuses.end())
		{
			if (BONUS_IsApplicable(*bonusIter, player, false))
			{
				BONUS_Apply(*bonusIter, player);
				Sound::playSample(D6_SND_BNPICK);
				bonusIter = d6Bonuses.erase(bonusIter);
			}
			else
			{
				++bonusIter;
			}
		}
	}

	static void BONUS_PickWeapon(const Bonus& bonus, Player& player)
	{
		if (player.getAmmo() > 0)
		{
			// Leave the current weapon at the same place
			d6Bonuses.push_back(Bonus(bonus.getX(), bonus.getY(), player.getWeapon(), player.getAmmo()));
		}

		player.pickWeapon(bonus.getWeaponType(), bonus.getBullets());
		d6MessageQueue.add(player, Format(D6_L("You picked up gun {0}")) << D6_L(bonus.getWeaponType().name));
	}

	void BONUS_CheckPick(Player& player)
	{
		auto bonusIter = d6Bonuses.begin();
		while (bonusIter != d6Bonuses.end())
		{
			if (BONUS_IsApplicable(*bonusIter, player, true))
			{
				BONUS_PickWeapon(*bonusIter, player);
				d6Bonuses.erase(bonusIter);
				return;
			}
			else
			{
				++bonusIter;
			}
		}
	}
}