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
	static const char *const NO_TEAM = "";

	static Int16 noAnim[4] = { 0, 50, 0, -1 };
	static Int16 d6SAnim[22] = { 0, 200, 1, 30, 0, 30, 2, 30, 0, 90, 3, 15, 4, 15, 3, 15, 4, 15, 3, 30, -1, 0 };
	static Int16 d6WAnim[10] = { 6, 5, 5, 5, 6, 5, 7, 5, -1, 0 };
	static Int16 d6JAnim[4] = { 8, 50, -1, 0 };
	static Int16 d6DAnim[18] = { 9, 300, 10, 60, 9, 60, 11, 60, 9, 150, 12, 60, 9, 60, 12, 60, -1, 0 };
	static Int16 d6LAnim[16] = { 13, 10, 14, 10, 15, 10, 16, 10, 17, 10, 18, 10, 19, 10, -1, 0 };
	static Int16 d6NAnim[4] = { 25, 100, -1, 0 };
	static Int16 d6PAnim[] = { 0, 10, 20, 10, 21, 10, 22, 10, 23, 10, 24, 10, 23, 10, 22, 10, 21, 10, 0, 10, -1, 0 };

	Player::Player(Person& person, const PlayerSkin& skin, const PlayerSounds& sounds, const PlayerControls& controls)
		: person(person), skin(skin), sounds(sounds), controls(controls)
	{
		camera.rotate(180.0, 0.0, 0.0);
	}

	Player::~Player()
	{
	}
	
	void Player::startGame(World& world, Int32 startBlockX, Int32 startBlockY, Int32 ammo)
	{
		this->world = &world;
		state.position = Vector(Float32(startBlockX), Float32(startBlockY) + 0.0001f);

		Sprite manSprite(noAnim, skin.getTextures());
		manSprite.setPosition(getSpritePosition(), 0.5f);
		sprite = world.getSpriteList().addSprite(manSprite);

		state.weapon = &WPN_GetRandomWeapon();
		Sprite gunSprite(state.weapon->animation, state.weapon->textures.gun);
		gunSprite.setPosition(getGunSpritePosition(), 0.5f)
			.setLooping(AnimationLooping::OnceAndStop)
			.setFrame(6);
		this->gunSprite = world.getSpriteList().addSprite(gunSprite);

		state.flags = FlagHasGun;
		state.velocity = 0.0f;
		state.orientation = Orientation::Left;
		state.jumpPhase = 0;
		state.timeToReload = 0;
		state.life = D6_MAX_LIFE;
		state.air = D6_MAX_AIR;
		state.ammo = ammo;
		state.elevator = nullptr;
		state.bonus = D6_BONUS_INVUL;
		state.bonusDuration = 2.0f;
		state.bonusRemainingTime = 2.0f;
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

	void Player::moveHorizontal(const Level& level, Float32 elapsedTime, Float32 speed)
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
			state.position.x += state.velocity * D6_PLAYER_ACCEL * speed;
			checkHorizontalMove(level);
		}
	}

	void Player::moveVertical(const Level& level, Float32 elapsedTime, Float32 speed)
	{
		if (isOnGround() && hasFlag(FlagMoveUp))
		{
			Int32 up = (Int32)(getPosition().y + 1.0f); // TODO: Coord
			Int32 left = (Int32)(getPosition().x + 0.1f); // TODO: Coord
			Int32 right = (Int32)(getPosition().x + 0.9f); // TODO: Coord

			if (!level.isWall(left, up, true) && !level.isWall(right, up, true))
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

			state.position.y += Math::fastSin(Int32(state.jumpPhase)) * D6_PLAYER_JUMP_SPEED * speed;

			if (isRising())
			{
				checkMoveUp(level);
			}
			else
			{
				checkMoveDown(level);
			}
		}
		else
		{
			checkFall(level);
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
			BONUS_CheckPick(*this, world->getMessageQueue());
		}
	}

	void Player::shoot()
	{
		if (!getAmmo() || isReloading() || !hasGun())
			return;

		state.timeToReload = getReloadInterval();
		state.ammo--;
		gunSprite->setFrame(0);
		getPerson().addShots(1);
		Orientation originalOrientation = getOrientation();

		WPN_AddShot(*this, world->getSpriteList(), originalOrientation);

		if (getBonus() == D6_BONUS_SPLITFIRE && getAmmo() > 0)
		{
			state.ammo--;
			getPerson().addShots(1);
			Orientation secondaryOrientation = originalOrientation == Orientation::Left ? Orientation::Right : Orientation::Left;
			WPN_AddShot(*this, world->getSpriteList(), secondaryOrientation);
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

	void Player::makeMove(const Level& level, Float32 elapsedTime) {
		Float32 speed = getSpeed() * elapsedTime;

		if (isOnElevator() && level.isWall((Int32) getPosition().x, (Int32) getPosition().y, false))
		{
			state.velocity = 0;
		}

		state.elevator = nullptr;

		moveVertical(level, elapsedTime, speed);
		moveHorizontal(level, elapsedTime, speed);

		if (isOnElevator())
		{
			state.position += state.elevator->getVelocity() * elapsedTime;
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
		else
		{
			spd *= (1.4f - (getLife() / 250));
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
		if ((isDead() && !isGhost()) || isPickingGun())
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

	void Player::update(World& world, ScreenMode screenMode, Float32 elapsedTime)
	{
		checkWater(world, elapsedTime);
		if (!isDead())
		{
			BONUS_Check(*this, world.getMessageQueue());
		}

		checkKeys();
		makeMove(world.getLevel(), elapsedTime);
		setAnm();

		// Drop gun if still has it and died
		if (isLying() && hasGun() && isOnGround())
		{
			clearBonus();
			dropWeapon(world.getLevel());
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
				clearBonus();
			}
		}

		if(getHPBarDuration() > 0)
		{
			state.hpBarDuration -= elapsedTime;
		}

		if(getLife() < D6_MAX_LIFE && getLife() > 1 && state.timeSinceHit > D6_PLAYER_HPREGEN_DELAY)
		{
			addLife(elapsedTime * getRoundKills() * 0.8f, false);
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
			updateCam(world.getLevel().getWidth(), world.getLevel().getHeight());
		}

		state.timeSinceHit += elapsedTime;
	}

	void Player::setAnm()
	{
		Int16 *animation;

		if (isDead() && !isGhost())
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
		}
		else if (!isMoving())
		{
			animation = d6SAnim;
		}
		else
		{
			animation = d6WAnim;
		}

		sprite->setPosition(getSpritePosition())
			.setOrientation(getOrientation())
			.setAnimation(animation);

		gunSprite->setPosition(getGunSpritePosition())
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
		Float32 mX = 0.0, mY = 0.0;
		Vector pos = getCentre();

		if (pos.x < cameraPos.Pos.x - cameraPos.TolX)
		{
			mX = pos.x - (cameraPos.Pos.x - cameraPos.TolX);
			if (cameraPos.Left + mX < 0.0f)
				mX = -cameraPos.Left;
		}
		else if (pos.x > cameraPos.Pos.x + cameraPos.TolX)
		{
			mX = pos.x - (cameraPos.Pos.x + cameraPos.TolX);
			if (cameraPos.Right + mX >(Float32)levelSizeX)
				mX = (Float32)levelSizeX - cameraPos.Right;
		}
		if (pos.y < cameraPos.Pos.y - cameraPos.TolY)
		{
			mY = pos.y - (cameraPos.Pos.y - cameraPos.TolY);
			if (cameraPos.Down + mY < 0.0f)
				mY = -cameraPos.Down;
		}
		else if (pos.y > cameraPos.Pos.y + cameraPos.TolY)
		{
			mY = pos.y - (cameraPos.Pos.y + cameraPos.TolY);
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
		{
			return false;
		}

		Player& shootingPlayer = shot.getPlayer();
		Person& shootingPerson = shootingPlayer.getPerson();
		const Weapon& weapon = shot.getWeapon();

		if (directHit && weapon.blood)
		{
			Rectangle rect = getCollisionRect();
			Vector shotCentre = shot.getCentre();
			world->getExplosionList().add(Vector(rect.left.x + (0.3f + (rand() % 40) * 0.01f) * rect.getSize().x, shotCentre.y), 0.2f, 0.5f, Color(255, 0, 0));
		}

		if (isDead())
		{
			if (weapon.explodes && directHit)
			{
				unsetFlag(FlagLying);
				world->getExplosionList().add(getCentre(), 0.5f, 1.2f, weapon.explosionColor);

				setFlag(FlagGhost);
				setAlpha(0.1f);
			}
			return false;
		}


        if (!eventListener->onDamageByShot(*this, shootingPlayer, amount, shot, directHit))
        {
            return false;
        }

		state.timeSinceHit = 0;
		
		if (directHit)
		{			
			shootingPerson.addHits(1);

			if (shootingPlayer.getBonus() == D6_BONUS_VAMPIRESHOTS)
			{
				shootingPlayer.addLife(amount);
			}
		}

		if (state.life <= 0.0f)
		{
			setFlag(FlagDead | FlagLying);
			unsetFlag(FlagKnee | FlagPick);
			
			sprite->setPosition(getSpritePosition()).setLooping(AnimationLooping::OnceAndStop);
			gunSprite->setDraw(false);

			state.orientation = (shot.getCentre().x < getCentre().x) ? Orientation::Left : Orientation::Right;

			if (weapon.explodes && directHit)
			{
				unsetFlag(FlagLying);
				world->getExplosionList().add(getCentre(), 0.5f, 1.2f, weapon.explosionColor);
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

        if (!eventListener->onDamageByEnv(*this, amount))
        {
            return false;
        }

		state.timeSinceHit = 0;

		if (state.life <= 0.0f)
		{
			setFlag(FlagDead | FlagLying);
			unsetFlag(FlagKnee | FlagPick);
			
			sprite->setPosition(getSpritePosition()).setLooping(AnimationLooping::OnceAndStop);
			gunSprite->setDraw(false);
			eventListener->onKillByEnv(*this);
			return true;
		}

		return false;
	}

	void Player::dropWeapon(const Level& level)
	{
		Rectangle bbox = getCollisionRect();
		Int32 x1 = Int32(bbox.left.x), x2 = Int32(bbox.right.x);
		Int32 y = Int32(bbox.left.y + 0.5f);

		if (level.isWall(x1, y - 1, true) && !level.isWall(x1, y, true))
		{
			BONUS_AddDeadManGun(Vector(x1, y), *this);
		}
		else
		{
			if (level.isWall(x2, y - 1, true) && !level.isWall(x2, y, true))
			{
				BONUS_AddDeadManGun(Vector(x2, y), *this);
			}
		}
	}

	Player& Player::addLife(Float32 change, bool showHpBar)
	{
		Float32 oldLife = state.life;
		state.life = std::max(0.0f, std::min(Float32(D6_MAX_LIFE), state.life + change));
		if (showHpBar && oldLife != state.life) {
			showHPBar();
		}
		return *this;
	}

	void Player::checkWater(World& world, Float32 elapsedTime)
	{
		const Rectangle rect = getCollisionRect();
		Float32 centerX = rect.getCentre().x;
		Float32 headY = rect.left.y + rect.getSize().y * 0.8f;
		Float32 feetY = rect.left.y + rect.getSize().y * 0.1f;
		Float32 bottomY = rect.left.y;

		// Check if head is in water
		water.headUnderWater = false;
		Water::Type waterType = world.getLevel().getWaterType(Int32(centerX), Int32(headY));
		if (waterType != Water::Type::None)
		{
			Float32 airHitAmount = world.getWaterSet().at(waterType)->getAirHit() * elapsedTime;
			water.headUnderWater = true;
			if ((state.air -= airHitAmount) < 0)
			{
				state.air = 0;
				if (hit(airHitAmount))
				{
					playSound(PlayerSounds::Type::Drowned);
				}
			}
			return;
		}

		state.air = std::min(state.air + 2 * D6_AIR_RECHARGE_SPEED * elapsedTime, D6_MAX_AIR);

		// Check if foot is in water
		waterType = world.getLevel().getWaterType(Int32(centerX), Int32(feetY));
		if (waterType != Water::Type::None && !water.feetInWater) {
			const Water &water = *world.getWaterSet().at(waterType);
			water.addSplash(world.getSpriteList(), Vector(centerX, bottomY));
			water.getSplashSound().play();
			this->water.feetInWater = true;
		}
		else if (waterType == Water::Type::None) {
			water.type = Water::Type::None;
			water.feetInWater = false;
		}
	}

	void Player::checkMoveUp(const Level& level)
	{
		Int32 up = (Int32)(getPosition().y + 0.94); // TODO: Coord
		Int32 left = (Int32)(getPosition().x + 0.1f); // TODO: Coord
		Int32 right = (Int32)(getPosition().x + 0.9f); // TODO: Coord

		if (level.isWall(left, up, true) || level.isWall(right, up, true))
		{
			state.position.y = (Float32)(up) - 1.0f; // TODO: Coord
			state.jumpPhase = 180.0f;
		}
	}

	void Player::checkMoveDown(const Level& level)
	{
		Int32 down = (Int32)getPosition().y; // TODO: Coord
		Int32 left = (Int32)(getPosition().x + 0.1f); // TODO: Coord
		Int32 right = (Int32)(getPosition().x + 0.9f); // TODO: Coord

		if (level.isWall(left, down, true) || level.isWall(right, down, true))
		{
			state.position.y = (Float32)(down) + 1.0001f; // TODO: Coord
			state.jumpPhase = 0.0f;
		}

		checkElevator();
	}

	void Player::checkFall(const Level& level)
	{
		checkElevator();

		if (isOnElevator())
			return;

		Int32 down = (Int32)(getPosition().y - 0.001f); // TODO: Coord
		Int32 left = (Int32)(getPosition().x + 0.1f); // TODO: Coord
		Int32 right = (Int32)(getPosition().x + 0.9f); // TODO: Coord

		if (!level.isWall(left, down, true) && !level.isWall(right, down, true))
		{
			state.jumpPhase = 180.0f;
		}
	}

	void Player::checkHorizontalMove(const Level& level)
	{
		Int32 up = (Int32)(getPosition().y + 0.94); // TODO: Coord
		Int32 down = (Int32)getPosition().y; // TODO: Coord

		if (state.velocity < 0)
		{
			Int32 left = (Int32)(getPosition().x + 0.1f); // TODO: Coord
			if (level.isWall(left, up, true) || level.isWall(left, down, true))
			{
				state.position.x = (Float32)left + 0.9001f; // TODO: Coord
			}
		}
		else
		{
			Int32 right = (Int32)(getPosition().x + 0.9f); // TODO: Coord
			if (level.isWall(right, up, true) || level.isWall(right, down, true))
			{
				state.position.x = (Float32)right - 0.9001f; // TODO: Coord
			}
		}
	}

	void Player::checkElevator()
	{
		const Elevator* elevator = ELEV_CheckMan(*this);

		if (elevator != nullptr)
		{
			state.elevator = elevator;
			state.position.y = elevator->getPosition().y;
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

	bool Player::hasAnyTeam() const
	{
		return teamName != NO_TEAM;
	}

	bool Player::hasTeam(std::string team) const
	{
		return team == teamName;
	}

	void Player::setTeam(std::string team)
	{
		this->teamName = team;
	}

	void Player::unsetTeam()
	{
		this->teamName = NO_TEAM;
	}

	std::string Player::getTeam() const
	{
		return teamName;
	}

	void Player::setOverlay(Color overlay)
	{
		sprite->setOverlay(overlay);
	}

	void Player::unsetOverlay()
	{
		sprite->unsetOverlay();
	}

    void Player::processKills(Shot &shot, std::vector<Player *> killedPlayers)
    {
        bool suicideKills = (std::find(killedPlayers.begin(), killedPlayers.end(), this) != killedPlayers.end());

        if (suicideKills) // Killed self
        {
            playSound(PlayerSounds::Type::Suicide);
            eventListener->onSuicide(*this, (int) (killedPlayers.size() -1));
        }
        else if (killedPlayers.size() > 0)
        {
            playSound(PlayerSounds::Type::KilledOther);
        }

        for (Player* player : killedPlayers)
        {
            if (!player->is(*this))
            {
                player->playSound(PlayerSounds::Type::WasKilled);
                player->eventListener->onKillByPlayer(*player, *this, shot, suicideKills);
            }
        }
    }

    void Player::processHits(Shot &shot, std::vector<Player *> hittedPlayers)
    {
        bool hittedSelf = (std::find(hittedPlayers.begin(), hittedPlayers.end(), this) != hittedPlayers.end());
        if (!hittedSelf && hittedPlayers.size() > 0)
        {
            playSound(PlayerSounds::Type::HitOther);
        }
    }
}