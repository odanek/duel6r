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

#include <algorithm>
#include "Sound.h"
#include "TextureManager.h"
#include "InfoMessageQueue.h"
#include "World.h"
#include "Player.h"
#include "BonusList.h"
#include "Weapon.h"
#include "ElevatorList.h"
#include "Explosion.h"
#include "Math.h"
#include "Video.h"

namespace Duel6
{
	static Int16 noAnim[4] = { 0, 50, 0, -1 };
	static Int16 d6SAnim[22] = { 0, 200, 1, 30, 0, 30, 2, 30, 0, 90, 3, 15, 4, 15, 3, 15, 4, 15, 3, 30, -1, 0 };
	static Int16 d6WAnim[10] = { 6, 5, 5, 5, 6, 5, 7, 5, -1, 0 };
	static Int16 d6JAnim[4] = { 8, 50, -1, 0 };
	static Int16 d6DAnim[18] = { 9, 300, 10, 60, 9, 60, 11, 60, 9, 150, 12, 60, 9, 60, 12, 60, -1, 0 };
	static Int16 d6LAnim[16] = { 13, 10, 14, 10, 15, 10, 16, 10, 17, 10, 18, 10, 19, 10, -1, 0 };
	static Int16 d6NAnim[4] = { 25, 100, -1, 0 };
	static Int16 d6PAnim[] = { 0, 10, 20, 10, 21, 10, 22, 10, 23, 10, 24, 10, 23, 10, 22, 10, 21, 10, 0, 10, -1, 0 };

	Player::Player(Person& person, const PlayerSkin& skin, const PlayerSounds& sounds, const PlayerControls& controls, 
		SpriteList& spriteList, InfoMessageQueue& messageQueue, const Water::WaterSet& waterSet)
		: spriteList(spriteList), messageQueue(messageQueue), person(person), skin(skin), 
		sounds(sounds), controls(controls), waterSet(waterSet)
	{
		camera.rotate(180.0, 0.0, 0.0);
	}

	Player::~Player()
	{
	}
	
	void Player::startGame(Int32 startBlockX, Int32 startBlockY, Int32 ammo)
	{
		state.x = (Float32)startBlockX;
		state.y = (Float32)startBlockY + 0.0001f;

		Sprite manSprite(noAnim, skin.getTextures());
		manSprite.setPosition(getX(), getY(), 0.5f);
		sprite = spriteList.addSprite(manSprite);		

		state.weapon = &WPN_GetRandomWeapon();
		Sprite gunSprite(state.weapon->animation, state.weapon->textures.gun);
		gunSprite.setPosition(getX(), getY(), 0.5f)
			.setLooping(AnimationLooping::OnceAndStop)
			.setFrame(6);
		this->gunSprite = spriteList.addSprite(gunSprite);

		state.flags = FlagHasGun;
		state.velocity = 0.0f;
		state.orientation = Orientation::Left;
		state.jumpPhase = 0;
		state.timeToReload = 0;
		state.life = D6_MAX_LIFE;
		state.air = D6_MAX_AIR;
		state.ammo = ammo;
		state.elevator = nullptr;
		state.bonus = -1;
		state.bonusDuration = 0;
		state.bonusRemainingTime = 0;
		state.hpBarDuration = 0;
		state.tempSkinDuration = 0;
        state.roundKills = 0;
		this->view = view;

		getPerson().addGames(1);
	}

	void Player::setView(const PlayerView& view)
	{
		this->view = view;
	}

	void Player::moveHorizontal(const World& world, Float32 elapsedTime, Float32 speed)
	{
		if (hasFlag(FlagMoveLeft))
		{
			state.velocity = std::max(state.velocity - elapsedTime, -D6_PLAYER_MAX_SPEED);

			if (state.velocity < 0.0f)
			{
				state.orientation = Orientation::Left;
			}
		} 
		else if (state.velocity < 0.0f)
		{
			state.velocity = std::min(state.velocity + elapsedTime, 0.0f);
		}

		if (hasFlag(FlagMoveRight))
		{
			state.velocity = std::min(state.velocity + elapsedTime, D6_PLAYER_MAX_SPEED);

			if (state.velocity > 0.0f)
			{
				state.orientation = Orientation::Right;
			}
		} 
		else if (state.velocity > 0.0f)
		{
			state.velocity = std::max(state.velocity - elapsedTime, 0.0f);
		}

		if (isMoving())
		{
			state.x += state.velocity * D6_PLAYER_ACCEL * speed;
			checkHorizontalMove(world);
		}
	}

