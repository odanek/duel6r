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

namespace Duel6
{
	struct d6BONUS_s
	{
		float   X;
		float   Y;
		int     Texture;
		int     Type;
		int     Kind;
		int     Bull;
		bool    Used;
	};

	extern myUINT       *d6WpnTexture;
	extern Int16        d6WpnAnm[D6_WEAPONS][16];
	static d6BONUS_s    d6Bonus[D6_BONUS_MAX];
	int                 d6BonusArt[D6_BONUS_COUNT] = { 19, 20, 21, 22, 23, 24, 25, 46, 53, 26 };  // Otaznik musi byt posledni
	bool                d6WpnEnabled[D6_WEAPONS];

	void BONUS_Init(int(*bonus)[3])
	{
		int     i;

		for (i = 0; i < D6_BONUS_MAX; i++)
			if (bonus[i][0] == -1)
				d6Bonus[i].Used = false;
			else
			{
				d6Bonus[i].Used = true;
				d6Bonus[i].Type = bonus[i][0];
				d6Bonus[i].Texture = d6World.Anm.TexGlNum[d6Bonus[i].Type];
				d6Bonus[i].Kind = 0;
				d6Bonus[i].X = (float)bonus[i][1];
				d6Bonus[i].Y = (float)(d6World.Level.SizeY - bonus[i][2]);
			}
	}

	void BONUS_DrawAll(void)
	{
		d6BONUS_s   *b;
		int         i;

		glEnable(GL_ALPHA_TEST);

		for (i = 0; i < D6_BONUS_MAX; i++)
			if (d6Bonus[i].Used)
			{
				b = &d6Bonus[i];

				glBindTexture(GL_TEXTURE_2D, b->Texture);
				glBegin(GL_QUADS);
				glTexCoord2f(0.1f, 0.1f); glVertex3f(b->X, b->Y, 0.5f);
				glTexCoord2f(0.9f, 0.1f); glVertex3f(b->X + 1.0f, b->Y, 0.5f);
				glTexCoord2f(0.9f, 0.9f); glVertex3f(b->X + 1.0f, b->Y - 1.0f, 0.5f);
				glTexCoord2f(0.1f, 0.9f); glVertex3f(b->X, b->Y - 1.0f, 0.5f);
				glEnd();
			}

		glDisable(GL_ALPHA_TEST);
	}

	void BONUS_AddNew(float elapsedTime)
	{
		int     x, y, i, k;

		k = rand() % 2;

		if (rand() % int(2.4f / elapsedTime))
			return;

		x = rand() % d6World.Level.SizeX;
		y = rand() % d6World.Level.SizeY;

		if (k && D6_BlockZ(x, y + 1) != D6_ANM_F_BLOCK)
			return;

		if (D6_BlockZ(x, y) != D6_ANM_F_BLOCK)
		{
			for (i = 0; i < D6_BONUS_MAX; i++)
				if (!d6Bonus[i].Used)
					break;

			if (i >= D6_BONUS_MAX)
				return;

			d6Bonus[i].Used = true;
			if (k)
			{
				d6Bonus[i].Type = WPN_GetRandomWeapon();
				d6Bonus[i].Texture = d6WpnTexture[d6WpnAnm[d6Bonus[i].Type][12]];
			}
			else
			{
				d6Bonus[i].Type = d6BonusArt[rand() % D6_BONUS_COUNT];
				d6Bonus[i].Texture = d6World.Anm.TexGlNum[d6Bonus[i].Type];
			}
			d6Bonus[i].Kind = k;
			d6Bonus[i].Bull = rand() % 10 + 10;
			d6Bonus[i].X = (float)x;
			d6Bonus[i].Y = (float)(d6World.Level.SizeY - y);
		}
	}

	void BONUS_AddDeadManGun(int x, int y, Player& player)
	{
		d6PLSTATE_s *state = &player.State;
		int     i;

		for (i = 0; i < D6_BONUS_MAX; i++)
			if (!d6Bonus[i].Used)
				break;

		if (i >= D6_BONUS_MAX)
			return;

		d6Bonus[i].Used = true;
		d6Bonus[i].Type = state->GN;
		d6Bonus[i].Texture = d6WpnTexture[d6WpnAnm[state->GN][12]];
		d6Bonus[i].Kind = 1;
		d6Bonus[i].Bull = state->Ammo;
		d6Bonus[i].X = (float)x;
		d6Bonus[i].Y = (float)(d6World.Level.SizeY - y);
	}

