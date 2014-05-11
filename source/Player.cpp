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
#include "Elevator.h"
#include "Math.h"

namespace Duel6
{
	static PlayerControls d6Controls[12] =
	{
		// 1
		{ APP_INP_KEY | SDLK_LEFT, APP_INP_KEY | SDLK_RIGHT, APP_INP_KEY | SDLK_UP,
		APP_INP_KEY | SDLK_DOWN, APP_INP_KEY | SDLK_RCTRL, APP_INP_KEY | SDLK_RSHIFT },
		// 2
		{ APP_INP_KEY | SDLK_a, APP_INP_KEY | SDLK_d, APP_INP_KEY | SDLK_w,
		APP_INP_KEY | SDLK_s, APP_INP_KEY | SDLK_q, APP_INP_KEY | SDLK_e },
		// 3
		{ APP_INP_KEY | SDLK_h, APP_INP_KEY | SDLK_k, APP_INP_KEY | SDLK_u,
		APP_INP_KEY | SDLK_j, APP_INP_KEY | SDLK_o, APP_INP_KEY | SDLK_l },
		// 4
		{ APP_INP_KEY | SDLK_KP1, APP_INP_KEY | SDLK_KP3, APP_INP_KEY | SDLK_KP5,
		APP_INP_KEY | SDLK_KP2, APP_INP_KEY | SDLK_KP0, APP_INP_KEY | SDLK_KP_PERIOD },
		// 5
		{ APP_INP_JOY1 | APP_INP_JOY_LEFT, APP_INP_JOY1 | APP_INP_JOY_RIGHT,
		APP_INP_JOY1 | APP_INP_JOY_BUT1, APP_INP_JOY1 | APP_INP_JOY_DOWN,
		APP_INP_JOY1 | APP_INP_JOY_BUT2, APP_INP_JOY1 | APP_INP_JOY_BUT3 },
		// 6
		{ APP_INP_JOY2 | APP_INP_JOY_LEFT, APP_INP_JOY2 | APP_INP_JOY_RIGHT,
		APP_INP_JOY2 | APP_INP_JOY_BUT1, APP_INP_JOY2 | APP_INP_JOY_DOWN,
		APP_INP_JOY2 | APP_INP_JOY_BUT2, APP_INP_JOY2 | APP_INP_JOY_BUT3 },
		// 7
		{ APP_INP_JOY3 | APP_INP_JOY_LEFT, APP_INP_JOY3 | APP_INP_JOY_RIGHT,
		APP_INP_JOY3 | APP_INP_JOY_BUT1, APP_INP_JOY3 | APP_INP_JOY_DOWN,
		APP_INP_JOY3 | APP_INP_JOY_BUT2, APP_INP_JOY3 | APP_INP_JOY_BUT3 },
		// 8
		{ APP_INP_JOY4 | APP_INP_JOY_LEFT, APP_INP_JOY4 | APP_INP_JOY_RIGHT,
		APP_INP_JOY4 | APP_INP_JOY_BUT1, APP_INP_JOY4 | APP_INP_JOY_DOWN,
		APP_INP_JOY4 | APP_INP_JOY_BUT2, APP_INP_JOY4 | APP_INP_JOY_BUT3 },
		// 9
		{ APP_INP_JOY5 | APP_INP_JOY_LEFT, APP_INP_JOY5 | APP_INP_JOY_RIGHT,
		APP_INP_JOY5 | APP_INP_JOY_BUT1, APP_INP_JOY5 | APP_INP_JOY_DOWN,
		APP_INP_JOY5 | APP_INP_JOY_BUT2, APP_INP_JOY5 | APP_INP_JOY_BUT3 },
		// 10
		{ APP_INP_JOY6 | APP_INP_JOY_LEFT, APP_INP_JOY6 | APP_INP_JOY_RIGHT,
		APP_INP_JOY6 | APP_INP_JOY_BUT1, APP_INP_JOY6 | APP_INP_JOY_DOWN,
		APP_INP_JOY6 | APP_INP_JOY_BUT2, APP_INP_JOY6 | APP_INP_JOY_BUT3 },
		// 11
		{ APP_INP_JOY7 | APP_INP_JOY_LEFT, APP_INP_JOY7 | APP_INP_JOY_RIGHT,
		APP_INP_JOY7 | APP_INP_JOY_BUT1, APP_INP_JOY7 | APP_INP_JOY_DOWN,
		APP_INP_JOY7 | APP_INP_JOY_BUT2, APP_INP_JOY7 | APP_INP_JOY_BUT3 },
		// 12
		{ APP_INP_JOY8 | APP_INP_JOY_LEFT, APP_INP_JOY8 | APP_INP_JOY_RIGHT,
		APP_INP_JOY8 | APP_INP_JOY_BUT1, APP_INP_JOY8 | APP_INP_JOY_DOWN,
		APP_INP_JOY8 | APP_INP_JOY_BUT2, APP_INP_JOY8 | APP_INP_JOY_BUT3 }
	};

