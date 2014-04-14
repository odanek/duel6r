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

#include "mylib/mycam.h"
#include "Person.h"
#include "PlayerSkin.h"
#include "Orientation.h"
#include "ScreenMode.h" // TODO: Remove

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
	class d6SHOT_s; // Forward declaration
	struct d6LEVEL; // Forward declaration

	struct d6VIEW_s
	{
		int     X;
		int     Y;
		int     Width;
		int     Height;
	};

	struct d6KEYBOARD_s
	{
		int     Left;
		int     Right;
		int     Up;
		int     Down;
		int     Shoot;
		int     Pick;
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

	class d6PLSTATE_s
	{
	public:
		int         Flags;      // Flags
		int         A;          // Animation number
		int         GA;         // Gun animation
		int         GN;         // Gun number
		float       Speed;      // Speed of movement
		Orientation O;          // Orientation
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
		float       SD;         // Temporary skin duration
		bool        InWater;    // Player is in the water (feet)
	};

	class Player
	{
	private:
		PlayerSkin *m_skin;     // Player skin
		Duel6::Person *m_person;       // Person playing the player

	private:
		void Left(float elapsedTime);
		void Right(float elapsedTime);
		void MakeMove(float elapsedTime);
		void CheckKeys(float elapsedTime);
		void CheckWater(const d6LEVEL& level, float elapsedTime);

	public:
		mycam_c         *Camera;
		d6CAMPOS_s      CamPos;
		d6VIEW_s        View;
		d6KEYBOARD_s    *Keys;
		d6PLSTATE_s     State;

		Player(size_t index);
		~Player(void);

		Player& SetSkin(const PlayerSkinColors& skinColors);
		void PrepareForGame();

		void SetView(int x, int y, int w, int h);
		void Jump();
		void Fall();
		void Pick();
		void SetAnm();
		void Update(float elapsedTime);
		void PrepareCam(ScreenMode screenMode);
		void UpdateCam();
		void SetControls(int n);
		bool Hit(float pw, d6SHOT_s *s, bool hit); // Returns true if the shot caused the player to die

		float X() const;
		float Y() const;
		float Width() const;
		float Height() const;

		Duel6::Person& Person()
		{
			return *m_person;
		}

		const Duel6::Person& Person() const
		{
			return *m_person;
		}

		Player& SetPerson(Duel6::Person& person)
		{
			m_person = &person;
			return *this;
		}

		bool HasPowerfulShots() const;
		bool IsKneeling() const;
		bool IsLying() const;
		bool IsDead() const;
		bool IsInvulnerable() const;

		void UseTemporarySkin(PlayerSkin& skin);
		void SwitchToOriginalSkin();

	private:
		void FreeSkin();
	};

	//////////////////////////////////////////////////////////////////////
	//                          player.cpp                              //
	//////////////////////////////////////////////////////////////////////

	void    PLAYER_PrepareViews(ScreenMode screenMode);
}

#endif