	void BONUS_Check(Player& player)
	{
		d6PLSTATE_s *s = &player.State;
		d6BONUS_s   *b;
		int         i, t, h;

		for (i = 0; i < D6_BONUS_MAX; i++)
			if (d6Bonus[i].Used && !d6Bonus[i].Kind)
			{
				b = &d6Bonus[i];

				if (s->Bonus == D6_BONUS_INVUL && b->Type == D6_BONUS_LIFEM)
					continue;

				if (fabs(b->X - s->X) < 0.5f && fabs(b->Y - s->Y) < 0.5f)
				{
					t = 13 + rand() % 17;
					h = (D6_MAX_LIFE / 7) + rand() % (D6_MAX_LIFE / 2);

					if (b->Type == D6_BONUS_GUESS)
						b->Type = d6BonusArt[rand() % (D6_BONUS_COUNT - 1)];

					switch (b->Type)
					{
					case D6_BONUS_INVIS:
						s->Bonus = b->Type;
						s->BD = (float)t;
						d6MessageQueue.Add(player, MY_L("APP00094|Neviditelnost na %d sekund"), t);
						s->A->SetAlpha(0.2f);
						s->GA->SetAlpha(0.2f);
						break;

					case D6_BONUS_SPEED:
						s->Bonus = b->Type;
						s->BD = (float)t;
						d6MessageQueue.Add(player, MY_L("APP00093|Rychly pohyb na %d sekund"), t);
						s->A->SetAlpha(1);
						s->GA->SetAlpha(1);
						break;

					case D6_BONUS_LIFEP:
						s->Life += h;
						d6MessageQueue.Add(player, MY_L("APP00012|Zivot +%d"), h);
						break;

					case D6_BONUS_LIFEM:
						player.Hit(float(h), NULL, false);
						d6MessageQueue.Add(player, MY_L("APP00013|Zivot -%d"), h);
						break;

					case D6_BONUS_LIFEF:
						s->Life = D6_MAX_LIFE;
						d6MessageQueue.Add(player, MY_L("APP00014|Plny zivot"), h);
						break;

					case D6_BONUS_SHOTS:
						s->Bonus = b->Type;
						s->BD = (float)t;
						d6MessageQueue.Add(player, MY_L("APP00015|Rychle nabiti na %d sekund"), t);
						s->A->SetAlpha(1);
						s->GA->SetAlpha(1);
						break;

					case D6_BONUS_SHOTP:
						s->Bonus = b->Type;
						s->BD = (float)t;
						d6MessageQueue.Add(player, MY_L("APP00016|Silne strely na %d sekund"), t);
						s->A->SetAlpha(1);
						s->GA->SetAlpha(1);
						break;

					case D6_BONUS_INVUL:
						s->Bonus = b->Type;
						s->BD = (float)t;
						d6MessageQueue.Add(player, MY_L("APP00017|Nesmrtelnost na %d sekund"), t);
						s->A->SetAlpha(1);
						s->GA->SetAlpha(1);
						break;

					case D6_BONUS_BULLT:
						h = 5 + rand() % 12;
						s->Ammo += h;
						d6MessageQueue.Add(player, MY_L("APP00018|Naboje +%d"), h);
						break;
					}

					b->Used = false;
					SOUND_PlaySample(D6_SND_BNPICK);
					if (s->Life > D6_MAX_LIFE)
					{
						s->Life = D6_MAX_LIFE;
					}
				}
			}
	}

	void BONUS_Pick(Player& player)
	{
		d6PLSTATE_s *s = &player.State;
		d6BONUS_s   *b;
		int         i, l, t;

		for (i = 0; i < D6_BONUS_MAX; i++)
			if (d6Bonus[i].Used && d6Bonus[i].Kind)
			{
				b = &d6Bonus[i];

				if (fabs(b->X - player.X()) < 0.5f && fabs(b->Y - player.Y()) < 0.5f)
				{
					l = b->Bull;
					t = b->Type;
					if (s->Ammo)
					{
						b->Bull = s->Ammo;
						b->Type = s->GN;
						b->Texture = d6WpnTexture[d6WpnAnm[b->Type][12]];
					}
					else
						b->Used = false;

					s->Ammo = l;
					s->GN = t;
					s->Flags |= D6_FLAG_PICK;
					
					s->GA->SetAnimation(d6WpnAnm[s->GN])
						.SetFrame(6)
						.SetDraw(false);
					s->A->SetFrame(0);

					d6MessageQueue.Add(player, MY_L("APP00019|Sebral jsi zbran %s"), MY_L(d6WpnDef[s->GN].Name));
					return;
				}
			}
	}
}