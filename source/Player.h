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
#include "SpriteList.h"
#include "Person.h"
#include "PlayerSkin.h"
#include "PlayerControls.h"
#include "Orientation.h"
#include "ScreenMode.h"
#include "Bonus.h"

#define D6_PLAYER_MAX_SPEED     32.0f
#define D6_PLAYER_ACCEL         0.001f
#define D6_PLAYER_JPHASE_SPEED  122
#define D6_PLAYER_JUMP_SPEED    4.88f

#define D6_CAM_TOLPER_X         30
#define D6_CAM_TOLPER_Y         30

#define D6_MAX_LIFE				100
#define D6_MAX_AIR				200

namespace Duel6
{
	// Forward declarations
	class Elevator; 
	class Shot;
	struct Weapon;

	struct PlayerView
	{
		int     X;
		int     Y;
		int     Width;
		int     Height;
	};

	struct CameraPosition
	{
		float           Left;
		float           Right;
		float           Up;
		float           Down;
		vec3_c<float>   Pos;
		float           TolX;
		float           TolY;
	};

	class Player
	{
	private:
		enum Flags
		{
			FlagDead = 0x01,
			FlagPick = 0x02,
			FlagKnee = 0x04,
			FlagLying = 0x08,
			FlagUnderWater = 0x10
		};

		struct PlayerState
		{
			Uint32 flags;
			SpriteIterator sprite;    // Player sprite
			SpriteIterator gunSprite;
			const Weapon *weapon;
			Float32 velocity;         // Move velocity
			Orientation orientation;
			Float32 jumpPhase;
			Float32 x;
			Float32 y;
			Int32 IBP[2];     // Info bar position
			Float32 life;
			Float32 air; 
			Int32 ammo;
			Int32 bonus;
			Float32 reloadInterval;
			Float32 bonusDuration;
			Float32 tempSkinDuration;
			bool feetInWater;
			const Elevator* elevator;
		};

	private:
		Person& person;
		std::shared_ptr<PlayerSkin> skin;
		mycam_c camera;
		CameraPosition cameraPos;
		const PlayerControls& controls;
		PlayerState State;
		PlayerView view;

	public:
		Player(Person& person, PlayerSkin* skin, const PlayerControls& controls);
		~Player();

		bool is(const Player& player) const
		{
			return (this == &player);
		}

		void prepareForGame();

		void setView(int x, int y, int w, int h);
		void update(ScreenMode screenMode, Float32 elapsedTime);
		void prepareCam(ScreenMode screenMode);
		bool hit(Float32 pw); // Returns true if the shot caused the player to die
		bool hitByShot(Float32 pw, Shot& s, bool directHit);

		Float32 getX() const
		{
			return State.x;
		}

		Float32 getY() const
		{
			return State.y;
		}

		Float32 getWidth() const
		{
			return 1.0f;
		}

		Float32 getHeight() const
		{
			return 1.0f;
		}

		const PlayerView& getView() const
		{
			return view;
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
			return State.ammo;
		}

		Orientation getOrientation() const
		{
			return State.orientation;
		}

		Player& setAlpha(Float32 alpha)
		{
			State.sprite->setAlpha(alpha);
			State.gunSprite->setAlpha(alpha);
			return *this;
		}

		Player& setBonus(Size type, Int32 duration)
		{
			State.bonus = type;
			State.bonusDuration = Float32(duration);
			return *this;
		}

		Float32 getLife() const
		{
			return State.life;
		}

		Float32 getAir() const
		{
			return State.air;
		}

		Int32 getBonus() const
		{
			return State.bonus;
		}

		Float32 getBonusDuration() const
		{
			return State.bonusDuration;
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
			State.life = D6_MAX_LIFE;
			return *this;
		}

		Player& pickAmmo(Int32 ammo)
		{
			State.ammo += ammo;
			return *this;
		}

		void useTemporarySkin(PlayerSkin& skin);
		Player& pickWeapon(const Weapon& weapon, Int32 bulelts);		

		bool isReloading()
		{
			return State.reloadInterval > 0;
		}

		bool isKneeling() const
		{
			return hasFlag(FlagKnee);
		}
		
		bool isLying() const
		{
			return hasFlag(FlagLying);
		}

		bool isDead() const
		{
			return hasFlag(FlagDead);
		}

		bool isUnderWater() const
		{
			return hasFlag(FlagUnderWater);
		}

		bool isPickingGun() const
		{
			return hasFlag(FlagPick);
		}

		bool isInGame() const
		{
			return !isDead() || isLying();
		}

		bool isInvulnerable() const
		{
			return (getBonus() == D6_BONUS_INVUL);
		}

		bool isRising() const
		{
			return (State.jumpPhase >= 90.0f && State.jumpPhase < 180.0f);
		}

		bool isFalling() const
		{
			return (State.jumpPhase >= 180.0f && State.jumpPhase <= 270.0f);
		}

		bool isOnGround() const
		{
			return (State.jumpPhase == 0.0f);
		}

		bool isMoving() const
		{
			return (State.velocity != 0.0f);
		}

		bool hasPowerfulShots() const
		{
			return (getBonus() == D6_BONUS_SHOTP);
		}

		bool hasFastReload() const
		{
			return (getBonus() == D6_BONUS_SHOTS);
		}

		bool isOnElevator() const
		{
			return (State.elevator != nullptr);
		}

	private:
		void moveLeft(Float32 elapsedTime);
		void moveRight(Float32 elapsedTime);
		void makeMove(Float32 elapsedTime);
		void checkKeys(Float32 elapsedTime);
		void checkWater(Float32 elapsedTime);
		void jump();
		void fall();
		void pick();
		void shoot();
		void setAnm();
		void updateCam();
		void switchToOriginalSkin();
		void findStartingPosition();
		void dropWeapon();
		Float32 getSpeed() const;

		void checkMoveUp();
		void checkMoveDown();
		void checkFall();
		void checkMoveAside();
		void checkElevator();

		bool hasFlag(Uint32 flag) const
		{
			return (State.flags & flag) == flag;
		}

		void setFlag(Uint32 flag)
		{
			State.flags |= flag;
		}

		void unsetFlag(Uint32 flag)
		{
			State.flags &= ~flag;
		}
	};

	//////////////////////////////////////////////////////////////////////
	//                          player.cpp                              //
	//////////////////////////////////////////////////////////////////////

	void    PLAYER_PrepareViews(ScreenMode screenMode);
}

#endif