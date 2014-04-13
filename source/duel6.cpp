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

#include "project.h"

namespace Duel6
{
	d6WORLD     d6World;
	float       d6Sin[450];
	myUINT      d6BackTex;
	int         d6Wireframe = 0;
	int         d6ZoomBlc = 13;
	ScreenMode d6ScreenMode = ScreenMode::FullScreen;
	Size        d6Playing = 3;
	float       d6KeyWait = 0;
	bool        d6ShowFps = false;
	Player      *d6Player[D6_MAX_PLAYERS];
	int         d6Winner;
	float		d6GameOverWait;
	bool        d6InMenu = false, d6PlayMusic = false;
	PlayerSkinColors d6PlayerSkin[D6_MAX_PLAYERS];
	int			d6AmmoRangeMin = 15, d6AmmoRangeMax = 15;
	bool        d6ShowRanking = false;
	InfoMessageQueue d6MessageQueue;

	int D6_BlockZ(int x, int y)
	{
		d6LEVEL *l = &d6World.Level;

		if (x < 0 || x >= l->SizeX || y < 0 || y >= l->SizeY)
			return D6_ANM_F_BLOCK;

		return d6World.Anm.Znak[l->Data[y * l->SizeX + x]];
	}

	int D6_BlockN(int x, int y)
	{
		d6LEVEL *l = &d6World.Level;

		if (x < 0 || x >= l->SizeX || y < 0 || y >= l->SizeY)
			return 0;

		return l->Data[y * l->SizeX + x];
	}

	float D6_Sin(int a)
	{
		return d6Sin[abs(a + 90) % 360];
	}

	float D6_Cos(int a)
	{
		return d6Sin[abs(a) % 360];
	}

	void D6_ConSwitchW(con_c *con)
	{
		d6Wireframe = 1 - d6Wireframe;
		if (d6Wireframe)
			con->printf(MY_L("APP00020|Vykreslovaci mod prepnut na dratovy\n"));
		else
			con->printf(MY_L("APP00021|Vykreslovaci mod prepnut na solidni\n"));
	}

	void D6_ConShowFps(con_c *con)
	{
		d6ShowFps = !d6ShowFps;
		if (d6ShowFps)
			con->printf(MY_L("APP00022|Ukazatel fps zapnut\n"));
		else
			con->printf(MY_L("APP00023|Ukazatel fps vypnut\n"));
	}

	void D6_CheckWinner(void)
	{
		int numAlive = 0;
		Player* lastAlive = nullptr;

		for (Size i = 0; i < d6Playing; i++)
		{
			Player& player = *d6Player[i];

			if (!player.IsDead())
			{
				numAlive++;
				lastAlive = &player;
			}
		}

		if (numAlive < 2)
		{
			d6Winner = 1;
			d6GameOverWait = D6_GAME_OVER_WAIT;

			if (numAlive == 1)
			{
				d6MessageQueue.Add(*lastAlive, MY_L("APP00024|Jsi vitez - stiskni ESC pro konec nebo F1 pro novou hru"));
				lastAlive->Person().SetWins(lastAlive->Person().Wins() + 1);
			}
			else
			{
				for (Size i = 0; i < d6Playing; i++)
				{
					d6MessageQueue.Add(*d6Player[i], MY_L("APP00025|Konec hry - bez viteze"));
				}
			}
		}
	}

	void D6_StartGame(const std::string& levelPath)
	{
		bool    mirror = rand() % 2 ? true : false;
		int     av, bonus[D6_BONUS_MAX][3];

		g_app.con->printf(MY_L("APP00060|\n===Nahravam uroven %s===\n"), levelPath.c_str());
		LOADER_LoadWorld(levelPath, &d6World, ANM_MAX, mirror, bonus);
		g_app.con->printf(MY_L("APP00061|...Sirka   : %d\n"), d6World.Level.SizeX);
		g_app.con->printf(MY_L("APP00062|...Vyska   : %d\n"), d6World.Level.SizeY);
		g_app.con->printf(MY_L("APP00063|...Bloku   : %d\n"), d6World.Blocks);
		g_app.con->printf(MY_L("APP00064|...Spritu  : %d\n"), d6World.Sprites);
		g_app.con->printf(MY_L("APP00065|...Voda    : %d\n"), d6World.Waters);

		glVertexPointer(3, GL_FLOAT, sizeof (d6VERTEX), (void *)&d6World.Vertex[0].X);
		glTexCoordPointer(2, GL_FLOAT, sizeof (d6VERTEX), (void *)&d6World.Vertex[0].U);

		d6World.Anm.Wait = 0;
		WATER_Build();
		av = d6World.Faces << 2;
		ANM_Init(&d6World.Vertex[av], av);
		g_app.con->printf(MY_L("APP00066|...Pripravuji hrace\n"));
		
		for (Size i = 0; i < d6Playing; i++)
		{
			d6Player[i]->PrepareForGame();
		}
				
		PLAYER_PrepareViews(d6ScreenMode);
		g_app.con->printf(MY_L("APP00067|...Inicializace urovne\n"));
		WPN_LevelInit();
		KONTR_Init();
		EXPL_Init();
		BONUS_Init(bonus);
		ELEV_Load(levelPath, mirror);
		FIRE_Find();
		RENDER_InitScreen();
		d6Winner = -1;
	}

