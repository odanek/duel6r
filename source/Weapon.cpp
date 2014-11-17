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
#include <list>
#include "Sound.h"
#include "Player.h"
#include "PlayerSkin.h"
#include "Weapon.h"
#include "Math.h"
#include "Game.h"
#include "Util.h"
#include "TextureManager.h"
#include "World.h"
#include "Fire.h"
#include "Globals.h"

namespace Duel6
{
	Weapon d6WpnDef[D6_WEAPONS] =
	{
		{ true, 9.15f, true, false, Color(0, 0, 0), 0, 30, 0.98f, "pistol", D6_SND_SHOT_PI, -1, 0, false, { 25, 5, 26, 5, 27, 5, 28, 5, 29, 5, 30, 5, 24, 50, -1, 0 }, { 0, 50, -1, 0 }, { 14, 5, 15, 5, 14, 5, 15, 5, 14, 5, 15, 5, -1, 0 } },
		{ true, 6.1f, false, true, Color(255, 0, 0), 3, 100, 3.28f, "bazooka", D6_SND_SHOT_BZ, D6_SND_BOOM_BZ, 0.01f, false, { 32, 5, 33, 5, 34, 5, 35, 5, 36, 5, 37, 5, 31, 50, -1, 0 }, { 1, 10, 2, 10, -1, 0 }, { 16, 5, 17, 5, 16, 5, 17, 5, 16, 5, 17, 5, -1, 0 } },
		{ true, 12.2f, false, true, Color(0, 255, 255), 0, 100, 4.1f, "lightning gun", D6_SND_SHOT_BL, D6_SND_BOOM_BL, 0, false, { 39, 5, 40, 5, 41, 5, 42, 5, 38, 5, 38, 5, 38, 50, -1, 0 }, { 3, 10, 4, 10, -1, 0 }, { 18, 5, 19, 5, 18, 5, 19, 5, 18, 5, 19, 5, -1, 0 } },
		{ true, 9.15f, true, false, Color(0, 0, 0), 0, 60, 2.29f, "shotgun", D6_SND_SHOT_SG, -1, 0, false, { 44, 5, 45, 5, 46, 5, 45, 5, 44, 5, 43, 5, 43, 50, -1, 0 }, { 5, 50, -1, 0 }, { 14, 5, 15, 5, 14, 5, 15, 5, 14, 5, 15, 5, -1, 0 } },
		{ true, 12.2f, true, false, Color(0, 0, 0), 0, 25, 0.33f, "plasma gun", D6_SND_SHOT_PL, -1, 0, false, { 48, 5, 47, 5, 47, 5, 47, 5, 47, 5, 47, 5, 47, 50, -1, 0 }, { 6, 50, -1, 0 }, { 20, 5, 21, 5, 20, 5, 21, 5, 20, 5, 21, 5, -1, 0 } },
		{ true, 15.25f, true, false, Color(0, 0, 0), 0, 35, 0.25f, "laser", D6_SND_SHOT_LS, -1, 0, false, { 50, 5, 49, 5, 49, 5, 49, 5, 49, 5, 49, 5, 49, 50, -1, 0 }, { 7, 50, -1, 0 }, { 22, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ true, 9.15f, true, false, Color(0, 0, 0), 0, 40, 0.66f, "machine gun", D6_SND_SHOT_KM, -1, 0, false, { 52, 5, 51, 5, 51, 5, 51, 5, 51, 5, 51, 5, 51, 50, -1, 0 }, { 8, 50, -1, 0 }, { 23, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ true, 6.1f, false, true, Color(255, 255, 0), 4, 200, 6.56f, "triton", D6_SND_SHOT_TZ, D6_SND_BOOM_BZ, 0.04f, false, { 54, 5, 55, 5, 56, 5, 53, 5, 53, 5, 53, 5, 53, 50, -1, 0 }, { 9, 8, 10, 8, 11, 8, 12, 8, 13, 8, 12, 8, 11, 8, 10, 8, -1, 0 }, { 16, 5, 17, 5, 16, 5, 17, 5, 16, 5, 17, 5, -1, 0 } },		
		{ true, 10.98f, true, false, Color(0, 0, 0), 0, 15, 0.26f, "uzi", D6_SND_SHOT_PI, -1, 0, false, { 69, 5, 69, 5, 69, 5, 69, 5, 69, 5, 69, 5, 69, 50, -1, 0 }, { 0, 50, -1, 0 }, { 14, 5, 15, 5, 14, 5, 15, 5, 14, 5, 15, 5, -1, 0 } },
		{ true, 12.2f, true, false, Color(0, 0, 0), 0, 25, 0.82f, "bow", D6_SND_SHOT_LK, -1, 0, false, { 71, 10, 72, 10, 70, 5, 70, 5, 70, 5, 70, 5, 70, 50, -1, 0 }, { 73, 50, -1, 0 }, { 22, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ true, 7.93f, true, false, Color(0, 0, 0), 0, 50, 1.31f, "slime gun", D6_SND_SHOT_SM, -1, 0, false, { 75, 10, 76, 10, 74, 5, 74, 5, 74, 5, 74, 5, 74, 50, -1, 0 }, { 77, 50, -1, 0 }, { 78, 5, 79, 5, 78, 5, 79, 5, 78, 5, 79, 5, -1, 0 } },
		{ true, 12.2f, true, true, Color(255, 255, 0), 2, 80, 3.44f, "double laser", D6_SND_SHOT_LS, D6_SND_BOOM_BZ, 0.005f, false, { 81, 10, 80, 10, 80, 5, 80, 5, 80, 5, 80, 5, 80, 50, -1, 0 }, { 82, 50, -1, 0 }, { 16, 5, 17, 5, 16, 5, 17, 5, 16, 5, 17, 5, -1, 0 } },
		{ false, 4.27f, false, true, Color(255, 0, 255), 0, 100, 3.11f, "kiss of death", D6_SND_SHOT_KD1, D6_SND_SHOT_KD2, 0.01f, false, { 94, 8, 95, 8, 96, 8, 95, 8, 94, 8, 94, 8, 94, 8, -1, 0 }, { 97, 10, 98, 10, 99, 10, 98, 10, -1, 0 }, { 20, 5, 21, 5, 20, 5, 21, 5, 20, 5, 21, 5, -1, 0 } },
		{ false, 4.88f, false, false, Color(0, 0, 0), 0, 20, 0.66f, "spray", D6_SND_SHOT_SPRAY, -1, 0, false, { 100, 8, 101, 8, 100, 8, 101, 8, 100, 8, 100, 8, 100, 8, -1, 0 }, { 102, 5, 103, 5, -1, 0 }, { 104, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ false, 6.71f, true, false, Color(0, 0, 0), 0, 15, 0.82f, "sling", D6_SND_SHOT_SLING, -1, 0, false, { 105, 5, 106, 5, 105, 5, 107, 5, 105, 5, 105, 5, 105, 5, -1, 0 }, { 108, 50, -1, 0 }, { 22, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ false, 7.93f, true, false, Color(0, 0, 0), 0, 10, 0.9f, "stopper gun", D6_SND_SHOT_SPUNT, -1, 0, false, { 109, 5, 110, 5, 110, 5, 111, 5, 111, 5, 109, 5, 109, 5, -1, 0 }, { 112, 50, -1, 0 }, { 22, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ false, 5.49f, false, false, Color(0, 0, 0), 2, 0, 1.97f, "shit thrower", D6_SND_SHOT_SHIT, D6_SND_SHOT_SHIT_HIT, 0.04f, true, { 114, 5, 114, 5, 114, 5, 114, 5, 114, 5, 114, 5, 114, 5, -1, 0 }, { 115, 10, 116, 10, 117, 10, 116, 10, -1, 0 }, { 113, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
	};
	
	namespace
	{
		PlayerSkin brownSkin;
		std::list<Shot> d6Shots;
		typedef std::list<Shot>::iterator ShotIterator;
	}

	void WPN_Init()
	{
		const TextureManager::TextureList& textures = d6TextureManager.get(D6_TEXTURE_WPN_KEY);
		for (Size i = 0; i < textures.size(); ++i)
		{
			if (i < 14 || (i > 21 && i < 78) || i > 79)
			{
				glBindTexture(GL_TEXTURE_2D, textures[i]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
		}

		Color brownColor(83, 44, 0);
		PlayerSkinColors skinColors(brownColor);
		brownSkin = PlayerSkin::create("textures/man/", skinColors);
	}

	void WPN_LevelInit()
	{
		d6Shots.clear();
	}

	void WPN_AddShot(Player& player)
	{
		Float32 ad = player.isKneeling() ? 0.52f : 0.32f;
		Float32 x = (player.getOrientation() == Orientation::Left) ? (player.getX() - 0.65f) : (player.getX() + 0.65f);  // TODO: Coord
		Float32 y = player.getY() - ad;

		Sprite shotSprite(player.getWeapon().shotAnimation, d6TextureManager.get(D6_TEXTURE_WPN_KEY));		
		d6Shots.push_back(Shot(player, x, y, d6SpriteList.addSprite(shotSprite)));
		Sound::playSample(player.getWeapon().shotSound);
	}

	static void WPN_Boom(Shot& shot, std::vector<Player>& players, Player* playerThatWasHit)
	{
		int killedPlayers = 0, initialAuthorKills = shot.getPlayer().getPerson().getKills();
		bool killedSelf = false;

		Float32 range = shot.getExplosionRange();
		Float32 power = shot.getExplosionPower();

		float X = (shot.getOrientation() == Orientation::Left) ? (shot.getX() + 0.32f) : (shot.getX() + 0.67f);
		float Y = shot.getY() + 0.83f;  // TODO: Coord
		bool shit = shot.getWeapon().shit;

		if (!shit)
		{
			FIRE_Check(X, Y, range);
		}

		for (Player& player : players)
		{
			if (player.is(*playerThatWasHit))
			{
				if (shit)
				{
					player.useTemporarySkin(brownSkin);
				}
				else
				{
					if (player.hitByShot(power, shot, true))
					{
						killedPlayers++;
					}
				}
			}
			else
			{
				Float32 dist = Math::distance(player.getX() + 0.5f, player.getY() + 0.5f, X, Y); // TODO: Coord

				if (dist < range)
				{
					if (shit)
					{
						player.useTemporarySkin(brownSkin);
					}
					else
					{
						if (player.hitByShot(((range - dist) * power) / range, shot, false))
						{
							killedPlayers++;

							if (player.is(shot.getPlayer()))
							{
								killedSelf = true;
							}
						}
					}
				}
			}
		}

		if (killedSelf)
		{
			shot.getPlayer().getPerson().addPenalties(killedPlayers);
		}
		else
		{
			shot.getPlayer().getPerson().addKills(killedPlayers);
			shot.getPlayer().addRoundKills(killedPlayers);
		}
	}

	static bool WPN_ShotPlayerCollision(Shot& shot, std::vector<Player>& players)
	{
		Float32 X = (shot.getOrientation() == Orientation::Left) ? shot.getX() : shot.getX() + 0.35f; // TODO: Coord

		for (Player& player : players)
		{
			if (player.getBonus() == D6_BONUS_INVIS || player.is(shot.getPlayer()))
			{
				continue;
			}
			if (!player.isInGame())
			{
				continue;
			}

			Float32 ad = player.isKneeling() ? 0.2f : (player.isLying() ? 0.6f : 0.0f); // TODO: Coord

			if (X > player.getX() + 1.0f || X + 0.65f < player.getX() ||
				shot.getY() < player.getY() - 1.0f || shot.getY() - 0.35f > player.getY() - ad) // TODO: Coord
				continue;

			WPN_Boom(shot, players, &player);
			return true;
		}

		return false;
	}

	static bool WPN_ShotCollision(Game& game, Shot& s)
	{
		if (WPN_ShotPlayerCollision(s, game.getPlayers()))
		{
			return true;
		}

		Int32 up = (int)(s.getY() + 1.0f); // TODO: Coord
		Int32 down = (int)(s.getY() + 0.65f); // TODO: Coord
		
		Int32 left, right;
		if (s.getOrientation() == Orientation::Left)
		{
			left = (int)(s.getX());
			right = (int)(s.getX() + 0.65f); // TODO: Coord
		}
		else
		{
			left = (int)(s.getX() + 0.35f); // TODO: Coord
			right = (int)(s.getX() + 1.0f); // TODO: Coord
		}

		const World& world = game.getWorld();
		if (world.isWall(left, up, true) || world.isWall(left, down, true) ||
			world.isWall(right, up, true) || world.isWall(right, down, true))
		{
			WPN_Boom(s, game.getPlayers(), nullptr);
			return true;
		}

		return false;
	}

	static ShotIterator WPN_RemoveShot(ShotIterator shot)
	{
		d6SpriteList.removeSprite(shot->getSprite());
		return d6Shots.erase(shot);
	}

	void WPN_MoveShots(Game& game, float elapsedTime)
	{
		auto shot = d6Shots.begin();

		while (shot != d6Shots.end())
		{
			const Weapon& weapon = shot->getWeapon();
			shot->move(elapsedTime);

			if (WPN_ShotCollision(game, *shot))
			{				
				float x = (shot->getOrientation() == Orientation::Left) ? shot->getX() - 0.3f : shot->getX() + 0.3f;
				
				Sprite boom(weapon.boomAnimation, d6TextureManager.get(D6_TEXTURE_WPN_KEY));
				boom.setPosition(x, shot->getY() + 0.3f, 0.6f)
					.setSpeed(2.0f)
					.setLooping(AnimationLooping::OnceAndRemove)
					.setOrientation(shot->getOrientation())
					.setAlpha(0.6f);

				SpriteIterator boomSprite = d6SpriteList.addSprite(boom);

				if (shot->getPlayer().hasPowerfulShots())
					boomSprite->setGrow(weapon.expGrow * 1.2f);
				else
					boomSprite->setGrow(weapon.expGrow);
				if (weapon.boomSound != -1)
					Sound::playSample(weapon.boomSound);
				if (weapon.boom > 0)
				{
					boomSprite->setNoDepth(true);
				}
				
				shot = WPN_RemoveShot(shot);
			}
			else
			{
				++shot;
			}
		}
	}

	/*
	==================================================
	WPN_GetRandomWeapon

	Vybere a vraci cislo nahodne zbrane (musi byt enabled)
	==================================================
	*/
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