	/* ==================================================================== */

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

	Player::Player(Person& person, PlayerSkin* skin, Size controls)
		: person(person), skin(skin)
	{
		camera.resize(false, (mval_t)g_vid.gl_fov, float(g_vid.cl_width) / g_vid.cl_height);
		camera.rotate(180.0, 0.0, 0.0);
		this->controls = &d6Controls[controls];
	}

	Player::~Player()
	{
	}

	void Player::prepareForGame()
	{
		findStartingPosition();
		
		Sprite manSprite(noAnim, skin->getTextures());
		manSprite.setPosition(State.X, State.Y, 0.5f);
		State.A = d6SpriteList.addSprite(manSprite);		

		State.weapon = &WPN_GetRandomWeapon();
		Sprite gunSprite(State.weapon->animation, d6WpnTextures);
		gunSprite.setPosition(State.X, State.Y, 0.5f)
			.setLooping(AnimationLooping::OnceAndStop)
			.setFrame(6);
		State.GA = d6SpriteList.addSprite(gunSprite);

		State.Flags = 0;
		State.Speed = 0;
		State.O = Orientation::Left;
		State.J = 0;
		State.SI = 0;
		State.Life = D6_MAX_LIFE;
		State.Air = D6_MAX_AIR;
		State.Ammo = d6AmmoRangeMin + rand() % (d6AmmoRangeMax - d6AmmoRangeMin + 1);
		State.elevator = nullptr;
		State.Bonus = 0;
		State.BD = 0;
		State.SD = 0;
		Person().setGames(Person().getGames() + 1);
		State.InWater = false;
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
						State.X = (Float32)X;
						State.Y = (Float32)Y + 0.0001f;
						return;
					}
				}
			}
		}
	}

	void Player::setView(int x, int y, int w, int h)
	{
		View.X = x;
		View.Y = y;
		View.Width = w;
		View.Height = h;
	}

	void Player::moveLeft(Float32 elapsedTime)
	{
		if (State.Speed >= -D6_PLAYER_MAX_SPEED)
			State.Flags |= D6_FLAG_REQ_LEFT;
		if ((State.Speed -= elapsedTime) < -D6_PLAYER_MAX_SPEED)
			State.Speed = -D6_PLAYER_MAX_SPEED;
		if (State.Speed < 0)
			State.O = Orientation::Left;
	}

	void Player::moveRight(Float32 elapsedTime)
	{
		if (State.Speed <= D6_PLAYER_MAX_SPEED)
			State.Flags |= D6_FLAG_REQ_RIGHT;
		if ((State.Speed += elapsedTime) > D6_PLAYER_MAX_SPEED)
			State.Speed = D6_PLAYER_MAX_SPEED;
		if (State.Speed > 0)
			State.O = Orientation::Right;
	}

	void Player::jump()
	{
		if (!State.J)
		{
			if (KONTR_CanJump(this))
			{
				State.J = 90;
			}
		}
	}

	void Player::fall()
	{
		if (!State.J && !State.Speed)
		{
			State.Flags |= D6_FLAG_KNEE;
		}
		else
		{
			if (State.J > 0 && State.J < 180)
			{
				State.J = 180;
			}
		}
	}

	void Player::pick()
	{
		if (!State.J && !State.Speed && !isOnElevator())
		{
			BONUS_CheckPick(*this);
		}
	}

	Player& Player::pickWeapon(const Weapon& weapon, Int32 bullets)
	{
		State.weapon = &weapon;
		State.Ammo = bullets;
		State.SI = 0;
		State.Flags |= D6_FLAG_PICK;
					
		State.GA->setAnimation(weapon.animation)
			.setFrame(6)
			.setDraw(false);
		State.A->setFrame(0);

		return *this;
	}

	void Player::makeMove(Float32 elapsedTime)
	{
		Float32 coefElapsedTime = elapsedTime * D6_SPEED_COEF;
		Float32 sp = getSpeed() * coefElapsedTime;
		State.elevator = nullptr;

		if (State.J > 0)
		{
			State.J += 2 * coefElapsedTime;
			if (State.J > 270.0)
			{
				State.J = 270.0f;
			}

			State.Y += Math::fastCos(Int32(State.J)) * D6_PLAYER_JUMP_SPEED * sp;

			if (State.J < 180.0f)
			{
				KONTR_Kontr(*this, 1);
			}
			else
			{
				KONTR_Kontr(*this, 2);
			}
		}
		else
		{
			KONTR_Kontr(*this, 3);
		}

		if (State.Speed != 0)
		{
			if (!(State.Flags & D6_FLAG_REQ_RIGHT) && State.Speed > 0)
			{
				State.Speed -= coefElapsedTime;
				if (State.Speed < 0)
				{
					State.Speed = 0;
				}
			}
			if (!(State.Flags & D6_FLAG_REQ_LEFT) && State.Speed < 0)
			{
				State.Speed += coefElapsedTime;
				if (State.Speed > 0)
				{
					State.Speed = 0;
				}
			}

			State.X += State.Speed * D6_PLAYER_ACCEL * sp;
			KONTR_Kontr(*this, 4);
		}

		if (isOnElevator())
		{
			State.elevator->movePlayer(*this, elapsedTime);
		}

		State.Flags &= ~(D6_FLAG_REQ_RIGHT | D6_FLAG_REQ_LEFT);
	}

	Float32 Player::getSpeed() const
	{
		Float32 spd = 1.0f;

		if (isInWater())
		{
			spd *= 0.67f;
		}

		if (State.SD)
		{
			spd *= 0.5f;
		}

		if (State.Bonus == D6_BONUS_SPEED)
		{
			spd *= 1.43f;
		}

		return spd;
	}

	void Player::checkKeys(Float32 elapsedTime)
	{
		if ((State.Flags & (D6_FLAG_DEAD | D6_FLAG_PICK)) != 0)
			return;

		if (!(State.Flags & D6_FLAG_KNEE))
		{
			if (CO_InpIsPressed(controls->Left))
			{
				moveLeft(elapsedTime);
			}
			if (CO_InpIsPressed(controls->Right))
			{
				moveRight(elapsedTime);
			}
			if (CO_InpIsPressed(controls->Up))
			{
				jump();
			}
			if (CO_InpIsPressed(controls->Pick))
			{
				pick();
			}
		}

		if (CO_InpIsPressed(controls->Shoot))
		{
			WPN_Shoot(*this);
		}
		State.Flags &= ~D6_FLAG_KNEE;
		if (CO_InpIsPressed(controls->Down))
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

		checkKeys(elapsedTime * D6_SPEED_COEF);
		makeMove(elapsedTime);
		setAnm();

		// Move intervals
		if (State.SI > 0)
		{
			if ((State.SI -= elapsedTime) <= 0)
			{
				State.SI = 0;
			}
		}

		if (State.BD > 0)
		{
			if ((State.BD -= elapsedTime) <= 0)
			{
				if (State.Bonus == D6_BONUS_INVIS)
				{
					State.A->setAlpha(1);
					State.GA->setAlpha(1);
				}
				State.Bonus = 0;
				State.BD = 0;
			}
		}

		if (State.SD > 0)
		{
			if ((State.SD -= elapsedTime) <= 0)
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
		float   ad = 0.0;
		short   *a;

		if (isDead())
		{
			if (isLying())
			{
				State.A->setPosition(State.X, State.Y)
					.setOrientation(State.O)
					.setAnimation(d6LAnim);;
			}
			else
			{
				State.A->setPosition(State.X, State.Y)
					.setOrientation(State.O)
					.setAnimation(d6NAnim);
			}

			return;
		}

		if (State.Flags & D6_FLAG_PICK)
			a = d6PAnim;
		else if (State.J)
			a = d6JAnim;
		else if (State.Flags & D6_FLAG_KNEE)
		{
			a = d6DAnim;
			ad = 0.2f;
		}
		else if (!State.Speed)
			a = d6SAnim;
		else
			a = d6WAnim;

		State.A->setPosition(State.X, State.Y).setOrientation(State.O).setAnimation(a);
		State.GA->setPosition(State.X, State.Y - ad).setOrientation(State.O);

		if (State.Flags & D6_FLAG_PICK)
		{
			if (State.A->isFinished())
			{
				State.GA->setDraw(true);
				State.Flags &= ~D6_FLAG_PICK;
			}
		}
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

		X = State.X + 0.5f;
		Y = State.Y - 0.5f;

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
				State.Flags &= ~D6_FLAG_LYING;
				EXPL_Add(getX() + 0.5f, getY() + 0.3f, 0.5f, 1.2f, weapon.explosionColor); // TODO: Coord
			}
			return false;
		}

		State.Life -= amount;
		
		if (directHit)
		{			
			shootingPerson.setHits(shootingPerson.getHits() + 1);
		}

		if (State.Life < 1)
		{
			State.Life = 0;
			State.Flags |= D6_FLAG_DEAD | D6_FLAG_LYING;
			
			State.A->setPosition(State.X, State.Y).setLooping(AnimationLooping::OnceAndStop);
			d6SpriteList.removeSprite(State.GA);

			State.O = (shot.getX() < State.X) ? Orientation::Left : Orientation::Right;

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
				State.Flags &= ~D6_FLAG_LYING;
				EXPL_Add(State.X + 0.5f, State.Y + 0.5f, 0.5f, 1.2f, weapon.explosionColor);  // TODO: Coord
			}
			else if (!State.J)
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

		State.Life -= amount;

		if (State.Life < 1)
		{
			State.Life = 0;
			State.Flags |= D6_FLAG_DEAD | D6_FLAG_LYING;
			
			State.A->setPosition(State.X, State.Y).setLooping(AnimationLooping::OnceAndStop);
			d6SpriteList.removeSprite(State.GA);
			d6MessageQueue.add(*this, MY_L("APP00054|Jsi mrtvy"));

			SOUND_PlaySample(D6_SND_DEAD);

			// Add lying weapon
			if (!State.J)
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
		State.Life = std::max(0.0f, std::min(Float32(D6_MAX_LIFE), State.Life + life));
		return *this;
	}

	void Player::checkWater(float elapsedTime)
	{
		float airHitAmount = D6_WATER_HIT * elapsedTime;
		State.Flags &= ~D6_FLAG_INWATER;

		// Check if head is in water
		WaterType water = d6World.getWaterType(Int32(getX() + 0.5f), Int32(getY() + 0.8f));   // TODO: Coord
		if (water != WaterType::None)
		{
			State.Flags |= D6_FLAG_INWATER;
			if ((State.Air -= airHitAmount) < 0)
			{
				State.Air = 0;
				if (hit(airHitAmount))
				{
					Person().setKills(Person().getKills() - 1);  // Player drowned = -1 kill
				}
			}
			return;
		}

		State.Air = MY_Min(State.Air + 2 * airHitAmount, D6_MAX_AIR);

		// Check if foot is in water
		water = d6World.getWaterType(Int32(getX() + 0.5f), Int32(getY() + 0.1f));  // TODO: Coord
		if (water != WaterType::None && !State.InWater)
		{
			Sprite waterSplash(wtAnim[(water == WaterType::Blue) ? 0 : 1], d6WpnTextures);
			waterSplash.setPosition(State.X, State.Y, 0.5f)
				.setLooping(AnimationLooping::OnceAndRemove);
			d6SpriteList.addSprite(waterSplash);
			
			SOUND_PlaySample(D6_SND_WATER);
		}

		State.InWater = (water != WaterType::None);
	}

	Player& Player::assignElevator(const Elevator& elevator)
	{
		State.elevator = &elevator;
		State.Y = elevator.getY();
		State.J = 0;
		return *this;
	}

	void Player::useTemporarySkin(PlayerSkin& skin)
	{
		State.SD = float(10 + rand() % 5);
		State.A->setTextures(skin.getTextures());
	}

	void Player::switchToOriginalSkin()
	{
		State.SD = 0;
		State.A->setTextures(skin->getTextures());
	}
}