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
	static void KONTR_Kontr1(Player& player, Int32 left, Int32 right, Int32 up)
	{
		if (d6World.isWall(left, up, true) || d6World.isWall(right, up, true))
		{
			player.State.Y = (float)(up) - 1.0f; // TODO: Coord
			player.State.J = 180;
		}
	}

	static void KONTR_Kontr2(Player& player, Int32 left, Int32 right, Int32 down)
	{
		if (d6World.isWall(left, down, true) || d6World.isWall(right, down, true))
		{
			player.State.Y = (float)(down) + 1.0001f; // TODO: Coord
			player.State.J = 0;
		}

		ELEV_CheckMan(player);
	}

	static void KONTR_Kontr3(Player& player, Int32 left, Int32 right, Int32 down)
	{
		ELEV_CheckMan(player);
		if (player.State.Elev != -1)
			return;

		if (!d6World.isWall(left, down, true) && !d6World.isWall(right, down, true))
			player.State.J = 180;
	}

	static void KONTR_Kontr4(Player& player, Int32 left, Int32 right, Int32 up, Int32 down)
	{
		if (player.State.Speed < 0)
		{
			if (d6World.isWall(left, up, true) || d6World.isWall(left, down, true))
				player.State.X = (float)left + 0.9001f; // TODO: Coord
		}
		else
		{
			if (d6World.isWall(right, up, true) || d6World.isWall(right, down, true))
				player.State.X = (float)right - 0.9001f; // TODO: Coord
		}
	}

	void KONTR_Kontr(Player& player, int c)
	{
		Int32 up = (int)(player.getY() + 0.94); // TODO: Coord
		Int32 down = (int)player.getY(); // TODO: Coord
		Int32 down2 = (int)(player.getY() - 0.001f); // TODO: Coord
		Int32 left = (int)(player.getX() + 0.1f); // TODO: Coord
		Int32 right = (int)(player.getX() + 0.9f); // TODO: Coord

		switch (c)
		{
		case 1: KONTR_Kontr1(player, left, right, up); break;
		case 2: KONTR_Kontr2(player, left, right, down); break;
		case 3: KONTR_Kontr3(player, left, right, down2); break;
		case 4: KONTR_Kontr4(player, left, right, up, down); break;
		}
	}

	bool KONTR_CanJump(Player* p)
	{
		Int32 up = (int)(p->getY() + 1.0f); // TODO: Coord
		Int32 left = (int)(p->getX() + 0.1f); // TODO: Coord
		Int32 right = (int)(p->getX() + 0.9f); // TODO: Coord

		if (d6World.isWall(left, up, true) || d6World.isWall(right, up, true))
			return false;
		return true;
	}

	static bool KONTR_ShotPlayer(Shot& shot)
	{
		Float32 X = (shot.getOrientation() == Orientation::Left) ? shot.getX() : shot.getX() + 0.35f; // TODO: Coord

		for (Player& player : d6Players)
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

			WPN_Boom(shot, &player);
			return true;
		}

		return false;
	}

	bool KONTR_Shot(Shot& s)
	{
		if (KONTR_ShotPlayer(s))
			return true;

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

		if (d6World.isWall(left, up, true) || d6World.isWall(left, down, true) ||
			d6World.isWall(right, up, true) || d6World.isWall(right, down, true))
		{
			WPN_Boom(s, NULL);
			return true;
		}

		return false;
	}
}