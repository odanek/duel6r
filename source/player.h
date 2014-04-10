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

#ifndef DUEL6_PLAYER_H
#define DUEL6_PLAYER_H

#include "coltexture.h"

#define D6_PLAYER_MAX_SPEED     32
#define D6_PLAYER_ACCEL         0.001f
#define D6_PLAYER_JUMP_SPEED    0.08f

#define D6_CAM_TOLPER_X         30
#define D6_CAM_TOLPER_Y         30

#define D6_FLAG_DEAD            0x01
#define D6_FLAG_PICK            0x02
#define D6_FLAG_KNEE            0x04
#define D6_FLAG_REQ_LEFT        0x08
#define D6_FLAG_REQ_RIGHT       0x10
#define D6_FLAG_LYING           0x20
#define D6_FLAG_INWATER         0x40

namespace Duel6
{
	struct d6PHIST_s
	{
		char    Name[20];
		int     Shots;
		int     Hits;
		int     Kills;
		int     Wins;
		int     Games;
	};

	struct d6CAMPOS_s
	{
		float           Left;
		float           Right;
		float           Up;
		float           Down;
		vec3_c<float>   Pos;
		float           TolX;
		float           TolY;
	};

	struct d6PLSTATE_s
	{
		int         Flags;      // Flags
		int         I;          // Offset in d6Player array
		int         A;          // Animation number
		int         GA;         // Gun animation
		int         GN;         // Gun number
		float       Speed;      // Speed of movement
		int         O;          // Orientation (0 = left, 1 = right)
		float       J;          // Jump phase
		float       X;          // X position
		float       Y;          // Y position
		float       SI;         // Shot interval
		int         IBP[2];     // Info bar position
		float       Life;       // Life
		float       Air;        // Air
		int         Ammo;       // Number of bullets
		int         Elev;       // Standing on elevator
		int         Bonus;      // Bonus
		float       BD;         // Bonus duration
		float       SD;         // Sht duration
		bool        InWater;    // Player is in the water
		d6PHIST_s   *PH;        // Player history
	};

	class Player
	{
	private:
		static int      next_id;

	public:
		myUINT          *Texture;
		int             Textures;
		mycam_c         *Camera;
		d6CAMPOS_s      CamPos;
		d6VIEW_s        View;
		d6KEYBOARD_s    *Keys;
		d6PLSTATE_s     State;

		Player(size_t index);
		~Player(void);

		void    Skin(void);
		void    SetView(int x, int y, int w, int h);
		void    Left(void);
		void    Right(void);
		void    Jump(void);
		void    Fall(void);
		void    Pick(void);
		void    MakeMove(void);
		void    SetAnm(void);
		void    CheckKeys(void);
		void    PrepareCam(void);
		void    UpdateCam(void);
		void    SetControls(int n);
		bool    Hit(float pw, d6SHOT_s *s, bool hit); // Returns true if the shot caused the player to die

		int GetIndex();
		float GetX();
		float GetY();

		bool HasPowerfulShots();
		bool IsKneeling();
		bool IsLying();
		bool IsDead();

		void SetColorTexture(ColorTexture& texture);
	};

	extern Player   *d6Player[D6_MAX_PLAYERS];
	extern int       d6PlayerSkin[D6_MAX_PLAYERS][8];

	//////////////////////////////////////////////////////////////////////
	//                          player.cpp                              //
	//////////////////////////////////////////////////////////////////////

	void    PLAYER_PrepareViews(void);
	void    PLAYER_UpdateAll(void);
	void    PLAYER_PrepareAll(void);
}

#endif