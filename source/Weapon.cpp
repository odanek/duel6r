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
#include <set>
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

namespace Duel6
{
	Weapon d6WpnDef[D6_WEAPONS] =
	{
		{ 0, true, 9.15f, true, false, Color(0, 0, 0), 0, 30, 0.98f, "pistol", "pistol.wav", nullptr, 0, false, { 1, 5, 2, 5, 3, 5, 4, 5, 5, 5, 6, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 1, true, 6.1f, false, true, Color(255, 0, 0), 3, 100, 3.28f, "bazooka", "bazooka.wav", "bmbazook.wav", 0.01f, false, { 1, 5, 2, 5, 3, 5, 4, 5, 5, 5, 6, 5, 0, 50, -1, 0 }, { 0, 10, 1, 10, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 2, true, 12.2f, false, true, Color(0, 255, 255), 0, 100, 4.1f, "lightning gun", "blesk.wav", "bmblesk.wav", 0, false, { 1, 5, 2, 5, 3, 5, 4, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 10, 1, 10, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 3, true, 9.15f, true, false, Color(0, 0, 0), 0, 60, 2.29f, "shotgun", "shotgun.wav", nullptr, 0, false, { 1, 5, 2, 5, 3, 5, 2, 5, 1, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 4, true, 12.2f, true, false, Color(0, 0, 0), 0, 25, 0.33f, "plasma gun", "plasma.wav", nullptr, 0, false, { 1, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 5, true, 15.25f, true, false, Color(0, 0, 0), 0, 35, 0.25f, "laser", "laser.wav", nullptr, 0, false, { 1, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ 6, true, 9.15f, true, false, Color(0, 0, 0), 0, 40, 0.66f, "machine gun", "kulomet.wav", nullptr, 0, false, { 1, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ 7, true, 6.1f, false, true, Color(255, 255, 0), 4, 200, 6.56f, "triton", "triton.wav", "bmbazook.wav", 0.04f, false, { 1, 5, 2, 5, 3, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 8, 1, 8, 2, 8, 3, 8, 4, 8, 3, 8, 2, 8, 1, 8, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 8, true, 10.98f, true, false, Color(0, 0, 0), 0, 15, 0.26f, "uzi", "pistol.wav", nullptr, 0, false, { 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 9, true, 12.2f, true, false, Color(0, 0, 0), 0, 25, 0.82f, "bow", "luk.wav", nullptr, 0, false, { 1, 10, 2, 10, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ 10, true, 7.93f, true, false, Color(0, 0, 0), 0, 50, 1.31f, "slime gun", "sliz.wav", nullptr, 0, false, { 1, 10, 2, 10, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 11, true, 12.2f, true, true, Color(255, 255, 0), 2, 80, 3.44f, "double laser", "laser.wav", "bmbazook.wav", 0.005f, false, { 1, 10, 0, 10, 0, 5, 0, 5, 0, 5, 0, 5, 0, 50, -1, 0 }, { 0, 50, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 12, false, 4.27f, false, true, Color(255, 0, 255), 0, 100, 3.11f, "kiss of death", "kiss2.wav", "kiss.wav", 0.01f, false, { 0, 8, 1, 8, 2, 8, 1, 8, 0, 8, 0, 8, 0, 8, -1, 0 }, { 0, 10, 1, 10, 2, 10, 1, 10, -1, 0 }, { 0, 5, 1, 5, 0, 5, 1, 5, 0, 5, 1, 5, -1, 0 } },
		{ 13, false, 4.88f, false, false, Color(0, 0, 0), 0, 20, 0.66f, "spray", "spray.wav", nullptr, 0, false, { 0, 8, 1, 8, 0, 8, 1, 8, 0, 8, 0, 8, 0, 8, -1, 0 }, { 0, 5, 1, 5, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ 14, false, 6.71f, true, false, Color(0, 0, 0), 0, 15, 0.82f, "sling", "prak.wav", nullptr, 0, false, { 0, 5, 1, 5, 0, 5, 2, 5, 0, 5, 0, 5, 0, 5, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ 15, false, 7.93f, true, false, Color(0, 0, 0), 0, 10, 0.9f, "stopper gun", "spunt.wav", nullptr, 0, false, { 0, 5, 1, 5, 1, 5, 2, 5, 2, 5, 0, 5, 0, 5, -1, 0 }, { 0, 50, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ 16, false, 5.49f, false, false, Color(0, 0, 0), 2, 0, 1.97f, "shit thrower", "shit.wav", "shit-hit.wav", 0.04f, true, { 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, -1, 0 }, { 0, 10, 1, 10, 2, 10, 1, 10, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
	};
	
	namespace
	{
		std::unique_ptr<PlayerSkin> brownSkin;
		std::list<Shot> d6Shots;
		typedef std::list<Shot>::iterator ShotIterator;
	}

	struct Hit
	{
		bool hit; // Was there any hit
		Player* player; // Was any player hit directly?
	};

	void WPN_Init(TextureManager& textureManager, Sound& sound, Console& console)
	{
		std::set<Int32> nearestFilterBoom = { 5, 6, 9, 13, 14, 15, 16 };

		console.printLine("\n===Weapon initialization===");
		for (Weapon& weap : d6WpnDef)
		{
			const std::string wpnPath = Format("{0}{1,3|0}") << D6_TEXTURE_WPN_PATH << weap.index;
			const std::string wi = std::to_string(weap.index);
			weap.textures.boom = textureManager.load(Format("{0}/boom/") << wpnPath, nearestFilterBoom.find(weap.index) != nearestFilterBoom.end() ? GL_NEAREST : GL_LINEAR, true);
			weap.textures.gun = textureManager.load(Format("{0}/gun/") << wpnPath, GL_NEAREST, true);
			weap.textures.shot = textureManager.load(Format("{0}/shot/") << wpnPath, GL_NEAREST, true);

			if (weap.shotSound)
			{
				weap.shotSample = sound.loadSample(std::string(D6_FILE_WEAPON_SOUNDS) + weap.shotSound);
			}
			if (weap.boomSound)
			{
				weap.boomSample = sound.loadSample(std::string(D6_FILE_WEAPON_SOUNDS) + weap.boomSound);
			}
		}

		Color brownColor(83, 44, 0);
		PlayerSkinColors skinColors(brownColor);
		brownSkin = std::make_unique<PlayerSkin>("textures/man/", skinColors, textureManager);
	}

	void WPN_DeInit()
	{
		brownSkin.reset();
	}

	void WPN_LevelInit()
	{
		d6Shots.clear();
	}

	void WPN_AddShot(Player& player, SpriteList& spriteList)
	{
		Float32 ad = player.isKneeling() ? 0.52f : 0.32f;
		Float32 x = (player.getOrientation() == Orientation::Left) ? (player.getX() - 0.65f) : (player.getX() + 0.65f);  // TODO: Coord
		Float32 y = player.getY() - ad;

		Sprite shotSprite(player.getWeapon().shotAnimation, player.getWeapon().textures.shot);		
		d6Shots.push_back(Shot(player, x, y, spriteList.addSprite(shotSprite)));
		player.getWeapon().shotSample.play();
	}

	static void WPN_Boom(Shot& shot, std::vector<Player>& players, Player* playerThatWasHit, SpriteList& spriteList)
	{
		std::vector<const Player*> killedPlayers;
		Player& author = shot.getPlayer();

		Float32 range = shot.getExplosionRange();
		Float32 power = shot.getExplosionPower();

		float X = (shot.getOrientation() == Orientation::Left) ? (shot.getX() + 0.32f) : (shot.getX() + 0.67f);
		float Y = shot.getY() + 0.83f;  // TODO: Coord
		bool shit = shot.getWeapon().shit;

		if (!shit)
		{
			FIRE_Check(X, Y, range, spriteList);
		}

		for (Player& player : players)
		{
			bool directHit = (playerThatWasHit != nullptr && player.is(*playerThatWasHit));
			Float32 dist = directHit ? 0 : Math::distance(player.getX() + 0.5f, player.getY() + 0.5f, X, Y); // TODO: Coord

			if (directHit || dist < range)
			{
				if (shit)
				{
					player.useTemporarySkin(*brownSkin);
				}
				else
				{
					if (player.hitByShot(directHit ? power : ((range - dist) * power) / range, shot, directHit))
					{
						killedPlayers.push_back(&player);
					}
				}
			}
		}

		if (std::find(killedPlayers.begin(), killedPlayers.end(), &author) != killedPlayers.end()) // Killed self
		{
			author.playSound(PlayerSounds::Type::Suicide);
			author.getPerson().addPenalties(killedPlayers.size());
		}
		else
		{
			if (killedPlayers.size() > 0)
			{
				author.playSound(PlayerSounds::Type::KilledOther);
			}
			else if (playerThatWasHit != nullptr)
			{
				author.playSound(PlayerSounds::Type::HitOther);
			}

			author.getPerson().addKills(killedPlayers.size());
			author.addRoundKills(killedPlayers.size());
		}

		for (auto player : killedPlayers)
		{
			if (!player->is(author))
			{
				player->playSound(PlayerSounds::Type::WasKilled);
			}
		}
	}

	static Hit WPN_ShotPlayerCollision(Shot& shot, std::vector<Player>& players)
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

			return { true, &player };
		}

		return { false, nullptr };
	}

	static Hit WPN_ShotWallCollision(Shot& shot, const World& world)
	{
		Int32 up = (int)(shot.getY() + 1.0f); // TODO: Coord
		Int32 down = (int)(shot.getY() + 0.65f); // TODO: Coord
		
		Int32 left, right;
		if (shot.getOrientation() == Orientation::Left)
		{
			left = (int)(shot.getX());
			right = (int)(shot.getX() + 0.65f); // TODO: Coord
		}
		else
		{
			left = (int)(shot.getX() + 0.35f); // TODO: Coord
			right = (int)(shot.getX() + 1.0f); // TODO: Coord
		}

		if (world.isWall(left, up, true) || world.isWall(left, down, true) ||
			world.isWall(right, up, true) || world.isWall(right, down, true))
		{
			return { true, nullptr };
		}

		return { false, nullptr };
	}

	static ShotIterator WPN_RemoveShot(ShotIterator shot, SpriteList& spriteList)
	{
		spriteList.removeSprite(shot->getSprite());
		return d6Shots.erase(shot);
	}

	void WPN_MoveShots(GameService& gameService, float elapsedTime)
	{
		auto shot = d6Shots.begin();

		while (shot != d6Shots.end())
		{
			const Weapon& weapon = shot->getWeapon();
			shot->move(elapsedTime);

			Hit hit = WPN_ShotPlayerCollision(*shot, gameService.getPlayers());
			if (!hit.hit)
			{
				hit = WPN_ShotWallCollision(*shot, gameService.getWorld());
			}

			if (hit.hit)
			{
				WPN_Boom(*shot, gameService.getPlayers(), hit.player, gameService.getSpriteList());
				float x = (shot->getOrientation() == Orientation::Left) ? shot->getX() - 0.3f : shot->getX() + 0.3f;
				
				Sprite boom(weapon.boomAnimation, weapon.textures.boom);
				boom.setPosition(x, shot->getY() + 0.3f, 0.6f)
					.setSpeed(2.0f)
					.setLooping(AnimationLooping::OnceAndRemove)
					.setOrientation(shot->getOrientation())
					.setAlpha(0.6f);

				SpriteIterator boomSprite = gameService.getSpriteList().addSprite(boom);

				if (shot->getPlayer().hasPowerfulShots())
					boomSprite->setGrow(weapon.expGrow * 1.2f);
				else
					boomSprite->setGrow(weapon.expGrow);
				weapon.boomSample.play();
				if (weapon.boom > 0)
				{
					boomSprite->setNoDepth(true);
				}
				
				shot = WPN_RemoveShot(shot, gameService.getSpriteList());
			}
			else
			{
				++shot;
			}
		}
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