	void Player::moveVertical(const World& world, Float32 elapsedTime, Float32 speed)
	{
		if (isOnGround() && hasFlag(FlagMoveUp))
		{
			Int32 up = (Int32)(getY() + 1.0f); // TODO: Coord
			Int32 left = (Int32)(getX() + 0.1f); // TODO: Coord
			Int32 right = (Int32)(getX() + 0.9f); // TODO: Coord

			if (!world.isWall(left, up, true) && !world.isWall(right, up, true))
			{
				state.jumpPhase = 90.0f;
			}
		}

		if (!isOnGround())
		{
			state.jumpPhase += D6_PLAYER_JPHASE_SPEED * elapsedTime;
			if (state.jumpPhase > 270.0)
			{
				state.jumpPhase = 270.0f;
			}

			state.y += Math::fastSin(Int32(state.jumpPhase)) * D6_PLAYER_JUMP_SPEED * speed;

			if (isRising())
			{
				checkMoveUp(world);
			}
			else
			{
				checkMoveDown(world);
			}
		}
		else
		{
			checkFall(world);
		}
	}

	void Player::fall()
	{
		if (isOnGround() && !isMoving())
		{
			setFlag(FlagKnee);
		}
		else
		{
			if (isRising())
			{
				state.jumpPhase = 180.0f;
			}
		}
	}

	void Player::pick()
	{
		if (isOnGround() && !isMoving() && !isOnElevator())
		{
			BONUS_CheckPick(*this, messageQueue);
		}
	}

	void Player::shoot()
	{
		if (!getAmmo() || isReloading())
			return;

		state.timeToReload = getReloadInterval();
		state.ammo--;
		gunSprite->setFrame(0);
		getPerson().addShots(1);
		Orientation originalOrientation = getOrientation();

		WPN_AddShot(*this, spriteList, originalOrientation);

		if (getBonus() == D6_BONUS_SPLITFIRE && getAmmo() > 0)
		{
			state.ammo--;
			getPerson().addShots(1);
			Orientation secondaryOrientation = originalOrientation == Orientation::Left ? Orientation::Right : Orientation::Left;
			WPN_AddShot(*this, spriteList, secondaryOrientation);
		}
	}

	Player& Player::pickWeapon(const Weapon &weapon, Int32 bullets)
	{
		setFlag(FlagPick);
		state.weapon = &weapon;
		state.ammo = bullets;
		state.timeToReload = 0;
					
		gunSprite->setAnimation(weapon.animation).setTextures(weapon.textures.gun).setFrame(6);

		return *this;
	}

	void Player::makeMove(const World& world, Float32 elapsedTime)
	{
		Float32 speed = getSpeed() * elapsedTime;
		state.elevator = nullptr;

		moveVertical(world, elapsedTime, speed);
		moveHorizontal(world, elapsedTime, speed);

		if (isOnElevator())
		{
			state.x += state.elevator->getMoveX() * elapsedTime;
			state.y += state.elevator->getMoveY() * elapsedTime;
		}

		if (isPickingGun() && sprite->isFinished())
		{
			unsetFlag(FlagPick);
		}
		unsetFlag(FlagMoveLeft | FlagMoveRight | FlagMoveUp);
	}

	Float32 Player::getSpeed() const
	{
		Float32 spd = 1.0f;

		if (isUnderWater())
		{
			spd *= 0.67f;
		}

		if (state.tempSkinDuration)
		{
			spd *= 0.5f;
		}

		if (getBonus() == D6_BONUS_SPEED)
		{
			spd *= 1.43f;
		}

		return spd;
	}

	Float32 Player::getReloadInterval() const
	{
		Float32 coef = hasFastReload() ? 2.0f : 1.0f;
		return getWeapon().reloadSpeed / coef;
	}

	void Player::checkKeys()
	{
		if (isDead() || isPickingGun())
		{
			return;
		}

		if (!isKneeling())
		{
			if (controls.getLeft().isPressed())
			{
				setFlag(FlagMoveLeft);
			}

			if (controls.getRight().isPressed())
			{
				setFlag(FlagMoveRight);
			}

			if (controls.getUp().isPressed())
			{
				setFlag(FlagMoveUp);
			}
			if (controls.getPick().isPressed())
			{
				pick();
			}
		}

		if (controls.getShoot().isPressed())
		{
			shoot();
		}

		unsetFlag(FlagKnee);
		if (controls.getDown().isPressed())
		{
			fall();
		}
	}

