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
#include "project.h"
#include "Bonus.h"

namespace Duel6
{
	extern Int16 d6WpnAnm[D6_WEAPONS][16];

	Float32 Bonus::getScreenX() const
	{
		return Float32(x);
	}

	Float32 Bonus::getScreenY() const
	{
		return Float32(d6World.Level.SizeY - y);
	}

	void Bonus::render() const
	{
		Float32 rx = getScreenX();
		Float32 ry = getScreenY();

		glBindTexture(GL_TEXTURE_2D, getTexture());
		glBegin(GL_QUADS);
			glTexCoord2f(0.1f, 0.1f); glVertex3f(rx, ry, 0.5f);
			glTexCoord2f(0.9f, 0.1f); glVertex3f(rx + 1.0f, ry, 0.5f);
			glTexCoord2f(0.9f, 0.9f); glVertex3f(rx + 1.0f, ry - 1.0f, 0.5f);
			glTexCoord2f(0.1f, 0.9f); glVertex3f(rx, ry - 1.0f, 0.5f);
		glEnd();
	}

	GLuint Bonus::getTexture() const
	{
		if (weapon)
		{
			return d6WpnTextures[d6WpnAnm[type][12]];
		}
			
		return d6World.Anm.textures[type];
	}

	static std::list<Bonus> d6Bonuses;
	int d6BonusArt[D6_BONUS_COUNT] = { 19, 20, 21, 22, 23, 24, 25, 46, 53, 26 };  // Otaznik musi byt posledni

	void BONUS_Init(const std::vector<Bonus>& initialBonuses)
	{
		d6Bonuses.clear();
		for (const Bonus& bonus : initialBonuses)
		{
			d6Bonuses.push_back(bonus);
		}
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

	void BONUS_AddNew()
	{	
		bool weapon = ((rand() % 2) == 1);
		Int32 x = rand() % d6World.Level.SizeX;
		Int32 y = rand() % d6World.Level.SizeY;

		if (weapon && D6_BlockZ(x, y + 1) != D6_ANM_F_BLOCK) // Weapons must be on the ground
		{
			return;
		}

		if (D6_BlockZ(x, y) != D6_ANM_F_BLOCK)
		{
			Size type = weapon ? WPN_GetRandomWeapon() : d6BonusArt[rand() % D6_BONUS_COUNT];
			Int32 bullets = rand() % 10 + 10;
			d6Bonuses.push_back(Bonus(x, y, type, weapon, bullets));
		}
	}

	void BONUS_AddDeadManGun(int x, int y, Player& player)
	{
		d6Bonuses.push_back(Bonus(x, y, player.getWeapon(), true, player.getAmmo()));
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

		// TODO: Unify screen and world coordinates!
		return (fabs(bonus.getScreenX() - player.getX()) < 0.5f && fabs(bonus.getScreenY() - player.getY()) < 0.5f);
	}

	static void BONUS_Apply(const Bonus& bonus, Player& player)
	{
		Int32 duration = 13 + rand() % 17;
		Int32 hit = (D6_MAX_LIFE / 7) + rand() % (D6_MAX_LIFE / 2);
		Size type = (bonus.getType() == D6_BONUS_GUESS) ? d6BonusArt[rand() % (D6_BONUS_COUNT - 1)] : bonus.getType();

		switch (type)
		{
		case D6_BONUS_INVIS:
			player.setBonus(type, duration).setAlpha(0.2f);
			d6MessageQueue.add(player, MY_L("APP00094|Neviditelnost na %d sekund"), duration);
			break;

		case D6_BONUS_SPEED:
			player.setBonus(type, duration).setAlpha(1.0f);
			d6MessageQueue.add(player, MY_L("APP00093|Rychly pohyb na %d sekund"), duration);
			break;

		case D6_BONUS_LIFEP:
			player.adjustLife(Float32(hit));			
			d6MessageQueue.add(player, MY_L("APP00012|Zivot +%d"), hit);
			break;

		case D6_BONUS_LIFEM:
			player.hit(Float32(hit), nullptr, false);
			d6MessageQueue.add(player, MY_L("APP00013|Zivot -%d"), hit);
			break;

		case D6_BONUS_LIFEF:
			player.setFullLife();
			d6MessageQueue.add(player, MY_L("APP00014|Plny zivot"), hit);
			break;

		case D6_BONUS_SHOTS:
			player.setBonus(type, duration).setAlpha(1.0f);
			d6MessageQueue.add(player, MY_L("APP00015|Rychle nabiti na %d sekund"), duration);
			break;

		case D6_BONUS_SHOTP:
			player.setBonus(type, duration).setAlpha(1.0f);
			d6MessageQueue.add(player, MY_L("APP00016|Silne strely na %d sekund"), duration);
			break;

		case D6_BONUS_INVUL:
			player.setBonus(type, duration).setAlpha(1.0f);
			d6MessageQueue.add(player, MY_L("APP00017|Nesmrtelnost na %d sekund"), duration);
			break;

		case D6_BONUS_BULLT:
			hit = 5 + rand() % 12;
			player.adjustAmmo(hit);
			d6MessageQueue.add(player, MY_L("APP00018|Naboje +%d"), hit);
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
				SOUND_PlaySample(D6_SND_BNPICK);
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
			d6Bonuses.push_back(Bonus(bonus.getX(), bonus.getY(), player.getWeapon(), true, player.getAmmo()));
		}

		player.pickWeapon(bonus.getType(), bonus.getBullets());
		d6MessageQueue.add(player, MY_L("APP00019|Sebral jsi zbran %s"), MY_L(d6WpnDef[bonus.getType()].Name));
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