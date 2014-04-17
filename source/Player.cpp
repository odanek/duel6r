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

#include <stdlib.h>
#include "project.h"
#include "Player.h"
#include "anims.h"

namespace Duel6
{
	static  d6KEYBOARD_s    d6Keyboard[12] =
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

	static short    noAnim[4] = { 0, 50, 0, -1 };
	extern short    wtAnim[2][24];
	extern short    d6WpnAnm[D6_WEAPONS][16];
	extern myUINT   *d6WpnTexture;

	static void PLAYER_FindStart(float *x, float *y)
	{
		d6LEVEL *l = &d6World.Level;
		int     X, Y, y2, s = 1;

		while (s)
		{
			X = rand() % l->SizeX;
			Y = rand() % l->SizeY;
			s = 1;

			if (D6_BlockZ(X, Y) != D6_ANM_F_BLOCK)
			{
				y2 = Y - 1;

				while (++y2 < l->SizeY)
				{
					if (D6_BlockZ(X, y2) == D6_ANM_F_WATER)
						break;
					if (D6_BlockZ(X, y2) == D6_ANM_F_BLOCK)
					{
						*x = (float)X;
						*y = (float)(l->SizeY - Y) + 0.0001f;
						return;
					}
				}
			}
		}
	}

	void PLAYER_View(int i, int x, int y)
	{
		int w = g_vid.cl_width / 2 - 4, h = g_vid.cl_height / 2 - 4;

		d6Player[i]->SetView(x, y, w, h);
		d6Player[i]->State.IBP[0] = x + w / 2 - 76;
		d6Player[i]->State.IBP[1] = y + 30;
	}