	void Player::update(const World& world, ScreenMode screenMode, Float32 elapsedTime)
	{
		checkWater(world, elapsedTime);
		if (!isDead())
		{
			BONUS_Check(*this, messageQueue);
		}

		checkKeys();
		makeMove(world, elapsedTime);
		setAnm();

		// Drop gun if still has it and died
		if (isLying() && hasGun() && isOnGround())
		{
			dropWeapon(world);
			unsetFlag(FlagHasGun);
		}

		// Move intervals
		if (isReloading())
		{
			if ((state.timeToReload -= elapsedTime) <= 0)
			{
				state.timeToReload = 0;
			}
		}

		if (getBonusRemainingTime() > 0)
		{
			if ((state.bonusRemainingTime -= elapsedTime) <= 0)
			{
				if (getBonus() == D6_BONUS_INVIS)
				{
					setAlpha(1.0f);
				}
				state.bonus = -1;
				state.bonusRemainingTime = 0;
				state.bonusDuration = 0;
			}
		}

		if(getHPBarDuration() > 0)
		{
			state.hpBarDuration -= elapsedTime;
		}

		if (state.tempSkinDuration > 0)
		{
			if ((state.tempSkinDuration -= elapsedTime) <= 0)
			{
				switchToOriginalSkin();
			}
		}

		if (screenMode == ScreenMode::SplitScreen)
		{
			updateCam(world.getSizeX(), world.getSizeY());
		}		
	}

	void Player::setAnm()
	{
		Float32 ad = 0.0;
		Int16 *animation;

		if (isDead())
		{
			if (isLying())
			{
				animation = d6LAnim;
			}
			else
			{
				animation = d6NAnim;
			}
		}
		else if (isPickingGun())
		{
			animation = d6PAnim;
		}
		else if (!isOnGround())
		{
			animation = d6JAnim;
		}
		else if (isKneeling())
		{
			animation = d6DAnim;
			ad = 0.2f;
		}
		else if (!isMoving())
		{
			animation = d6SAnim;
		}
		else
		{
			animation = d6WAnim;
		}

		sprite->setPosition(getX(), getY())
			.setOrientation(getOrientation())
			.setAnimation(animation);

		gunSprite->setPosition(getX(), getY() - ad)
			.setOrientation(getOrientation())
			.setDraw(!isDead() && !isPickingGun());
	}

	void Player::prepareCam(const Video& video, ScreenMode screenMode, Int32 zoom, Int32 levelSizeX, Int32 levelSizeY)
	{
		Float32 fovX, fovY, mZ, dX = 0.0, dY = 0.0;
		fovY = (Float32)tan(MM_D2R(video.getView().getFieldOfView()) / 2.0f);
		fovX = video.getScreen().getAspect() * fovY;

		if (screenMode == ScreenMode::FullScreen)
		{
			if (levelSizeX > video.getScreen().getAspect() * levelSizeY)
				dX = (Float32)levelSizeX;
			else
				dY = (Float32)levelSizeY;
		}
		else if (levelSizeX > levelSizeY)
		{
			dX = (Float32)std::min(zoom, std::max(levelSizeX, levelSizeY));
		}

		if (dX == 0.0)
		{
			mZ = dY / (2.0f * fovY);
			dX = 2.0f * fovX * mZ;
		}
		else
		{
			mZ = dX / (2.0f * fovX);
			dY = 2.0f * fovY * mZ;
		}

		cameraPos.Pos.x = levelSizeX / 2.0f;
		cameraPos.Pos.y = levelSizeY / 2.0f;
		cameraPos.Pos.z = mZ + 1.0f;
		cameraPos.Left = cameraPos.Pos.x - dX / 2.0f;
		cameraPos.Right = cameraPos.Pos.x + dX / 2.0f;
		cameraPos.Down = cameraPos.Pos.y - dY / 2.0f;
		cameraPos.Up = cameraPos.Pos.y + dY / 2.0f;
		cameraPos.TolX = (dX * D6_CAM_TOLPER_X) / 200.0f;
		cameraPos.TolY = (dY * D6_CAM_TOLPER_Y) / 200.0f;
		
		camera.setpos(cameraPos.Pos);

		if (screenMode == ScreenMode::SplitScreen)
		{
			updateCam(levelSizeX, levelSizeY);
		}
	}

