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
#include "project.h"
#include "Player.h"
#include "PlayerSkin.h"
#include "Weapon.h"

namespace Duel6
{
	bool d6WpnEnabled[D6_WEAPONS];
	std::vector<GLuint> d6WpnTextures;
	
	namespace
	{
		std::unique_ptr<PlayerSkin> brownSkin;
		std::list<Shot> d6Shots;
		typedef std::list<Shot>::iterator ShotIterator;
	}

	void WPN_LoadTextures(void)
	{
		myKh3info_s     ki;
		int             i;

		g_app.con->printf(MY_L("APP00084|Nahravam textury zbrani\n"));
		MY_KH3Open(D6_FILE_WEAPON);
		MY_KH3GetInfo(&ki);
		g_app.con->printf(MY_L("APP00085|...Soubor %s obsahue %d textur\n"), D6_FILE_WEAPON, ki.picts);
		d6WpnTextures.resize(ki.picts);

		for (i = 0; i < (int)ki.picts; i++)
		{
			d6WpnTextures[i] = UTIL_LoadKH3Texture(D6_FILE_WEAPON, i, true);
			if (i < 14 || (i > 21 && i < 78) || i > 79)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
		}

		MY_KH3Close();
	}

	void WPN_FreeTextures(void)
	{
		glDeleteTextures(d6WpnTextures.size(), &d6WpnTextures[0]);
		d6WpnTextures.clear();
	}

	void WPN_Init(void)
	{
		Color brownColor(83, 44, 0);
		PlayerSkinColors skinColors(brownColor);
		brownSkin.reset(new PlayerSkin(skinColors));
	}

	void WPN_DeInit(void)
	{
		brownSkin.reset();
	}

	void WPN_LevelInit(void)
	{
		d6Shots.clear();
	}

	void WPN_Shoot(Player& player)
	{
		// TODO: Move this to player
		d6PLSTATE_s *s = &player.State;

		if (!player.getAmmo() || player.isReloading())
			return;

		if (s->Bonus == D6_BONUS_SHOTS)
			s->SI = float(player.getWeapon().ReloadSpeed / 2);
		else
			s->SI = float(player.getWeapon().ReloadSpeed);

		s->Ammo--;

		s->GA->setFrame(0);

		player.getPerson().setShots(player.getPerson().getShots() + 1);

		Float32 ad = player.isKneeling() ? 0.52f : 0.32f;
		Float32 shotX = (s->O == Orientation::Left) ? (s->X - 0.65f) : (s->X + 0.65f);
		Float32 shotY = s->Y - ad;

		Sprite shotSprite(player.getWeapon().shotAnimation, d6WpnTextures);		
		d6Shots.push_back(Shot(player, shotX, shotY, d6SpriteList.addSprite(shotSprite)));
		SOUND_PlaySample(player.getWeapon().ShSound);
	}

	static ShotIterator WPN_RemoveShot(ShotIterator shot)
	{
		d6SpriteList.removeSprite(shot->getSprite());
		return d6Shots.erase(shot);
	}

	void WPN_MoveShots(float elapsedTime)
	{
		auto shot = d6Shots.begin();

		while (shot != d6Shots.end())
		{
			const Weapon& weapon = shot->getWeapon();
			shot->move(elapsedTime);

			if (KONTR_Shot(*shot))
			{				
				float x = (shot->getOrientation() == Orientation::Left) ? shot->getX() - 0.3f : shot->getX() + 0.3f;
				
				Sprite boom(weapon.boomAnimation, d6WpnTextures);
				boom.setPosition(x, shot->getY() + 0.3f, 0.6f)
					.setSpeed(2.0f)
					.setLooping(AnimationLooping::OnceAndRemove)
					.setOrientation(shot->getOrientation())
					.setAlpha(0.6f);

				SpriteIterator boomSprite = d6SpriteList.addSprite(boom);

				if (shot->getPlayer().hasPowerfulShots())
					boomSprite->setGrow(weapon.ExpGrow * 1.2f);
				else
					boomSprite->setGrow(weapon.ExpGrow);
				if (weapon.BmSound != -1)
					SOUND_PlaySample(weapon.BmSound);
				if (weapon.Boom > 0)
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

	void WPN_Boom(Shot& shot, Player* playerThatWasHit)
	{
		int killedPlayers = 0, initialAuthorKills = shot.getPlayer().getPerson().getKills();
		bool killedSelf = false;

		Float32 range = shot.getExplosionRange();
		Float32 power = shot.getExplosionPower();

		float X = (shot.getOrientation() == Orientation::Left) ? (shot.getX() + 0.32f) : (shot.getX() + 0.67f);
		float Y = shot.getY() - 0.17f;
		bool shit = shot.getWeapon().shit;

		if (!shit)
		{
			FIRE_Check(X, Y, range);
		}

		for (Player& player : d6Players)
		{
			if (player.is(*playerThatWasHit))
			{
				if (shit)
				{
					player.useTemporarySkin(*brownSkin);
				}
				else
				{
					if (player.hit(power, &shot, true))
					{
						killedPlayers++;
					}
				}
			}
			else
			{
				Float32 dist = (float)sqrt(D6_SQR(player.getX() + 0.5f - X) + D6_SQR(player.getY() - 0.5f - Y));

				if (dist < range)
				{
					if (shit)
					{
						player.useTemporarySkin(*brownSkin);
					}
					else
					{
						if (player.hit(((range - dist) * power) / range, &shot, false))
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
			shot.getPlayer().getPerson().setKills(initialAuthorKills - killedPlayers);
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

		for (Size i = 0; i < D6_WEAPONS; i++)
		{
			if (d6WpnEnabled[i])
			{
				enabledWeaponCount++;
			}
		}

		Size weapon = rand() % enabledWeaponCount;
		for (Size i = 0; i < D6_WEAPONS; i++)
		{
			if (d6WpnEnabled[i])
			{
				if (weapon-- == 0)
				{
					return d6WpnDef[i];
				}
			}
		}

		return d6WpnDef[0];
	}
}