	void PLAYER_PrepareViews(ScreenMode screenMode)
	{
		int     xShift = (g_vid.cl_width / 4) / 2 - 70;

		for (Size i = 0; i < d6Playing; i++)
		{
			d6Player[i]->PrepareCam(screenMode);
		}

		if (screenMode == ScreenMode::FullScreen)
		{
			for (Size i = 0; i < d6Playing; i++)
			{
				d6Player[i]->SetView(0, 0, g_vid.cl_width, g_vid.cl_height);

				if (i < 4)
				{
					d6Player[i]->State.IBP[0] = (g_vid.cl_width / 4) * i + xShift;
					d6Player[i]->State.IBP[1] = 30;
				}
				else
				{
					d6Player[i]->State.IBP[0] = (g_vid.cl_width / 4) * (i - 4) + xShift;
					d6Player[i]->State.IBP[1] = g_vid.cl_height - 7;
				}
			}

			return;
		}

		if (d6Playing == 2)
		{
			PLAYER_View(0, g_vid.cl_width / 4 + 2, 2);
			PLAYER_View(1, g_vid.cl_width / 4 + 2, g_vid.cl_height / 2 + 2);
		}

		if (d6Playing == 3)
		{
			PLAYER_View(0, 2, 2);
			PLAYER_View(1, g_vid.cl_width / 2 + 2, 2);
			PLAYER_View(2, g_vid.cl_width / 4 + 2, g_vid.cl_height / 2 + 2);
		}

		if (d6Playing == 4)
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

	Player::Player(size_t index)
	{
		Camera = new mycam_c;
		Camera->resize(false, (mval_t)g_vid.gl_fov, float(g_vid.cl_width) / g_vid.cl_height);
		MY_RegMem(Camera, sizeof (mycam_c));
		Camera->rotate(180.0, 0.0, 0.0);
		Keys = &d6Keyboard[index];
		m_skin = nullptr;
	}

	Player::~Player(void)
	{
		FreeSkin();
		MY_UnregMem(Camera);
		delete Camera;
	}

	Player& Player::SetSkin(const PlayerSkinColors& skinColors)
	{
		FreeSkin();
		m_skin = new PlayerSkin(skinColors);
		return *this;
	}

	void Player::FreeSkin()
	{
		if (m_skin != nullptr)
		{
			delete m_skin;
			m_skin = nullptr;
		}
	}

	void Player::PrepareForGame()
	{
		PLAYER_FindStart(&State.X, &State.Y);
		
		Sprite manSprite(noAnim, m_skin->GlTexture());
		manSprite.SetPosition(State.X, State.Y, 0.5f);
		State.A = d6SpriteList.AddSprite(manSprite);		

		State.GN = WPN_GetRandomWeapon();
		Sprite gunSprite(d6WpnAnm[State.GN], d6WpnTexture);
		gunSprite.SetPosition(State.X, State.Y, 0.5f)
			.SetLooping(AnimationLooping::OnceAndStop)
			.SetFrame(6);
		State.GA = d6SpriteList.AddSprite(gunSprite);

		State.Flags = D6_FLAG_NONE;
		State.Speed = 0;
		State.O = Orientation::Left;
		State.J = 0;
		State.SI = 0;
		State.Life = D6_MAX_LIFE;
		State.Air = D6_MAX_AIR;
		State.Ammo = d6AmmoRangeMin + rand() % (d6AmmoRangeMax - d6AmmoRangeMin + 1);
		State.Elev = -1;
		State.Bonus = 0;
		State.BD = 0;
		State.SD = 0;
		Person().SetGames(Person().Games() + 1);
		State.InWater = false;
	}

	void Player::SetControls(int n)
	{
		if (n >= 0 && n <= 12)
			Keys = &d6Keyboard[n];
	}

	void Player::SetView(int x, int y, int w, int h)
	{
		View.X = x;
		View.Y = y;
		View.Width = w;
		View.Height = h;
	}

	void Player::Left(float elapsedTime)
	{
		if (State.Speed >= -D6_PLAYER_MAX_SPEED)
			State.Flags |= D6_FLAG_REQ_LEFT;
		if ((State.Speed -= elapsedTime) < -D6_PLAYER_MAX_SPEED)
			State.Speed = -D6_PLAYER_MAX_SPEED;
		if (State.Speed < 0)
			State.O = Orientation::Left;
	}

	void Player::Right(float elapsedTime)
	{
		if (State.Speed <= D6_PLAYER_MAX_SPEED)
			State.Flags |= D6_FLAG_REQ_RIGHT;
		if ((State.Speed += elapsedTime) > D6_PLAYER_MAX_SPEED)
			State.Speed = D6_PLAYER_MAX_SPEED;
		if (State.Speed > 0)
			State.O = Orientation::Right;
	}

	void Player::Jump(void)
	{
		if (!State.J)
		{
			if (KONTR_CanJump(this))
			{
				State.J = 90;
			}
		}
	}

	void Player::Fall(void)
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

	void Player::Pick(void)
	{
		if (!State.J && !State.Speed && State.Elev == -1)
			BONUS_Pick(*this);
	}

	void Player::MakeMove(float elapsedTime)
	{
		float   sp;

		if (State.Flags & D6_FLAG_INWATER)
			sp = 0.67f;
		else
			sp = 1.0f;

		if (State.SD)
			sp *= 0.5;

		if (State.Bonus == D6_BONUS_SPEED)
			sp *= 1.43f;

		sp *= elapsedTime;
		State.Elev = -1;

		if (State.J > 0)
		{
			if ((State.J += 2 * elapsedTime) > 270.0)
				State.J = 270.0f;

			State.Y += d6Sin[int(State.J)] * D6_PLAYER_JUMP_SPEED * sp;

			if (State.J < 180.0f)
				KONTR_Kontr(*this, 1);
			else
				KONTR_Kontr(*this, 2);
		}
		else
			KONTR_Kontr(*this, 3);

		if (State.Speed != 0)
		{
			if (!(State.Flags & D6_FLAG_REQ_RIGHT) && State.Speed > 0)
				if ((State.Speed -= elapsedTime) < 0)
					State.Speed = 0;
			if (!(State.Flags & D6_FLAG_REQ_LEFT) && State.Speed < 0)
				if ((State.Speed += elapsedTime) > 0)
					State.Speed = 0;

			State.X += State.Speed * D6_PLAYER_ACCEL * sp;
			KONTR_Kontr(*this, 4);
		}

		ELEV_MoveMan(*this, elapsedTime);

		State.Flags &= ~(D6_FLAG_REQ_RIGHT | D6_FLAG_REQ_LEFT);
	}

	void Player::CheckKeys(float elapsedTime)
	{
		if ((State.Flags & (D6_FLAG_DEAD | D6_FLAG_PICK)) != 0)
			return;

		if (!(State.Flags & D6_FLAG_KNEE))
		{
			if (CO_InpIsPressed(Keys->Left))
			{
				Left(elapsedTime);
			}
			if (CO_InpIsPressed(Keys->Right))
			{
				Right(elapsedTime);
			}
			if (CO_InpIsPressed(Keys->Up))
			{
				Jump();
			}
			if (CO_InpIsPressed(Keys->Pick))
			{
				Pick();
			}
		}

		if (CO_InpIsPressed(Keys->Shoot))
		{
			WPN_Shoot(*this);
		}
		State.Flags &= ~D6_FLAG_KNEE;
		if (CO_InpIsPressed(Keys->Down))
		{
			Fall();
		}
	}

	void Player::Update(float elapsedTime)
	{
		CheckWater(d6World.Level, elapsedTime);
		if (!IsDead())
		{
			BONUS_Check(*this);
		}

		CheckKeys(elapsedTime * D6_SPEED_COEF);
		MakeMove(elapsedTime * D6_SPEED_COEF);
		SetAnm();

		// Move intervals
		if (State.SI > 0)
		{
			if ((State.SI -= elapsedTime * D6_SPEED_COEF) <= 0)
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
					State.A->SetAlpha(1);
					State.GA->SetAlpha(1);
				}
				State.Bonus = 0;
				State.BD = 0;
			}
		}