	void Player::updateCam(Int32 levelSizeX, Int32 levelSizeY)
	{
		Float32 mX = 0.0, mY = 0.0, X, Y;

		X = getX() + 0.5f;  // TODO: Coord
		Y = getY() - 0.5f;  // TODO: Coord

		if (X < cameraPos.Pos.x - cameraPos.TolX)
		{
			mX = X - (cameraPos.Pos.x - cameraPos.TolX);
			if (cameraPos.Left + mX < 0.0f)
				mX = -cameraPos.Left;
		}
		else if (X > cameraPos.Pos.x + cameraPos.TolX)
		{
			mX = X - (cameraPos.Pos.x + cameraPos.TolX);
			if (cameraPos.Right + mX >(Float32)levelSizeX)
				mX = (Float32)levelSizeX - cameraPos.Right;
		}
		if (Y < cameraPos.Pos.y - cameraPos.TolY)
		{
			mY = Y - (cameraPos.Pos.y - cameraPos.TolY);
			if (cameraPos.Down + mY < 0.0f)
				mY = -cameraPos.Down;
		}
		else if (Y > cameraPos.Pos.y + cameraPos.TolY)
		{
			mY = Y - (cameraPos.Pos.y + cameraPos.TolY);
			if (cameraPos.Up + mY >(Float32)levelSizeY)
				mY = (Float32)levelSizeY - cameraPos.Up;
		}

		if (mX != 0.0)
		{
			cameraPos.Left += mX;
			cameraPos.Right += mX;
			cameraPos.Pos.x += mX;
		}
		if (mY != 0.0)
		{
			cameraPos.Up += mY;
			cameraPos.Down += mY;
			cameraPos.Pos.y += mY;
		}
		if (mX != 0.0 || mY != 0.0)
		{
			camera.setpos(cameraPos.Pos);
		}
	}

	bool Player::hitByShot(Float32 amount, Shot& shot, bool directHit)
	{
		if (isInvulnerable() || !isInGame())
			return false;

		Player& shootingPlayer = shot.getPlayer();
		Person& shootingPerson = shootingPlayer.getPerson();
		const Weapon& weapon = shot.getWeapon();

		if (directHit && weapon.blood)
		{
			EXPL_Add(getX() + 0.3f + (rand() % 40) * 0.01f, shot.getY() + 0.85f, 0.2f, 0.5f, Color(255, 0, 0));   // TODO: Coord
		}

		if (isDead())
		{
			if (weapon.explodes && directHit)
			{
				unsetFlag(FlagLying);
				EXPL_Add(getX() + 0.5f, getY() + 0.3f, 0.5f, 1.2f, weapon.explosionColor); // TODO: Coord
			}
			return false;
		}

		state.life -= amount;
		state.hpBarDuration = D6_PLAYER_HPBAR;
		
		if (directHit)
		{			
			shootingPerson.addHits(1);

			if (shootingPlayer.getBonus() == D6_BONUS_VAMPIRESHOTS)
			{
				shootingPlayer.addLife(amount);
			}
		}

		if (state.life < 1)
		{
			state.life = 0;
			setFlag(FlagDead | FlagLying);
			unsetFlag(FlagKnee | FlagPick);
			
			sprite->setPosition(getX(), getY()).setLooping(AnimationLooping::OnceAndStop);
			gunSprite->setDraw(false);

			state.orientation = (shot.getX() < getX()) ? Orientation::Left : Orientation::Right;

			if (!is(shootingPlayer))
			{
				messageQueue.add(*this, Format(D6_L("You are dead - you were killed by {0}")) << shootingPerson.getName());
				messageQueue.add(shootingPlayer, Format(D6_L("You killed player {0}")) << getPerson().getName());
			}
			else
			{
				messageQueue.add(*this, D6_L("You are dead"));
			}

			if (weapon.explodes && directHit)
			{
				unsetFlag(FlagLying);
				EXPL_Add(getX() + 0.5f, getY() + 0.5f, 0.5f, 1.2f, weapon.explosionColor);  // TODO: Coord
			}

			return true;
		}
		
		if (directHit)
		{
			playSound(PlayerSounds::Type::GotHit);
		}

		return false;
	}


	bool Player::hit(Float32 amount)
	{
		if (isInvulnerable() || isDead())
			return false;

		state.life -= amount;
		state.hpBarDuration = D6_PLAYER_HPBAR;
		if (state.life < 1)
		{
			state.life = 0;
			setFlag(FlagDead | FlagLying);
			unsetFlag(FlagKnee | FlagPick);
			
			sprite->setPosition(getX(), getY()).setLooping(AnimationLooping::OnceAndStop);
			gunSprite->setDraw(false);
			messageQueue.add(*this, D6_L("You are dead"));
			return true;
		}

		return false;
	}

	void Player::dropWeapon(const World& world)
	{
		int x1 = int(getX() + 0.2f), x2 = int(getX() + 0.8f); // TODO: Coord
		int y = int(getY() + 0.5f);  // TODO: Coord

		if (world.isWall(x1, y - 1, true) && !world.isWall(x1, y, true))
		{
			BONUS_AddDeadManGun(x1, y, *this);
		}
		else
		{
			if (world.isWall(x2, y - 1, true) && !world.isWall(x2, y, true))
			{
				BONUS_AddDeadManGun(x2, y, *this);
			}
		}
	}

