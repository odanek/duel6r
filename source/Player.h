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
#include <string>
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
#include "Water.h"
#include "Rectangle.h"
#include "Defines.h"
#include "Level.h"
#include "PlayerEventListener.h"

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
    class PlayerEventListener;

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
			FlagMoveLeft = 0x40,
			FlagMoveRight = 0x80,
			FlagMoveUp = 0x100,
			FlagHasGun = 0x200,
			FlagGhost = 0x400
		};

		struct WaterState
		{
			bool headUnderWater;
			bool feetInWater;
			Water::Type type;
		};

		struct PlayerState
		{
			Uint32 flags;
			Float32 velocity;
			Orientation orientation;
			Float32 jumpPhase;
			Vector position;
			Float32 life;
			Float32 air; 
			Int32 ammo;
			Int32 bonus;
            Int32 roundKills;
			Float32 timeToReload;
			Float32 bonusRemainingTime;
			Float32 bonusDuration;
			Float32 hpBarDuration;
			Float32 timeSinceHit;
			Float32 tempSkinDuration;
			const Weapon *weapon;
			const Elevator* elevator;
		};

	private:
		Person& person;
		const PlayerSkin& skin;
		mycam_c camera;
		CameraPosition cameraPos;
		const PlayerSounds& sounds;
		const PlayerControls& controls;
		PlayerView view;
		WaterState water;
		SpriteList::Iterator sprite;
		SpriteList::Iterator gunSprite;
		PlayerState state;
		Int32 infoBarPosition[2];
		std::string teamName;
        PlayerEventListener* eventListener;
		World* world; // TODO: Remove

	public:
		Player(Person& person, const PlayerSkin& skin, const PlayerSounds& sounds, const PlayerControls& controls);
		~Player();

		bool is(const Player& player) const
		{
			return (this == &player);
		}

		void startGame(World& world, Int32 startBlockX, Int32 startBlockY, Int32 ammo);
		void setView(const PlayerView& view);
		void update(World& world, ScreenMode screenMode, Float32 elapsedTime);
		void prepareCam(const Video& video, ScreenMode screenMode, Int32 zoom, Int32 levelSizeX, Int32 levelSizeY);
		bool hit(Float32 pw); // Returns true if the shot caused the player to die
		bool hitByShot(Float32 pw, Shot& s, bool directHit);
        void processKills(Shot &shot, std::vector<Player *> killedPlayers);
        void processHits(Shot &shot, std::vector<Player *> hittedPlayers);

        void setEventListener(PlayerEventListener& listener)
        {
            eventListener = &listener;
        }

		bool hasAnyTeam() const;
		bool hasTeam(std::string team) const;
		void setTeam(std::string team);
		void unsetTeam();
		std::string getTeam() const;


		void setOverlay(Color overlay);
		void unsetOverlay();

		const Vector& getPosition() const
		{
			return state.position;
		}

		Vector getDimensions() const
		{
			if (isKneeling())
			{
				return Vector(1.0f, 0.8f);
			}
			else if (isLying())
			{
				return Vector(1.0f, 0.4f);
			}
			return Vector(1.0f, 1.0f);
		}

		Vector getCentre() const
		{
			return getCollisionRect().getCentre();
		}

		Rectangle getCollisionRect() const
		{
			return Rectangle::fromCornerAndSize(getPosition(), getDimensions());
		}

		Vector getSpritePosition() const
		{
			return getPosition();
		}

		Float32 getGunVerticalPosition() const
		{
			return getPosition().y + getDimensions().y - 0.5f;
		}

		Vector getGunSpritePosition() const
		{
			return isKneeling() ? getSpritePosition() - Vector(0.0f, 0.15f) : getSpritePosition();
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

		void clearBonus()
		{
			setAlpha(1.0f);
			state.bonus = -1;
			state.bonusRemainingTime = 0;
			state.bonusDuration = 0;
		}

		Float32 getBonusRemainingTime() const
		{
			return state.bonusRemainingTime;
		}

		Float32 getBonusDuration() const
		{
			return state.bonusDuration;
		}

		Float32 getHPBarDuration() const
		{
			return state.hpBarDuration;
		}

		void showHPBar()
		{
			state.hpBarDuration = D6_PLAYER_HPBAR;
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

		Player &addLife(Float32 life, bool showHpBar = true);

		Player& setFullLife()
		{
			addLife(D6_MAX_LIFE);
			return *this;
		}

		Player& pickAmmo(Int32 ammo)
		{
			state.ammo += ammo;
			return *this;
		}

		void useTemporarySkin(PlayerSkin& tempSkin);
		Player& pickWeapon(const Weapon &weapon, Int32 bullets);

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

		bool isGhost() const
		{
			return hasFlag(FlagGhost);
		}

		bool isUnderWater() const
		{
			return water.headUnderWater;
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

		const WaterState& getWaterState() const
		{
			return water;
		}

		void playSound(PlayerSounds::Type type) const
		{
			sounds.getRandomSample(type).play();
		}

	private:
		void makeMove(const Level& level, Float32 elapsedTime);
		void moveHorizontal(const Level& level, Float32 elapsedTime, Float32 speed);
		void moveVertical(const Level& level, Float32 elapsedTime, Float32 speed);
		void checkKeys();
		void checkWater(World& world, Float32 elapsedTime);
		void fall();
		void pick();
		void shoot();
		void setAnm();
		void updateCam(Int32 levelSizeX, Int32 levelSizeY);
		void switchToOriginalSkin();
		void findStartingPosition();
		void dropWeapon(const Level& level);
		Float32 getSpeed() const;

		void checkMoveUp(const Level& level);
		void checkMoveDown(const Level& level);
		void checkFall(const Level& level);
		void checkHorizontalMove(const Level& level);
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