	void D6_SetGLMode(int mode)
	{
		if (mode == D6_GL_PERSPECTIVE)
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			//gluPerspective (g_vid.gl_fov, g_vid.gl_aspect, g_vid.gl_nearclip, g_vid.gl_farclip);        
			float fovy = MM_D2R(g_vid.gl_fov) / 2;
			float f = cos(fovy) / sin(fovy);

			mat4_c<mval_t> p(0.0f);
			p(0, 0) = f / g_vid.gl_aspect;
			p(1, 1) = f;
			p(2, 2) = (g_vid.gl_nearclip + g_vid.gl_farclip) / (g_vid.gl_nearclip - g_vid.gl_farclip);
			p(3, 2) = (2 * g_vid.gl_nearclip * g_vid.gl_farclip) / (g_vid.gl_nearclip - g_vid.gl_farclip);
			p(2, 3) = -1;
			glMultMatrixf(&p(0, 0));

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glEnable(GL_TEXTURE_2D);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
		}
		else
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0, g_vid.cl_width, 0, g_vid.cl_height, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glDisable(GL_CULL_FACE);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
		}
	}

	static void D6_MoveScene(float elapsedTime)
	{
		CO_InpUpdate();
		
		for (Size i = 0; i < d6Playing; i++)
		{
			Player& player = *d6Player[i];
			
			player.Update();
			if (d6ScreenMode == ScreenMode::SplitScreen)
			{
				player.UpdateCam();
			}
		}

		RENDER_MoveAnm();
		ANM_MoveAll();
		WATER_Move();
		WPN_MoveShots();
		EXPL_MoveAll();
		ELEV_MoveAll();
		d6MessageQueue.Update(elapsedTime);
		BONUS_AddNew();

		// Ochrana pred nekolikanasobnym zmacknutim klavesy
		d6KeyWait -= elapsedTime;
		if (d6KeyWait < 0)
			d6KeyWait = 0;

		// Check if there's a winner
		if (d6Winner == -1)
		{
			D6_CheckWinner();
		}
		else if (d6Winner == 1)
		{
			d6GameOverWait -= elapsedTime;
			if (d6GameOverWait <= 0)
			{
				d6GameOverWait = 0;
				d6Winner = 2;
				SOUND_PlaySample(D6_SND_GAME_OVER);
			}
		}
	}

	static void D6_RenderScene()
	{
		RENDER_DrawScene(d6ScreenMode);
	}

	void D6_GameLoop(void)
	{
		CO_FpsSyncLoops(&D6_MoveScene, &D6_RenderScene);

		if (g_inp.key[SDLK_ESCAPE])
		{
			MENU_SavePH();
			MENU_Start();
		}

		if (!d6KeyWait)
		{
			// Restart game
			if (g_inp.key[SDLK_F1])
			{
				MENU_SavePH();
				MENU_Restart(g_inp.key[SDLK_LSHIFT]);
				return;
			}

			// Switch between fullscreen and split screen mode
			if (g_inp.key[SDLK_F2] && d6Playing < 5)
			{
				d6ScreenMode = (d6ScreenMode == ScreenMode::FullScreen) ? ScreenMode::SplitScreen : ScreenMode::FullScreen;
				PLAYER_PrepareViews(d6ScreenMode);
				RENDER_InitScreen();
				d6KeyWait = APP_FPS_SPEED;
			}

			// Turn on/off player statistics
			if (g_inp.key[SDLK_F4])
			{
				d6ShowRanking = !d6ShowRanking;
				d6KeyWait = APP_FPS_SPEED;
			}

			// Save screenshot
			if (g_inp.key[SDLK_F10])
			{
				UTIL_SaveScreenTga(1);
				d6KeyWait = APP_FPS_SPEED;
			}
		}
	}
}