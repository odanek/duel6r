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
#include "PlayerSkin.h"

#define D6_COL_WPN_SHT      16

namespace Duel6
{
	myUINT *d6WpnTexture;
	
	namespace
	{
		int d6WpnTextures, d6Shots;
		PlayerSkin *brownSkin;
		d6SHOT_s d6Shot[ANM_MAX];
	}

	void WPN_LoadTextures(void)
	{
		myKh3info_s     ki;
		int             i;

		g_app.con->printf(MY_L("APP00084|Nahravam textury zbrani\n"));
		MY_KH3Open(D6_FILE_WEAPON);
		MY_KH3GetInfo(&ki);
		g_app.con->printf(MY_L("APP00085|...Soubor %s obsahue %d textur\n"), D6_FILE_WEAPON, ki.picts);
		d6WpnTexture = D6_MALLOC(myUINT, ki.picts);

		for (i = 0; i < (int)ki.picts; i++)
		{
			UTIL_LoadKH3Texture(&d6WpnTexture[i], D6_FILE_WEAPON, i, true);
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
		glDeleteTextures(d6WpnTextures, d6WpnTexture);
		MY_Free(d6WpnTexture);
	}

	void WPN_Init(void)
	{
		Color brownColor(83, 44, 0);
		PlayerSkinColors skinColors(brownColor);
		brownSkin = new PlayerSkin(skinColors);
	}

	void WPN_DeInit(void)
	{
		delete brownSkin;
	}

	void WPN_LevelInit(void)
	{
		d6Shots = 0;
	}

	void WPN_Shoot(Player& player)
	{
		float       ad = 0.32f;
		d6SHOT_s    *sh;
		d6PLSTATE_s *s = &player.State;

		if (!s->Ammo || s->SI || d6Shots >= ANM_MAX)
			return;

		if (s->Bonus == D6_BONUS_SHOTS)
			s->SI = float(d6WpnDef[s->GN].ReloadSpeed / 2);
		else
			s->SI = float(d6WpnDef[s->GN].ReloadSpeed);

		s->Ammo--;
		sh = &d6Shot[d6Shots++];
		ANM_SetAnm(s->GA, 0);

		if (s->Flags & D6_FLAG_KNEE)
			ad = 0.52f;

		player.Person().SetShots(player.Person().Shots() + 1);
		sh->Y = s->Y - ad;
		sh->X = (s->O == Orientation::Left) ? (s->X - 0.65f) : (s->X + 0.65f);
		sh->O = s->O;
		sh->GN = s->GN;
		sh->WD = &d6WpnDef[s->GN];
		sh->Author = &player;
		sh->I = d6Shots - 1;
		sh->A = ANM_Add(sh->X, sh->Y, 0.6f, 1, ANM_LOOP_FOREVER, sh->O, d6ShotAnm[sh->GN], d6WpnTexture, false);
		SOUND_PlaySample(sh->WD->ShSound);
	}

	static void WPN_RemoveShot(d6SHOT_s *s)
	{
		int     i;

		d6Shots--;
		if (s->I < d6Shots)
		{
			i = s->I;
			memcpy((void *)s, (void *)&d6Shot[d6Shots], sizeof (d6SHOT_s));
			s->I = i;
		}
	}

	void WPN_MoveShots(float elapsedTime)
	{
		d6SHOT_s    *s;
		float       x;
		int         i, j;

		for (i = 0; i < d6Shots; i++)
		{
			s = &d6Shot[i];

			if (s->O == Orientation::Right)
				s->X += s->WD->ShotSpeed * elapsedTime;
			else
				s->X -= s->WD->ShotSpeed * elapsedTime;

			ANM_ReSet(s->A, s->X, s->Y, -1, Orientation::None, NULL);

			if (KONTR_Shot(s))
			{
				ANM_RemoveFlags(s->A, ANM_FLAG_ALL);
				x = (s->O == Orientation::Left) ? s->X - 0.3f : s->X + 0.3f;
				j = ANM_Add(x, s->Y + 0.3f, 0.6f, 2, ANM_LOOP_ONEKILL, s->O, d6BoomAnm[s->GN], d6WpnTexture, true);
				if (s->Author->HasPowerfulShots())
					ANM_Grow(j, s->WD->ExpGrow * 1.2f);
				else
					ANM_Grow(j, s->WD->ExpGrow);
				if (s->WD->BmSound != -1)
					SOUND_PlaySample(s->WD->BmSound);
				if (s->WD->Boom > 0)
				{
					ANM_AddFlags(j, ANM_FLAG_NO_DEPTH);
				}
				WPN_RemoveShot(s);
				i--;
			}
		}
	}

	void WPN_Boom(d6SHOT_s *s, Player *playerThatWasHit)
	{
		int killedPlayers = 0, initialAuthorKills = s->Author->Person().Kills();
		bool killedSelf = false;

		int dosah = s->GetExplosionRange();
		int sila = s->GetExplosionPower();

		float X = (s->O == Orientation::Left) ? (s->X + 0.32f) : (s->X + 0.67f);
		float Y = s->Y - 0.17f;

		if (s->GN != D6_COL_WPN_SHT)
			FIRE_Check(X, Y, dosah);

		for (Size i = 0; i < d6Playing; i++)
		{
			Player *player = d6Player[i];

			if (player == playerThatWasHit)
			{
				if (s->GN == D6_COL_WPN_SHT)
				{
					player->UseTemporarySkin(*brownSkin);
				}
				else
				{
					if (player->Hit((float)sila, s, true))
					{
						killedPlayers++;
					}
				}
			}
			else
			{
				float vzd = (float)sqrt(D6_SQR(player->State.X + 0.5f - X) + D6_SQR(player->State.Y - 0.5f - Y));

				if (vzd < (float)dosah)
				{
					if (s->GN == D6_COL_WPN_SHT)
					{
						player->UseTemporarySkin(*brownSkin);
					}
					else
					{
						if (player->Hit(((dosah - vzd) * sila) / dosah, s, false))
						{
							killedPlayers++;

							if (player == s->Author)
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
			s->Author->Person().SetKills(initialAuthorKills - killedPlayers);
		}
	}

	/*
	==================================================
	WPN_GetRandomWeapon

	Vybere a vraci cislo nahodne zbrane (musi byt enabled)
	==================================================
	*/
	int WPN_GetRandomWeapon(void)
	{
		int     i, r, ebl = 0;

		for (i = 0; i < D6_WEAPONS; i++)
			if (d6WpnEnabled[i])
				ebl++;

		r = rand() % ebl;
		for (i = 0; i < D6_WEAPONS; i++)
			if (d6WpnEnabled[i])
				if (r-- == 0)
					break;

		return i;
	}

	/*
	==================================================
	d6SHOT_s
	==================================================
	*/
	int d6SHOT_s::GetExplosionPower()
	{
		int coef = Author->HasPowerfulShots() ? 2 : 1;
		return coef * WD->Power;
	}

	int d6SHOT_s::GetExplosionRange()
	{
		int coef = Author->HasPowerfulShots() ? 2 : 1;
		return coef * WD->Boom;
	}
}