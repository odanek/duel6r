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
#include "PlayerSounds.h"
#include "Orientation.h"
#include "ScreenMode.h"
#include "Bonus.h"
#include "Sound.h"
#include "Video.h"

#define D6_MAX_LIFE				100.0f
#define D6_MAX_AIR				200.0f

namespace Duel6
{
	// Forward declarations
	class Elevator; 
	class Shot;
	struct Weapon;
	class World;
	class InfoMessageQueue;

	struct PlayerView
	{
	private:
		Int32 x;
		Int32 y;
		Int32 width;
		Int32 height;

	public:
		PlayerView()
		{}

		PlayerView(Int32 x, Int32 y, Int32 width, Int32 height)
			: x(x), y(y), width(width), height(height)
		{}

		Int32 getX() const
		{
			return x;
		}

		Int32 getY() const
		{
			return y;
		}

		Int32 getWidth() const
		{
			return width;
		}

		Int32 getHeight() const
		{
			return height;
		}
	};

	struct CameraPosition
	{
		Float32 Left;
		Float32 Right;
		Float32 Up;
		Float32 Down;
		vec3_c<Float32> Pos;
		Float32 TolX;
		Float32 TolY;
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
			FlagHeadUnderWater = 0x10,
			FlagFeetInWater = 0x20,
			FlagMoveLeft = 0x40,
			FlagMoveRight = 0x80,
			FlagMoveUp = 0x100,
			FlagHasGun = 0x200
		};

		struct PlayerState
		{
			Uint32 flags;
			Float32 velocity;         // Move velocity
			Orientation orientation;
			Float32 jumpPhase;
			Float32 x;
			Float32 y;
			Float32 life;
			Float32 air; 
			Int32 ammo;
			Int32 bonus;
            Int32 roundKills;
			Float32 timeToReload;
			Float32 bonusRemainingTime;
			Float32 bonusDuration;
			Float32 tempSkinDuration;
			const Weapon *weapon;
			const Elevator* elevator;
		};

	private:
		const TextureManager& textureManager;
		SpriteList& spriteList;
		InfoMessageQueue& messageQueue;		

		Person& person;
		PlayerSkin skin;
		mycam_c camera;
		CameraPosition cameraPos;
		const PlayerSounds& sounds;
		const PlayerControls& controls;
		PlayerView view;
		SpriteIterator sprite;
		SpriteIterator gunSprite;
		PlayerState state;
		Int32 infoBarPosition[2];

		const Sound::Sample& waterSplashSample; // TODO: Remove

	public:
		Player(Person& person, PlayerSkin skin, const PlayerSounds& sounds, const PlayerControls& controls, 
			const TextureManager& textureManager, SpriteList& spriteList, InfoMessageQueue& messageQueue,
			const Sound::Sample& waterSplashSample);
		~Player();

		bool is(const Player& player) const
		{
			return (this == &player);
		}

		void startGame(Int32 startBlockX, Int32 startBlockY, Int32 ammo);
		void setView(const PlayerView& view);
		void update(const World& world, ScreenMode screenMode, Float32 elapsedTime);
		void prepareCam(const Video& video, ScreenMode screenMode, Int32 zoom, Int32 levelSizeX, Int32 levelSizeY);
		bool hit(Float32 pw); // Returns true if the shot caused the player to die
		bool hitByShot(Float32 pw, Shot& s, bool directHit);

		Float32 getX() const
		{
			return state.x;
		}

		Float32 getY() const
		{
			return state.y;
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
			return *state.weapon;
		}

		Int32 getAmmo() const
		{
			return state.ammo;
		}

		Int32 getRoundKills() const
		{
			return state.roundKills;
		}

		void addRoundKills(Int32 kills)
		{
			state.roundKills += kills;
		}

		Orientation getOrientation() const
		{
			return state.orientation;
		}

		Player& setAlpha(Float32 alpha)
		{
			sprite->setAlpha(alpha);
			gunSprite->setAlpha(alpha);
			return *this;
		}

		Player& setBonus(Size type, Int32 duration)
		{
			state.bonus = type;
			state.bonusRemainingTime = Float32(duration);
			state.bonusDuration = Float32(duration);
			return *this;
		}

		Float32 getLife() const
		{
			return state.life;
		}

		Float32 getAir() const
		{
			return state.air;
		}

		Float32 getReloadInterval() const;

		Float32 getReloadTime() const
		{
			return state.timeToReload;
		}

		Int32 getBonus() const
		{
			return state.bonus;
		}

		Float32 getBonusRemainingTime() const
		{
			return state.bonusRemainingTime;
		}

		Float32 getBonusDuration() const
		{
			return state.bonusDuration;
		}

		Player& setInfoBarPosition(Int32 x, Int32 y)
		{
			infoBarPosition[0] = x;
			infoBarPosition[1] = y;
			return *this;
		}

		const Int32* getInfoBarPosition() const
		{
			return infoBarPosition;
		}

		Player& adjustLife(Float32 life);

		Player& setFullLife()
		{
			state.life = D6_MAX_LIFE;
			return *this;
		}

		Player& pickAmmo(Int32 ammo)
		{
			state.ammo += ammo;
			return *this;
		}

		void useTemporarySkin(PlayerSkin& tempSkin);
		Player& pickWeapon(const Weapon& weapon, Int32 bulelts);		

		bool isReloading()
		{
			return state.timeToReload > 0;
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
			return hasFlag(FlagHeadUnderWater);
		}

		bool isPickingGun() const
		{
			return hasFlag(FlagPick);
		}

		bool hasGun() const
		{
			return hasFlag(FlagHasGun);
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
			return (state.jumpPhase >= 90.0f && state.jumpPhase < 180.0f);
		}

		bool isFalling() const
		{
			return (state.jumpPhase >= 180.0f && state.jumpPhase <= 270.0f);
		}

		bool isOnGround() const
		{
			return (state.jumpPhase == 0.0f);
		}

		bool isMoving() const
		{
			return (state.velocity != 0.0f);
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
			return (state.elevator != nullptr);
		}

		const PlayerSkin& getSkin() const
		{
			return skin;
		}		

	private:
		void makeMove(const World& world, Float32 elapsedTime);
		void moveHorizontal(const World& world, Float32 elapsedTime, Float32 speed);
		void moveVertical(const World& world, Float32 elapsedTime, Float32 speed);
		void checkKeys();
		void checkWater(const World& world, Float32 elapsedTime);
		void fall();
		void pick();
		void shoot();
		void setAnm();
		void updateCam(Int32 levelSizeX, Int32 levelSizeY);
		void switchToOriginalSkin();
		void findStartingPosition();
		void dropWeapon(const World& world);
		Float32 getSpeed() const;

		void checkMoveUp(const World& world);
		void checkMoveDown(const World& world);
		void checkFall(const World& world);
		void checkHorizontalMove(const World& world);
		void checkElevator();

		bool hasFlag(Uint32 flag) const
		{
			return (state.flags & flag) == flag;
		}

		void setFlag(Uint32 flag)
		{
			state.flags |= flag;
		}

		void unsetFlag(Uint32 flag)
		{
			state.flags &= ~flag;
		}
	};
}

#endif