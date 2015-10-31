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
#include "World.h"
#include "collision/Collision.h"

namespace Duel6
{
	BonusList::BonusList(const GameSettings& settings, const GameResources& resources, World& world)
		: settings(settings), textures(resources.getBonuseTextures()), world(world)
	{}

	GLuint BonusList::getTexture(Size type) const
	{
		return textures.getGlTextures()[type];
	}

	void BonusList::clear()
	{
		bonuses.clear();
	}

	void BonusList::render() const
	{
		glEnable(GL_ALPHA_TEST);

		for (const Bonus& bonus : bonuses)
		{
			bonus.render();
		}

		glDisable(GL_ALPHA_TEST);
	}

	void BonusList::addNew()
	{
		const Level& level = world.getLevel();
		bool weapon = ((rand() % 2) == 1);
		Int32 x = rand() % level.getWidth();
		Int32 y = rand() % level.getHeight();

		if (!level.isWall(x, y, true))
		{
			if (weapon)
			{
				if (!level.isWall(x, y - 1, true)) // Weapons must be on the ground
				{
					return;
				}
				bonuses.push_back(Bonus(Vector(x, y), Weapon::getRandomEnabled(settings), rand() % 10 + 10));
			}
			else
			{
				Size type = rand() % D6_BONUS_COUNT;
				bonuses.push_back(Bonus(Vector(x + 0.2f, y + 0.2f), type, textures.getGlTextures()[type]));
			}
		}
	}

	void BonusList::addDeadManGun(Player& player, const Vector& position)
	{
		bonuses.push_back(Bonus(position, player.getWeapon(), player.getAmmo()));
	}

	bool BonusList::isApplicable(const Bonus& bonus, const Player& player, bool weapon) const
	{
		if (weapon != bonus.isWeapon())
		{
			return false;
		}

		if (!weapon && (bonus.getType() == D6_BONUS_LIFEM && player.isInvulnerable()))
		{
			return false;
		}

		return Collision::rectangles(bonus.getCollisionRect(), player.getCollisionRect());
	}

	void BonusList::apply(const Bonus& bonus, Player& player)
	{
		InfoMessageQueue& messageQueue = world.getMessageQueue();
		Int32 duration = 13 + rand() % 17;
		Int32 hit = (Int32(D6_MAX_LIFE) / 7) + rand() % (Int32(D6_MAX_LIFE) / 2);
		Size type = (bonus.getType() == D6_BONUS_GUESS) ? rand() % (D6_BONUS_COUNT - 1) : bonus.getType();

		switch (type)
		{
		case D6_BONUS_INVIS:
			player.setBonus(type, duration).setAlpha(0.2f);
			messageQueue.add(player, Format("Invisibility for {0} seconds") << duration);
			break;

		case D6_BONUS_SPEED:
			player.setBonus(type, duration).setAlpha(1.0f);
			messageQueue.add(player, Format("Fast movement for {0} seconds") << duration);
			break;

		case D6_BONUS_LIFEP:
			player.addLife(Float32(hit));
			messageQueue.add(player, Format("Life +{0}") << hit);
			break;

		case D6_BONUS_LIFEM:
			if (player.hit(Float32(hit)))
			{
				player.playSound(PlayerSounds::Type::WasKilled);
			}
			messageQueue.add(player, Format("Life -{0}") << hit);
			break;

		case D6_BONUS_LIFEF:
			player.setFullLife();
			messageQueue.add(player, "Full life");
			break;

		case D6_BONUS_SHOTS:
			player.setBonus(type, duration).setAlpha(1.0f);
			messageQueue.add(player, Format("Fast reload for {0} seconds") << duration);
			break;

		case D6_BONUS_SHOTP:
			player.setBonus(type, duration).setAlpha(1.0f);
			messageQueue.add(player, Format("Powerful shots for {0} seconds") << duration);
			break;

		case D6_BONUS_INVUL:
			player.setBonus(type, duration).setAlpha(1.0f);
			messageQueue.add(player, Format("Invulnerability for {0} seconds") << duration);
			break;

		case D6_BONUS_BULLT:
			hit = 5 + rand() % 12;
			player.pickAmmo(hit);
			messageQueue.add(player, Format("Bullets +{0}") << hit);
			break;

		case D6_BONUS_SPLITFIRE:
			player.setBonus(type, duration).setAlpha(1.0f);
			messageQueue.add(player, Format("Splitfire for {0} seconds") << duration);
			break;

		case D6_BONUS_VAMPIRESHOTS:
			player.setBonus(type, duration).setAlpha(1.0f);
			messageQueue.add(player, Format("Vampire shots for {0} seconds") << duration);
			break;
		}
	}

	void BonusList::check(Player& player)
	{
		auto bonusIter = bonuses.begin();
		while (bonusIter != bonuses.end())
		{
			if (isApplicable(*bonusIter, player, false))
			{
				apply(*bonusIter, player);
				player.playSound(PlayerSounds::Type::PickedBonus);
				bonusIter = bonuses.erase(bonusIter);
			}
			else
			{
				++bonusIter;
			}
		}
	}

	void BonusList::pickWeapon(const Bonus& bonus, Player& player)
	{
		if (player.getAmmo() > 0)
		{
			// Leave the current weapon at the same place
			bonuses.push_back(Bonus(bonus.getPosition(), player.getWeapon(), player.getAmmo()));
		}

		player.pickWeapon(bonus.getWeaponType(), bonus.getBullets());
		world.getMessageQueue().add(player, Format("You picked up gun {0}") << bonus.getWeaponType().getName());
	}

	void BonusList::checkPick(Player& player)
	{
		auto bonusIter = bonuses.begin();
		while (bonusIter != bonuses.end())
		{
			if (isApplicable(*bonusIter, player, true))
			{
				pickWeapon(*bonusIter, player);
				bonuses.erase(bonusIter);
				return;
			}
			else
			{
				++bonusIter;
			}
		}
	}
}