		if (State.SD > 0)
		{
			if ((State.SD -= elapsedTime) <= 0)
			{
				SwitchToOriginalSkin();
			}
		}
	}

	void Player::SetAnm(void)
	{
		float   ad = 0.0;
		short   *a;

		if (IsDead())
		{
			if (IsLying())
			{
				State.A->SetPosition(State.X, State.Y)
					.SetOrientation(State.O)
					.SetAnimation(d6LAnim);;
			}
			else
			{
				State.A->SetPosition(State.X, State.Y)
					.SetOrientation(State.O)
					.SetAnimation(d6NAnim);
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

		State.A->SetPosition(State.X, State.Y).SetOrientation(State.O).SetAnimation(a);
		State.GA->SetPosition(State.X, State.Y - ad).SetOrientation(State.O);

		if (State.Flags & D6_FLAG_PICK)
		{
			if (State.A->IsFinished())
			{
				State.GA->SetDraw(true);
				State.Flags &= ~D6_FLAG_PICK;
			}
		}
	}

	void Player::PrepareCam(ScreenMode screenMode)
	{
		float   fovX, fovY, mZ, dX = 0.0, dY = 0.0;
		d6LEVEL *l = &d6World.Level;

		CamPos.Pos.x = l->SizeX / 2.0f;
		CamPos.Pos.y = l->SizeY / 2.0f;

		fovY = (float)tan(MM_D2R(g_vid.gl_fov) / 2.0f);
		fovX = g_vid.gl_aspect * fovY;

		if (screenMode == ScreenMode::FullScreen)
		{
			if (l->SizeX > g_vid.gl_aspect * l->SizeY)
				dX = (float)l->SizeX;
			else
				dY = (float)l->SizeY;
		}
		else if (l->SizeX > l->SizeY)
		{
			if (d6ZoomBlc > l->SizeY)
				dX = (float)l->SizeY;
			else
				dX = (float)d6ZoomBlc;
		}
		else
		{
			if (d6ZoomBlc > l->SizeX)
				dX = (float)l->SizeX;
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

		CamPos.Pos.z = mZ + 1.0f;
		CamPos.Left = CamPos.Pos.x - dX / 2.0f;
		CamPos.Right = CamPos.Pos.x + dX / 2.0f;
		CamPos.Down = CamPos.Pos.y - dY / 2.0f;
		CamPos.Up = CamPos.Pos.y + dY / 2.0f;
		CamPos.TolX = (dX * D6_CAM_TOLPER_X) / 200.0f;
		CamPos.TolY = (dY * D6_CAM_TOLPER_Y) / 200.0f;

		Camera->setpos(CamPos.Pos);

		if (screenMode == ScreenMode::SplitScreen)
		{
			UpdateCam();
		}
	}

	void Player::UpdateCam(void)
	{
		d6LEVEL *l = &d6World.Level;
		float   mX = 0.0, mY = 0.0, X, Y;

		X = State.X + 0.5f;
		Y = State.Y - 0.5f;

		if (X < CamPos.Pos.x - CamPos.TolX)
		{
			mX = X - (CamPos.Pos.x - CamPos.TolX);
			if (CamPos.Left + mX < 0.0f)
				mX = -CamPos.Left;
		}
		else if (X > CamPos.Pos.x + CamPos.TolX)
		{
			mX = X - (CamPos.Pos.x + CamPos.TolX);
			if (CamPos.Right + mX >(float) l->SizeX)
				mX = (float)l->SizeX - CamPos.Right;
		}
		if (Y < CamPos.Pos.y - CamPos.TolY)
		{
			mY = Y - (CamPos.Pos.y - CamPos.TolY);
			if (CamPos.Down + mY < 0.0f)
				mY = -CamPos.Down;
		}
		else if (Y > CamPos.Pos.y + CamPos.TolY)
		{
			mY = Y - (CamPos.Pos.y + CamPos.TolY);
			if (CamPos.Up + mY >(float) l->SizeY)
				mY = (float)l->SizeY - CamPos.Up;
		}

		if (mX != 0.0)
		{
			CamPos.Left += mX;
			CamPos.Right += mX;
			CamPos.Pos.x += mX;
		}
		if (mY != 0.0)
		{
			CamPos.Up += mY;
			CamPos.Down += mY;
			CamPos.Pos.y += mY;
		}
		if (mX != 0.0 || mY != 0.0)
			Camera->setpos(CamPos.Pos);
	}

	bool Player::Hit(float pw, d6SHOT_s *s, bool hit)
	{
		if (State.Bonus == D6_BONUS_INVUL)
			return false;
		if (IsDead() && !IsLying())
			return false;
		if (IsDead() && s == NULL)
			return false;

		if (s != NULL && hit)
		{
			if (s->WD->Blood)
			{
				EXPL_Add(State.X + 0.3f + (rand() % 40) * 0.01f, s->Y - 0.15f, 0.2f, 0.5f, 0xFF0000);
			}
		}

		if (IsDead())
		{
			if (s->WD->ExplC && hit)
			{
				State.Flags &= ~D6_FLAG_LYING;
				EXPL_Add(State.X + 0.5f, State.Y - 0.7f, 0.5f, 1.2f, s->WD->ExplC);
			}
			return false;
		}

		State.Life -= pw;
		
		if (hit && s != NULL)
		{
			Duel6::Person& shootingPerson = s->Author->Person();
			shootingPerson.SetHits(shootingPerson.Hits() + 1);
		}

		if (State.Life < 1)
		{
			State.Life = 0;
			State.Flags |= D6_FLAG_DEAD | D6_FLAG_LYING;
			
			State.A->SetPosition(State.X, State.Y).SetLooping(AnimationLooping::OnceAndStop);
			d6SpriteList.RemoveSprite(State.GA);

			if (s != NULL)
			{
				State.O = (s->X < State.X) ? Orientation::Left : Orientation::Right;

				if (this != s->Author)
				{
					Duel6::Person& shootingPerson = s->Author->Person();
					Duel6::Person& killedPerson = this->Person();					
					shootingPerson.SetKills(shootingPerson.Kills() + 1);
					d6MessageQueue.Add(*this, MY_L("APP00051|Jsi mrtvy - zabil te %s"), shootingPerson.Name().c_str());
					d6MessageQueue.Add(*s->Author, MY_L("APP00052|Zabil jsi hrace %s"), killedPerson.Name().c_str());
				}
				else
					d6MessageQueue.Add(*this, MY_L("APP00053|Jsi mrtvy"));

				if (s->WD->ExplC && hit)
				{
					State.Flags &= ~D6_FLAG_LYING;
					EXPL_Add(State.X + 0.5f, State.Y - 0.5f, 0.5f, 1.2f, s->WD->ExplC);
				}
			}
			else
				d6MessageQueue.Add(*this, MY_L("APP00054|Jsi mrtvy"));

			SOUND_PlaySample(D6_SND_DEAD);

			// Add lying weapon
			if (!State.J && (s == NULL || !s->WD->ExplC || !hit))
			{
				int x1 = int(X() + 0.2f), x2 = int(X() + 0.8f);
				int y = d6World.Level.SizeY - int(Y() - 0.5f) - 1;

				if (D6_BlockZ(x1, y + 1) == D6_ANM_F_BLOCK && D6_BlockZ(x1, y) != D6_ANM_F_BLOCK)
					BONUS_AddDeadManGun(x1, y, *this);
				else
					if (D6_BlockZ(x2, y + 1) == D6_ANM_F_BLOCK && D6_BlockZ(x2, y) != D6_ANM_F_BLOCK)
						BONUS_AddDeadManGun(x2, y, *this);
			}
		}
		else if (hit)
		{
			SOUND_PlaySample(D6_SND_HIT);
		}

		return IsDead();
	}

	static Uint8 WaterBlock(const d6LEVEL& level, float X, float Y) // 0 = no water, 1 = blue water, 2 = red water TODO: Method of Level?
	{
		int bX = int(X), bY = level.SizeY - int(Y) - 1;

		if (bX >= 0 && bY >= 0 && bX < level.SizeX && bY < level.SizeY)
		{
			if (D6_BlockZ(bX, bY) == D6_ANM_F_WATER)
			{
				return (D6_BlockN(bX, bY) == 4) ? 1 : 2;
			}
		}

		return 0;
	}

	void Player::CheckWater(const d6LEVEL& level, float elapsedTime)
	{
		float airHitAmount = D6_WATER_HIT * elapsedTime;
		State.Flags &= ~D6_FLAG_INWATER;

		// Check if head is in water
		Uint8 water = WaterBlock(level, X() + 0.5f, Y() - 0.2f);
		if (water > 0)
		{
			State.Flags |= D6_FLAG_INWATER;
			if ((State.Air -= airHitAmount) < 0)
			{
				State.Air = 0;
				if (Hit(airHitAmount, NULL, false))
				{
					Person().SetKills(Person().Kills() - 1);  // Player drowned = -1 kill
				}
			}
			return;
		}

		State.Air = MY_Min(State.Air + 2 * airHitAmount, D6_MAX_AIR);

		// Check if foot is in water
		water = WaterBlock(level, X() + 0.5f, Y() - 0.9f);
		if (water > 0 && !State.InWater)
		{
			Sprite waterSplash(wtAnim[water - 1], d6WpnTexture);
			waterSplash.SetPosition(State.X, State.Y, 0.5f)
				.SetLooping(AnimationLooping::OnceAndRemove);
			d6SpriteList.AddSprite(waterSplash);
			
			SOUND_PlaySample(D6_SND_WATER);
		}

		State.InWater = (water > 0);
	}

	float Player::X() const
	{
		return State.X;
	}

	float Player::Y() const
	{
		return State.Y;
	}

	float Player::Width() const
	{
		return 1.0f;
	}

	float Player::Height() const
	{
		return 1.0f;
	}

	bool Player::HasPowerfulShots() const
	{
		return State.Bonus == D6_BONUS_SHOTP;
	}

	bool Player::IsKneeling() const
	{
		return (State.Flags & D6_FLAG_KNEE) != 0;
	}

	bool Player::IsLying() const
	{
		return (State.Flags & D6_FLAG_LYING) != 0;
	}

	bool Player::IsDead() const
	{
		return (State.Flags & D6_FLAG_DEAD) != 0;
	}

	bool Player::IsInvulnerable() const
	{
		return (State.Bonus == D6_BONUS_INVUL);
	}

	void Player::UseTemporarySkin(PlayerSkin& skin)
	{
		State.SD = float(10 + rand() % 5);
		State.A->SetTextures(skin.GlTexture());
	}

	void Player::SwitchToOriginalSkin()
	{
		State.SD = 0;
		State.A->SetTextures(m_skin->GlTexture());
	}
}