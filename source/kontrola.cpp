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
#include "Weapon.h"

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
		d6UpY = l->SizeY - (int)(player.getY() - 0.06) - 1;
		d6DownY = l->SizeY - (int)(player.getY() - 1.0f) - 1;
		d6Down2 = l->SizeY - (int)(player.getY() - 1.001f) - 1;
		d6LeftX = (int)(player.getX() + 0.1f);
		d6RightX = (int)(player.getX() + 0.9f);

		switch (c)
		{
		case 1: KONTR_Kontr1(player); break;
		case 2: KONTR_Kontr2(player); break;
		case 3: KONTR_Kontr3(player); break;
		case 4: KONTR_Kontr4(player); break;
		}
	}

	bool KONTR_CanJump(Player* p)
	{
		d6UpY = l->SizeY - (int)p->getY() - 1;
		d6LeftX = (int)(p->getX() + 0.1f);
		d6RightX = (int)(p->getX() + 0.9f);

		if (KONTR_Bck(d6LeftX, d6UpY) || KONTR_Bck(d6RightX, d6UpY))
			return false;
		return true;
	}

	static bool KONTR_ShotPlayer(Shot& shot)
	{
		Float32 X = (shot.getOrientation() == Orientation::Left) ? shot.getX() : shot.getX() + 0.35f;

		for (Player& player : d6Players)
		{
			if (player.getBonus() == D6_BONUS_INVIS || player.is(shot.getPlayer()))
			{
				continue;
			}
			if (player.isDead() && !player.isLying())
			{
				continue;
			}

			Float32  ad = player.isKneeling() ? 0.2f : (player.isLying() ? 0.6f : 0.0f);

			if (X > player.getX() + 1.0f || X + 0.65f < player.getX() ||
				shot.getY() < player.getY() - 1.0f || shot.getY() - 0.35f > player.getY() - ad)
				continue;

			WPN_Boom(shot, &player);
			return true;
		}

		return false;
	}

	bool KONTR_Shot(Shot& s)
	{
		if (KONTR_ShotPlayer(s))
			return true;

		d6UpY = l->SizeY - (int)(s.getY()) - 1;
		d6DownY = l->SizeY - (int)(s.getY() - 0.35f) - 1;
		if (s.getOrientation() == Orientation::Left)
		{
			d6LeftX = (int)(s.getX());
			d6RightX = (int)(s.getX() + 0.65f);
		}
		else
		{
			d6LeftX = (int)(s.getX() + 0.35f);
			d6RightX = (int)(s.getX() + 1.0f);
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