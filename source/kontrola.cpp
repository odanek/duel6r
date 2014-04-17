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

#include "project.h"

namespace Duel6
{
	static  int         d6UpY, d6DownY, d6Down2, d6LeftX, d6RightX, *z;
	static  d6LEVEL     *l;

	static bool KONTR_Bck(int x, int y)
	{
		if (x >= 0 && x < l->SizeX && y >= 0 && y < l->SizeY)
			return (z[l->Data[y * l->SizeX + x]] == D6_ANM_F_BLOCK);

		return true;
	}

	static void KONTR_Kontr1(Player& player)
	{
		if (KONTR_Bck(d6LeftX, d6UpY) || KONTR_Bck(d6RightX, d6UpY))
		{
			player.State.Y = (float)(l->SizeY - d6UpY) - 1.0f;
			player.State.J = 180;
		}
	}

	static void KONTR_Kontr2(Player& player)
	{
		if (KONTR_Bck(d6LeftX, d6DownY) || KONTR_Bck(d6RightX, d6DownY))
		{
			player.State.Y = (float)(l->SizeY - d6DownY) + 1.0001f;
			player.State.J = 0;
		}

		ELEV_CheckMan(player);
	}

	static void KONTR_Kontr3(Player& player)
	{
		ELEV_CheckMan(player);
		if (player.State.Elev != -1)
			return;

		if (!KONTR_Bck(d6LeftX, d6Down2) && !KONTR_Bck(d6RightX, d6Down2))
			player.State.J = 180;
	}

	static void KONTR_Kontr4(Player& player)
	{
		if (player.State.Speed < 0)
		{
			if (KONTR_Bck(d6LeftX, d6UpY) || KONTR_Bck(d6LeftX, d6DownY))
				player.State.X = (float)d6LeftX + 0.9001f;
		}
		else
		{
			if (KONTR_Bck(d6RightX, d6UpY) || KONTR_Bck(d6RightX, d6DownY))
				player.State.X = (float)d6RightX - 0.9001f;
		}
	}

	void KONTR_Init(void)
	{
		l = &d6World.Level;
		z = d6World.Anm.Znak;
	}

	void KONTR_Kontr(Player& player, int c)
	{
		d6UpY = l->SizeY - (int)(player.Y() - 0.06) - 1;
		d6DownY = l->SizeY - (int)(player.Y() - 1.0f) - 1;
		d6Down2 = l->SizeY - (int)(player.Y() - 1.001f) - 1;
		d6LeftX = (int)(player.X() + 0.1f);
		d6RightX = (int)(player.X() + 0.9f);

		switch (c)
		{
		case 1: KONTR_Kontr1(player); break;
		case 2: KONTR_Kontr2(player); break;
		case 3: KONTR_Kontr3(player); break;
		case 4: KONTR_Kontr4(player); break;
		}
	}

	bool KONTR_CanJump(Player *p)
	{
		d6UpY = l->SizeY - (int)p->State.Y - 1;
		d6LeftX = (int)(p->State.X + 0.1f);
		d6RightX = (int)(p->State.X + 0.9f);

		if (KONTR_Bck(d6LeftX, d6UpY) || KONTR_Bck(d6RightX, d6UpY))
			return false;
		return true;
	}

	static bool KONTR_ShotPlayer(d6SHOT_s& s)
	{
		Player  *p;
		float   X, ad;

		X = (s.O == Orientation::Left) ? s.X : s.X + 0.35f;

		for (Size i = 0; i < d6Playing; i++)
		{
			p = d6Player[i];

			if (p->State.Bonus == D6_BONUS_INVIS || p == s.Author)
				continue;

			if (p->IsKneeling())
				ad = 0.2f;
			else
				ad = 0.0f;

			if (p->IsLying())
				ad = 0.6f;
			else
				if (p->IsDead())
					continue;

			if (X > p->X() + 1.0f || X + 0.65f < p->X() ||
				s.Y < p->Y() - 1.0f || s.Y - 0.35f > p->Y() - ad)
				continue;

			WPN_Boom(s, p);
			return true;
		}

		return false;
	}

	bool KONTR_Shot(d6SHOT_s& s)
	{
		if (KONTR_ShotPlayer(s))
			return true;

		d6UpY = l->SizeY - (int)(s.Y) - 1;
		d6DownY = l->SizeY - (int)(s.Y - 0.35f) - 1;
		if (s.O == Orientation::Left)
		{
			d6LeftX = (int)(s.X);
			d6RightX = (int)(s.X + 0.65f);
		}
		else
		{
			d6LeftX = (int)(s.X + 0.35f);
			d6RightX = (int)(s.X + 1.0f);
		}

		if (KONTR_Bck(d6LeftX, d6UpY) || KONTR_Bck(d6LeftX, d6DownY) ||
			KONTR_Bck(d6RightX, d6UpY) || KONTR_Bck(d6RightX, d6DownY))
		{
			WPN_Boom(s, NULL);
			return true;
		}

		return false;
	}
}