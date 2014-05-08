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

#include <memory>
#include "mylib/mycam.h"
#include "Person.h"
#include "PlayerSkin.h"
#include "Orientation.h"
#include "ScreenMode.h"

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

#define D6_MAX_LIFE				100
#define D6_MAX_AIR				200

namespace Duel6
{
	class Shot; // Forward declaration
	struct Weapon;

	struct d6VIEW_s
	{
		int     X;
		int     Y;
		int     Width;
		int     Height;
	};

	struct PlayerControls
	{
		int Left;
		int Right;
		int Up;
		int Down;
		int Shoot;
		int Pick;
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
		Int32       Flags;      // Flags
		SpriteIterator A;       // Player sprite
		SpriteIterator GA;      // Gun sprite
		const Weapon *weapon;
		Float32     Speed;      // Speed of movement
		Orientation O;          // Orientation
		Float32     J;          // Jump phase
		Float32     X;          // X position
		Float32     Y;          // Y position
		Float32     SI;         // Shot interval
		Int32       IBP[2];     // Info bar position
		Float32     Life;       // Life
		Float32     Air;        // Air
		Int32       Ammo;       // Number of bullets
		Int32       Elev;       // Standing on elevator
		Int32       Bonus;      // Bonus
		Float32     BD;         // Bonus duration
		Float32     SD;         // Temporary skin duration
		bool        InWater;    // Player is in the water (feet)
	};

	class Player
	{
	private:
		Person& person;
		std::shared_ptr<PlayerSkin> skin;
		mycam_c camera;
		d6CAMPOS_s cameraPos;
		PlayerControls *controls;

	private:
		void moveLeft(float elapsedTime);
		void moveRight(float elapsedTime);
		void makeMove(float elapsedTime);
		void checkKeys(float elapsedTime);
		void checkWater(float elapsedTime);
		void jump();
		void fall();
		void pick();
		void setAnm();
		void updateCam();
		void switchToOriginalSkin();

	public:
		d6VIEW_s View;
		d6PLSTATE_s State;		

		Player(Person& person, PlayerSkin* skin, Size controls);
		~Player(void);

		bool is(const Player& player) const
		{
			return (this == &player);
		}

		void prepareForGame();

		void setView(int x, int y, int w, int h);
		void update(ScreenMode screenMode, float elapsedTime);
		void prepareCam(ScreenMode screenMode);
		bool hit(float pw, Shot *s, bool hit); // Returns true if the shot caused the player to die

		Float32 getX() const
		{
			return State.X;
		}

		Float32 getY() const
		{
			return State.Y;
		}

		Float32 getWidth() const
		{
			return 1.0f;
		}

		Float32 getHeight() const
		{
			return 1.0f;
		}

		Person& getPerson()
		{
			return person;
		}

		const Person& getPerson() const
		{
			return person;
		}

		const mycam_c& getCamera() const
		{
			return camera;
		}

		const Weapon& getWeapon() const
		{
			return *State.weapon;
		}

		Int32 getAmmo() const
		{
			return State.Ammo;
		}

		Orientation getOrientation() const
		{
			return State.O;
		}

		Player& setAlpha(Float32 alpha)
		{
			State.A->setAlpha(alpha);
			State.GA->setAlpha(alpha);
			return *this;
		}

		Player& setBonus(Size type, Int32 duration)
		{
			State.Bonus = type;
			State.BD = Float32(duration);
			return *this;
		}

		Float32 getLife() const
		{
			return State.Life;
		}

		Float32 getAir() const
		{
			return State.Air;
		}

		Int32 getBonus() const
		{
			return State.Bonus;
		}

		Float32 getBonusDuration() const
		{
			return State.BD;
		}

		Player& setInfoBarPosition(Int32 x, Int32 y)
		{
			State.IBP[0] = x;
			State.IBP[1] = y;
			return *this;
		}

		const Int32* getInfoBarPosition() const
		{
			return State.IBP;
		}

		Player& adjustLife(Float32 life);

		Player& setFullLife()
		{
			State.Life = D6_MAX_LIFE;
			return *this;
		}

		Player& adjustAmmo(Int32 ammo)
		{
			State.Ammo += ammo;
			return *this;
		}

		bool isReloading()
		{
			return State.SI > 0;
		}

		Player& pickWeapon(const Weapon& weapon, Int32 bulelts);

		bool hasPowerfulShots() const;
		bool isKneeling() const;
		bool isLying() const;
		bool isDead() const;
		bool isInvulnerable() const;

		void useTemporarySkin(PlayerSkin& skin);

	private:
		void dropWeapon();
	};

	//////////////////////////////////////////////////////////////////////
	//                          player.cpp                              //
	//////////////////////////////////////////////////////////////////////

	void    PLAYER_PrepareViews(ScreenMode screenMode);
}

#endif