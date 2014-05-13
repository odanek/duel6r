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
#include "project.h"
#include "Player.h"
#include "BonusList.h"
#include "Weapon.h"
#include "ElevatorList.h"
#include "Math.h"

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
	static Int16 wtAnim[2][24] =
	{
		{ 57, 5, 58, 5, 59, 5, 60, 5, 61, 5, 62, 5, 63, 5, 64, 5, 66, 5, 67, 5, -1, 0 },
		{ 83, 5, 84, 5, 85, 5, 86, 5, 87, 5, 88, 5, 89, 5, 90, 5, 91, 5, 92, 5, -1, 0 }
	};


	void PLAYER_View(int i, int x, int y)
	{
		int w = g_vid.cl_width / 2 - 4, h = g_vid.cl_height / 2 - 4;

		Player& player = d6Players[i];
		player.setView(x, y, w, h);
		player.setInfoBarPosition(x + w / 2 - 76, y + 30);
	}

	void PLAYER_PrepareViews(ScreenMode screenMode)
	{
		int     xShift = (g_vid.cl_width / 4) / 2 - 70;

		for (Player& player : d6Players)
		{
			player.prepareCam(screenMode);
		}

		if (screenMode == ScreenMode::FullScreen)
		{
			Size index = 0;
			for (Player& player : d6Players)
			{
				player.setView(0, 0, g_vid.cl_width, g_vid.cl_height);

				if (index < 4)
				{
					player.setInfoBarPosition((g_vid.cl_width / 4) * index + xShift, 30);
				}
				else
				{
					player.setInfoBarPosition((g_vid.cl_width / 4) * (index - 4) + xShift, g_vid.cl_height - 7);
				}

				index++;
			}

			return;
		}

		if (d6Players.size() == 2)
		{
			PLAYER_View(0, g_vid.cl_width / 4 + 2, 2);
			PLAYER_View(1, g_vid.cl_width / 4 + 2, g_vid.cl_height / 2 + 2);
		}

		if (d6Players.size() == 3)
		{
			PLAYER_View(0, 2, 2);
			PLAYER_View(1, g_vid.cl_width / 2 + 2, 2);
			PLAYER_View(2, g_vid.cl_width / 4 + 2, g_vid.cl_height / 2 + 2);
		}

		if (d6Players.size() == 4)
		{
			PLAYER_View(0, 2, 2);
			PLAYER_View(1, g_vid.cl_width / 2 + 2, 2);
			PLAYER_View(2, 2, g_vid.cl_height / 2 + 2);
			PLAYER_View(3, g_vid.cl_width / 2 + 2, g_vid.cl_height / 2 + 2);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//
	//          Player
	//
	////////////////////////////////////////////////////////////////////////////////////////

	Player::Player(Person& person, PlayerSkin* skin, const PlayerControls& controls)
		: person(person), skin(skin), controls(controls)
	{
		camera.resize(false, (mval_t)g_vid.gl_fov, float(g_vid.cl_width) / g_vid.cl_height);
		camera.rotate(180.0, 0.0, 0.0);
	}

	Player::~Player()
	{
	}

	void Player::prepareForGame()
	{
		findStartingPosition();
		
		Sprite manSprite(noAnim, skin->getTextures());
		manSprite.setPosition(getX(), getY(), 0.5f);
		State.sprite = d6SpriteList.addSprite(manSprite);		

		State.weapon = &WPN_GetRandomWeapon();
		Sprite gunSprite(State.weapon->animation, d6WpnTextures);
		gunSprite.setPosition(getX(), getY(), 0.5f)
			.setLooping(AnimationLooping::OnceAndStop)
			.setFrame(6);
		State.gunSprite = d6SpriteList.addSprite(gunSprite);

		State.flags = 0;
		State.velocity = 0.0f;
		State.orientation = Orientation::Left;
		State.jumpPhase = 0;
		State.reloadInterval = 0;
		State.life = D6_MAX_LIFE;
		State.air = D6_MAX_AIR;
		State.ammo = d6AmmoRangeMin + rand() % (d6AmmoRangeMax - d6AmmoRangeMin + 1);
		State.elevator = nullptr;
		State.bonus = 0;
		State.bonusDuration = 0;
		State.tempSkinDuration = 0;
		State.feetInWater = false;

		Person().setGames(Person().getGames() + 1);
	}

	void Player::findStartingPosition()
	{
		while (true)
		{
			Int32 X = rand() % d6World.getSizeX();
			Int32 Y = rand() % d6World.getSizeY();

			if (Y > 0 && !d6World.isWall(X, Y, true) && !d6World.isWater(X, Y))
			{
				Int32 y2 = Y;

				while (y2-- > 0)
				{
					if (d6World.isWater(X, y2))
						break;
					if (d6World.isWall(X, y2, true))
					{
						State.x = (Float32)X;
						State.y = (Float32)Y + 0.0001f;
						return;
					}
				}
			}
		}
	}

	void Player::setView(int x, int y, int w, int h)
	{
		view.X = x;
		view.Y = y;
		view.Width = w;
		view.Height = h;
	}

	void Player::moveLeft(Float32 elapsedTime)
	{
		State.velocity = std::max(State.velocity - elapsedTime * D6_SPEED_COEF, -D6_PLAYER_MAX_SPEED);

		if (State.velocity < 0)
		{
			State.orientation = Orientation::Left;
		}
	}

	void Player::moveRight(Float32 elapsedTime)
	{
		State.velocity = std::min(State.velocity + elapsedTime * D6_SPEED_COEF, D6_PLAYER_MAX_SPEED);

		if (State.velocity > 0)
		{
			State.orientation = Orientation::Right;
		}
	}

	void Player::jump()
	{
		if (isOnGround())
		{
			Int32 up = (Int32)(getY() + 1.0f); // TODO: Coord
			Int32 left = (Int32)(getX() + 0.1f); // TODO: Coord
			Int32 right = (Int32)(getX() + 0.9f); // TODO: Coord

			if (!d6World.isWall(left, up, true) && !d6World.isWall(right, up, true))
			{
				State.jumpPhase = 90.0f;
			}
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
				State.jumpPhase = 180.0f;
			}
		}
	}

	void Player::pick()
	{
		if (isOnGround() && !isMoving() && !isOnElevator())
		{
			BONUS_CheckPick(*this);
		}
	}

	void Player::shoot()
	{
		if (!getAmmo() || isReloading())
			return;

		State.reloadInterval = getWeapon().reloadSpeed;
		if (hasFastReload())
		{
			State.reloadInterval /= 2.0f;
		}

		State.ammo--;
		State.gunSprite->setFrame(0);
		getPerson().setShots(getPerson().getShots() + 1);

		WPN_AddShot(*this);
	}

	Player& Player::pickWeapon(const Weapon& weapon, Int32 bullets)
	{
		setFlag(FlagPick);
		State.weapon = &weapon;
		State.ammo = bullets;
		State.reloadInterval = 0;		
					
		State.gunSprite->setAnimation(weapon.animation).setFrame(6);

		return *this;
	}

	void Player::makeMove(Float32 elapsedTime)
	{
		Float32 speed = getSpeed() * elapsedTime;
		State.elevator = nullptr;

		if (!isOnGround())
		{
			State.jumpPhase += D6_PLAYER_JPHASE_SPEED * elapsedTime;
			if (State.jumpPhase > 270.0)
			{
				State.jumpPhase = 270.0f;
			}

			State.y += Math::fastCos(Int32(State.jumpPhase)) * D6_PLAYER_JUMP_SPEED * speed;

			if (isRising())
			{
				checkMoveUp();
			}
			else
			{
				checkMoveDown();
			}
		}
		else
		{
			checkFall();
		}

		if (isMoving())
		{
			State.x += State.velocity * D6_PLAYER_ACCEL * D6_SPEED_COEF * speed;
			checkMoveAside();
		}

		if (isOnElevator())
		{
			State.x += State.elevator->getMoveX() * elapsedTime;
			State.y += State.elevator->getMoveY() * elapsedTime;
		}

		if (isPickingGun() && State.sprite->isFinished())
		{
			unsetFlag(FlagPick);
		}
	}

	Float32 Player::getSpeed() const
	{
		Float32 spd = 1.0f;

		if (isUnderWater())
		{
			spd *= 0.67f;
		}

		if (State.tempSkinDuration)
		{
			spd *= 0.5f;
		}

		if (getBonus() == D6_BONUS_SPEED)
		{
			spd *= 1.43f;
		}

		return spd;
	}

	void Player::checkKeys(Float32 elapsedTime)
	{
		if (isDead() || isPickingGun())
		{
			return;
		}

		if (!isKneeling())
		{
			if (CO_InpIsPressed(controls.Left))
			{
				moveLeft(elapsedTime);
			}
			else if (State.velocity < 0.0f)
			{
				State.velocity = std::min(State.velocity + elapsedTime * D6_SPEED_COEF, 0.0f);
			}

			if (CO_InpIsPressed(controls.Right))
			{
				moveRight(elapsedTime);
			}
			else if (State.velocity > 0.0f)
			{
				State.velocity = std::max(State.velocity - elapsedTime * D6_SPEED_COEF, 0.0f);
			}

			if (CO_InpIsPressed(controls.Up))
			{
				jump();
			}
			if (CO_InpIsPressed(controls.Pick))
			{
				pick();
			}
		}

		if (CO_InpIsPressed(controls.Shoot))
		{
			shoot();
		}

		unsetFlag(FlagKnee);
		if (CO_InpIsPressed(controls.Down))
		{
			fall();
		}
	}

	void Player::update(ScreenMode screenMode, Float32 elapsedTime)
	{
		checkWater(elapsedTime);
		if (!isDead())
		{
			BONUS_Check(*this);
		}

		checkKeys(elapsedTime);
		makeMove(elapsedTime);
		setAnm();

		// Move intervals
		if (State.reloadInterval > 0)
		{
			if ((State.reloadInterval -= elapsedTime) <= 0)
			{
				State.reloadInterval = 0;
			}
		}

		if (State.bonusDuration > 0)
		{
			if ((State.bonusDuration -= elapsedTime) <= 0)
			{
				if (getBonus() == D6_BONUS_INVIS)
				{
					setAlpha(1.0f);
				}
				State.bonus = 0;
				State.bonusDuration = 0;
			}
		}

		if (State.tempSkinDuration > 0)
		{
			if ((State.tempSkinDuration -= elapsedTime) <= 0)
			{
				switchToOriginalSkin();
			}
		}

		if (screenMode == ScreenMode::SplitScreen)
		{
			updateCam();
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

		State.sprite->setPosition(getX(), getY())
			.setOrientation(getOrientation())
			.setAnimation(animation);

		State.gunSprite->setPosition(getX(), getY() - ad)
			.setOrientation(getOrientation())
			.setDraw(!isDead() && !isPickingGun());
	}

	void Player::prepareCam(ScreenMode screenMode)
	{
		Float32 fovX, fovY, mZ, dX = 0.0, dY = 0.0;
		Int32 sX = d6World.getSizeX(), sY = d6World.getSizeY();

		cameraPos.Pos.x = sX / 2.0f;
		cameraPos.Pos.y = sY / 2.0f;

		fovY = (float)tan(MM_D2R(g_vid.gl_fov) / 2.0f);
		fovX = g_vid.gl_aspect * fovY;

		if (screenMode == ScreenMode::FullScreen)
		{
			if (sX > g_vid.gl_aspect * sY)
				dX = (float)sX;
			else
				dY = (float)sY;
		}
		else if (sX > sY)
		{
			if (d6ZoomBlc > sY)
				dX = (float)sY;
			else
				dX = (float)d6ZoomBlc;
		}
		else
		{
			if (d6ZoomBlc > sX)
				dX = (float)sX;
			else
				dX = (float)d6ZoomBlc;
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
			updateCam();
		}
	}

	void Player::updateCam()
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
			if (cameraPos.Right + mX >(float) d6World.getSizeX())
				mX = (float)d6World.getSizeX() - cameraPos.Right;
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
			if (cameraPos.Up + mY >(float) d6World.getSizeY())
				mY = (float)d6World.getSizeY() - cameraPos.Up;
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
			camera.setpos(cameraPos.Pos);
	}

	bool Player::hitByShot(Float32 amount, Shot& shot, bool directHit)
	{
		if (isInvulnerable() || !isInGame())
			return false;

		Person& shootingPerson = shot.getPlayer().getPerson();
		const Weapon& weapon = shot.getWeapon();

		if (directHit && weapon.Blood)
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

		State.life -= amount;
		
		if (directHit)
		{			
			shootingPerson.setHits(shootingPerson.getHits() + 1);
		}

		if (State.life < 1)
		{
			State.life = 0;
			setFlag(FlagDead | FlagLying);
			unsetFlag(FlagKnee | FlagPick);
			
			State.sprite->setPosition(getX(), getY()).setLooping(AnimationLooping::OnceAndStop);
			State.gunSprite->setDraw(false);

			State.orientation = (shot.getX() < getX()) ? Orientation::Left : Orientation::Right;

			if (!is(shot.getPlayer()))
			{
				shootingPerson.setKills(shootingPerson.getKills() + 1);
				d6MessageQueue.add(*this, MY_L("APP00051|Jsi mrtvy - zabil te %s"), shootingPerson.getName().c_str());
				d6MessageQueue.add(shot.getPlayer(), MY_L("APP00052|Zabil jsi hrace %s"), getPerson().getName().c_str());
			}
			else
			{
				d6MessageQueue.add(*this, MY_L("APP00053|Jsi mrtvy"));
			}

			if (weapon.explodes && directHit)
			{
				unsetFlag(FlagLying);
				EXPL_Add(getX() + 0.5f, getY() + 0.5f, 0.5f, 1.2f, weapon.explosionColor);  // TODO: Coord
			}
			else if (isOnGround())
			{
				dropWeapon();
			}

			SOUND_PlaySample(D6_SND_DEAD);
			return true;
		}
		
		if (directHit)
		{
			SOUND_PlaySample(D6_SND_HIT);
		}

		return false;
	}


	bool Player::hit(Float32 amount)
	{
		if (isInvulnerable() || isDead())
			return false;

		State.life -= amount;
		if (State.life < 1)
		{
			State.life = 0;
			setFlag(FlagDead | FlagLying);
			unsetFlag(FlagKnee | FlagPick);
			
			State.sprite->setPosition(getX(), getY()).setLooping(AnimationLooping::OnceAndStop);
			State.gunSprite->setDraw(false);
			d6MessageQueue.add(*this, MY_L("APP00054|Jsi mrtvy"));

			SOUND_PlaySample(D6_SND_DEAD);

			// Add lying weapon
			if (isOnGround())
			{
				dropWeapon();
			}

			return true;
		}

		SOUND_PlaySample(D6_SND_HIT);
		return false;
	}

	void Player::dropWeapon()
	{
		int x1 = int(getX() + 0.2f), x2 = int(getX() + 0.8f); // TODO: Coord
		int y = int(getY() + 0.5f);  // TODO: Coord

		if (d6World.isWall(x1, y - 1, true) && !d6World.isWall(x1, y, true))
		{
			BONUS_AddDeadManGun(x1, y, *this);
		}
		else
		{
			if (d6World.isWall(x2, y - 1, true) && !d6World.isWall(x2, y, true))
			{
				BONUS_AddDeadManGun(x2, y, *this);
			}
		}
	}

	Player& Player::adjustLife(Float32 life)
	{
		State.life = std::max(0.0f, std::min(Float32(D6_MAX_LIFE), State.life + life));
		return *this;
	}

	void Player::checkWater(Float32 elapsedTime)
	{
		Float32 airHitAmount = D6_WATER_HIT * elapsedTime;
		unsetFlag(FlagUnderWater);

		// Check if head is in water
		WaterType water = d6World.getWaterType(Int32(getX() + 0.5f), Int32(getY() + 0.8f));   // TODO: Coord
		if (water != WaterType::None)
		{
			setFlag(FlagUnderWater);
			if ((State.air -= airHitAmount) < 0)
			{
				State.air = 0;
				if (hit(airHitAmount))
				{
					Person().setKills(Person().getKills() - 1);  // Player drowned = -1 kill
				}
			}
			return;
		}

		State.air = MY_Min(State.air + 2 * airHitAmount, D6_MAX_AIR);

		// Check if foot is in water
		water = d6World.getWaterType(Int32(getX() + 0.5f), Int32(getY() + 0.1f));  // TODO: Coord
		if (water != WaterType::None && !State.feetInWater)
		{
			Sprite waterSplash(wtAnim[(water == WaterType::Blue) ? 0 : 1], d6WpnTextures);
			waterSplash.setPosition(getX(), getY(), 0.5f)
				.setLooping(AnimationLooping::OnceAndRemove);
			d6SpriteList.addSprite(waterSplash);
			
			SOUND_PlaySample(D6_SND_WATER);
		}

		State.feetInWater = (water != WaterType::None);
	}

	void Player::checkMoveUp()
	{
		Int32 up = (Int32)(getY() + 0.94); // TODO: Coord
		Int32 left = (Int32)(getX() + 0.1f); // TODO: Coord
		Int32 right = (Int32)(getX() + 0.9f); // TODO: Coord

		if (d6World.isWall(left, up, true) || d6World.isWall(right, up, true))
		{
			State.y = (Float32)(up) - 1.0f; // TODO: Coord
			State.jumpPhase = 180.0f;
		}
	}

	void Player::checkMoveDown()
	{
		Int32 down = (Int32)getY(); // TODO: Coord
		Int32 left = (Int32)(getX() + 0.1f); // TODO: Coord
		Int32 right = (Int32)(getX() + 0.9f); // TODO: Coord

		if (d6World.isWall(left, down, true) || d6World.isWall(right, down, true))
		{
			State.y = (Float32)(down) + 1.0001f; // TODO: Coord
			State.jumpPhase = 0.0f;
		}

		checkElevator();
	}

	void Player::checkFall()
	{
		checkElevator();

		if (isOnElevator())
			return;

		Int32 down = (Int32)(getY() - 0.001f); // TODO: Coord
		Int32 left = (Int32)(getX() + 0.1f); // TODO: Coord
		Int32 right = (Int32)(getX() + 0.9f); // TODO: Coord

		if (!d6World.isWall(left, down, true) && !d6World.isWall(right, down, true))
		{
			State.jumpPhase = 180.0f;
		}
	}

	void Player::checkMoveAside()
	{
		Int32 up = (Int32)(getY() + 0.94); // TODO: Coord
		Int32 down = (Int32)getY(); // TODO: Coord

		if (State.velocity < 0)
		{
			Int32 left = (Int32)(getX() + 0.1f); // TODO: Coord
			if (d6World.isWall(left, up, true) || d6World.isWall(left, down, true))
			{
				State.x = (Float32)left + 0.9001f; // TODO: Coord
			}
		}
		else
		{
			Int32 right = (Int32)(getX() + 0.9f); // TODO: Coord
			if (d6World.isWall(right, up, true) || d6World.isWall(right, down, true))
			{
				State.x = (Float32)right - 0.9001f; // TODO: Coord
			}
		}
	}

	void Player::checkElevator()
	{
		const Elevator* elevator = ELEV_CheckMan(*this);

		if (elevator != nullptr)
		{
			State.elevator = elevator;
			State.y = elevator->getY();
			State.jumpPhase = 0.0f;
		}
	}

	void Player::useTemporarySkin(PlayerSkin& skin)
	{
		State.tempSkinDuration = Float32(10 + rand() % 5);
		State.sprite->setTextures(skin.getTextures());
	}

	void Player::switchToOriginalSkin()
	{
		State.tempSkinDuration = 0;
		State.sprite->setTextures(skin->getTextures());
	}
}