	Player& Player::addLife(Float32 life)
	{
		state.life = std::max(0.0f, std::min(Float32(D6_MAX_LIFE), state.life + life));
		return *this;
	}

	void Player::checkWater(const World& world, Float32 elapsedTime)
	{
		// Check if head is in water
		water.headUnderWater = false;
		Water::Type waterType = world.getWaterType(Int32(getX() + 0.5f), Int32(getY() + 0.8f));   // TODO: Coord
		if (waterType != Water::Type::None)
		{
			Float32 airHitAmount = waterSet.at(waterType)->getAirHit() * elapsedTime;
			water.headUnderWater = true;
			if ((state.air -= airHitAmount) < 0)
			{
				state.air = 0;
				if (hit(airHitAmount))
				{
					playSound(PlayerSounds::Type::Drowned);
					getPerson().addPenalties(1);  // Player drowned
				}
			}
			return;
		}

		state.air = std::min(state.air + 2 * D6_AIR_RECHARGE_SPEED * elapsedTime, D6_MAX_AIR);

		// Check if foot is in water
		waterType = world.getWaterType(Int32(getX() + 0.5f), Int32(getY() + 0.1f));  // TODO: Coord
		if (waterType != Water::Type::None && !water.feetInWater)
		{
			const Water& water = *waterSet.at(waterType);
			water.addSplash(spriteList, getX(), getY()); // TODO: Coord
			water.getSplashSound().play();
			this->water.feetInWater = true;
		}
		else if (waterType == Water::Type::None)
		{
			water.type = Water::Type::None;
			water.feetInWater = false;
		}
	}

	void Player::checkMoveUp(const World& world)
	{
		Int32 up = (Int32)(getY() + 0.94); // TODO: Coord
		Int32 left = (Int32)(getX() + 0.1f); // TODO: Coord
		Int32 right = (Int32)(getX() + 0.9f); // TODO: Coord

		if (world.isWall(left, up, true) || world.isWall(right, up, true))
		{
			state.y = (Float32)(up) - 1.0f; // TODO: Coord
			state.jumpPhase = 180.0f;
		}
	}

	void Player::checkMoveDown(const World& world)
	{
		Int32 down = (Int32)getY(); // TODO: Coord
		Int32 left = (Int32)(getX() + 0.1f); // TODO: Coord
		Int32 right = (Int32)(getX() + 0.9f); // TODO: Coord

		if (world.isWall(left, down, true) || world.isWall(right, down, true))
		{
			state.y = (Float32)(down) + 1.0001f; // TODO: Coord
			state.jumpPhase = 0.0f;
		}

		checkElevator();
	}

	void Player::checkFall(const World& world)
	{
		checkElevator();

		if (isOnElevator())
			return;

		Int32 down = (Int32)(getY() - 0.001f); // TODO: Coord
		Int32 left = (Int32)(getX() + 0.1f); // TODO: Coord
		Int32 right = (Int32)(getX() + 0.9f); // TODO: Coord

		if (!world.isWall(left, down, true) && !world.isWall(right, down, true))
		{
			state.jumpPhase = 180.0f;
		}
	}

	void Player::checkHorizontalMove(const World& world)
	{
		Int32 up = (Int32)(getY() + 0.94); // TODO: Coord
		Int32 down = (Int32)getY(); // TODO: Coord

		if (state.velocity < 0)
		{
			Int32 left = (Int32)(getX() + 0.1f); // TODO: Coord
			if (world.isWall(left, up, true) || world.isWall(left, down, true))
			{
				state.x = (Float32)left + 0.9001f; // TODO: Coord
			}
		}
		else
		{
			Int32 right = (Int32)(getX() + 0.9f); // TODO: Coord
			if (world.isWall(right, up, true) || world.isWall(right, down, true))
			{
				state.x = (Float32)right - 0.9001f; // TODO: Coord
			}
		}
	}

	void Player::checkElevator()
	{
		const Elevator* elevator = ELEV_CheckMan(*this);

		if (elevator != nullptr)
		{
			state.elevator = elevator;
			state.y = elevator->getY();
			state.jumpPhase = 0.0f;
		}
	}

	void Player::useTemporarySkin(PlayerSkin& tempSkin)
	{
		state.tempSkinDuration = Float32(10 + rand() % 5);
		sprite->setTextures(tempSkin.getTextures());
	}

	void Player::switchToOriginalSkin()
	{
		state.tempSkinDuration = 0;
		sprite->setTextures(skin.getTextures());
	}	
}