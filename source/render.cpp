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
	void RENDER_SetView(const d6VIEW_s& w)
	{
		glViewport(w.X, w.Y, w.Width, w.Height);
	}

	void RENDER_SetView(int x, int y, int width, int height)
	{
		glViewport(x, y, width, height);
	}

	static void RENDER_Blocks(int start, int blocks)
	{
		const std::vector<GLuint>& ta = d6World.Anm.textures;
		GLuint t;
		d6FACE *df = d6World.Face;
		int i, v, c = 0;

		t = ta[df[start].NowTex];
		v = start << 2;
		blocks += start;

		for (i = start; i < blocks; i++)
			if (ta[df[i].NowTex] != t)
			{
				glBindTexture(GL_TEXTURE_2D, t);
				glDrawArrays(GL_QUADS, v, c);
				t = ta[df[i].NowTex];
				v += c;
				c = 4;
			}
			else
				c += 4;

		glBindTexture(GL_TEXTURE_2D, t);
		glDrawArrays(GL_QUADS, v, c);
	}

	static void RENDER_Water(void)
	{
		if (!d6World.Waters)
			return;

		glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

		RENDER_Blocks(d6World.Blocks + d6World.Sprites, d6World.Waters);

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
	}

	static void RENDER_Sprites(void)
	{
		if (!d6World.Sprites)
			return;

		glEnable(GL_ALPHA_TEST);
		glDisable(GL_CULL_FACE);

		RENDER_Blocks(d6World.Blocks, d6World.Sprites);

		glDisable(GL_ALPHA_TEST);
		glEnable(GL_CULL_FACE);
	}

	void RENDER_MoveAnm(float elapsedTime)
	{
		d6FACE  *f = d6World.Face;
		int     i;

		if ((d6World.Anm.Wait += elapsedTime) > D6_ANM_SPEED)
		{
			d6World.Anm.Wait = 0;
			for (i = 0; i < d6World.Faces; i++)
				if (++f[i].NowTex > f[i].MaxTex)
					f[i].NowTex = f[i].MinTex;
		}
	}

	static void RENDER_Background(void)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, d6BackTex);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2i(0, g_vid.cl_height);
		glTexCoord2f(1, 0); glVertex2i(g_vid.cl_width, g_vid.cl_height);
		glTexCoord2f(1, 1); glVertex2i(g_vid.cl_width, 0);
		glTexCoord2f(0, 1); glVertex2i(0, 0);
		glEnd();

		glDisable(GL_TEXTURE_2D);
	}

	void RENDER_PlayerRankings()
	{
		std::vector<std::string> rankNames;
		std::vector<Int32> rankPoints;
		Size players = d6Players.size(), maxNameLength = 0;;

		for (const Player& player : d6Players)
		{
			rankNames.push_back(player.getPerson().getName());
			rankPoints.push_back(player.getPerson().getTotalPoints());
			maxNameLength = MY_Max(maxNameLength, 5 + rankNames.back().size());
		}

		const d6VIEW_s& view = d6Players.front().View;
		int posX = view.X + view.Width - 8 * maxNameLength - 3;
		int posY = view.Y + view.Height - (players > 4 ? 50 : 20);

		CO_FontColor(255, 255, 0);

		for (Size i = 0; i < players; i++)
		{
			Size best = 0;

			for (Size j = 1; j < players - i; j++)
			{
				if (rankPoints[j] > rankPoints[best])
				{
					best = j;
				}
			}

			glColor4f(0, 0, 1, 0.7f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBegin(GL_QUADS);
				glVertex2i(posX, posY + 15);
				glVertex2i(posX + 8 * maxNameLength, posY + 15);
				glVertex2i(posX + 8 * maxNameLength, posY + 1);
				glVertex2i(posX, posY + 1);
			glEnd();
			glDisable(GL_BLEND);

			CO_FontPrintf(posX, posY, rankNames[best].c_str());
			CO_FontPrintf(posX + 8 * (maxNameLength - 5), posY, "|%4d", rankPoints[best]);

			if (best < players - i - 1)
			{
				rankNames[best] = rankNames[players - i - 1];
				rankPoints[best] = rankPoints[players - i - 1];
			}

			posY -= 16;
		}
	}

	static void RENDER_PlayerStatus(const Player& player)
	{
		if (player.isDead())
			return;

		int alpha = 180;
		int al = int((player.State.Air * 125) / D6_MAX_AIR);
		int ll = int((player.State.Life * 125) / D6_MAX_LIFE);
		const int *ibp = player.State.IBP;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBegin(GL_QUADS);
		glColor4ub(255, 255, 0, alpha);
		glVertex2i(ibp[0], ibp[1] + 2);
		glVertex2i(ibp[0] + 151, ibp[1] + 2);
		glVertex2i(ibp[0] + 151, ibp[1] - 25);
		glVertex2i(ibp[0], ibp[1] - 25);

		glColor4ub(200, 200, 0, alpha);
		glVertex2i(ibp[0] + 23, ibp[1] + 1);
		glVertex2i(ibp[0] + 130, ibp[1] + 1);
		glVertex2i(ibp[0] + 130, ibp[1] - 13);
		glVertex2i(ibp[0] + 23, ibp[1] - 13);

		glColor4ub(255, 0, 0, alpha);
		glVertex2i(ibp[0] + 5, ibp[1] - 14);
		glVertex2i(ibp[0] + 5 + ll, ibp[1] - 14);
		glVertex2i(ibp[0] + 5 + ll, ibp[1] - 18);
		glVertex2i(ibp[0] + 5, ibp[1] - 18);

		glColor4ub(0, 0, 255, alpha);
		glVertex2i(ibp[0] + 5, ibp[1] - 19);
		glVertex2i(ibp[0] + 5 + al, ibp[1] - 19);
		glVertex2i(ibp[0] + 5 + al, ibp[1] - 23);
		glVertex2i(ibp[0] + 5, ibp[1] - 23);
		glEnd();

		const std::string& playerName = player.getPerson().getName();
		CO_FontColor(0, 0, 255);
		CO_FontPrintf(ibp[0] + 5, ibp[1] - 13, "%d", player.State.Ammo);
		CO_FontPrintf(ibp[0] + 76 - 4 * playerName.length(), ibp[1] - 13, playerName.c_str());

		if (player.State.Bonus)
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_ALPHA_TEST);
			glBindTexture(GL_TEXTURE_2D, d6World.Anm.textures[player.State.Bonus]);
			glColor3ub(255, 255, 255);
			glBegin(GL_QUADS);
			glTexCoord2f(0.3f, 0.3f); glVertex2i(ibp[0] + 133, ibp[1] - 3);
			glTexCoord2f(0.7f, 0.3f); glVertex2i(ibp[0] + 148, ibp[1] - 3);
			glTexCoord2f(0.7f, 0.7f); glVertex2i(ibp[0] + 148, ibp[1] - 18);
			glTexCoord2f(0.3f, 0.7f); glVertex2i(ibp[0] + 133, ibp[1] - 18);
			glEnd();
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_ALPHA_TEST);
		}

		glDisable(GL_BLEND);
	}

	static void RENDER_FpsCounter()
	{
		int x = g_vid.cl_width - 80;
		int y = g_vid.cl_height - 20;
		int fps = (int)g_app.fps;
		int length = 0;
		if (fps < 10)
			length = 1;
		else if (fps < 100)
			length = 2;
		else if (fps < 1000)
			length = 3;
		else
			length = 4;
		int width = ((6 + length) << 3) + 2;

		glBegin(GL_QUADS);
			glColor3f(0.0f, 0.0f, 0.0f);
			glVertex2i(x - 1, y + 17);
			glVertex2i(x + width, y + 17);
			glVertex2i(x + width, y - 1);
			glVertex2i(x - 1, y - 1);
		glEnd();

		CO_FontColor(255, 255, 255);
		CO_FontPrintf(x, y, "FPS - %d", (int)g_app.fps);
	}

	static void RENDER_InvulRing(const Player& player)
	{
		float   x, y, X, Y;
		int     p, uh, u;

		x = player.getX() + 0.5f;
		y = player.getY() - 0.5f;
		p = int(player.State.BD * 30) % 360;

		glColor3ub(255, 0, 0);
		glDisable(GL_TEXTURE_2D);
		glPointSize(2.0f);
		glBegin(GL_POINTS);

		for (uh = p; uh < 360 + p; uh += 15)
		{
			u = uh % 360;
			X = x + 0.7f * d6Sin[90 + u];
			Y = y + 0.7f * d6Sin[u];
			glVertex3f(X, Y, 0.5f);
		}

		glEnd();
		glEnable(GL_TEXTURE_2D);
		glPointSize(1.0f);
		glColor3ub(255, 255, 255);
	}

	static void RENDER_InvulRings()
	{
		for (const Player& player : d6Players)
		{
			if (player.isInvulnerable())
			{
				RENDER_InvulRing(player);
			}
		}
	}

	static void RENDER_SplitBox(const d6VIEW_s& view)
	{
		glViewport(view.X - 2, view.Y - 2, view.Width + 4, view.Height + 4);
		glColor3f(1, 0, 0);
		glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(0, g_vid.cl_height);
		glVertex2i(g_vid.cl_width, g_vid.cl_height);
		glVertex2i(g_vid.cl_width, 0);
		glEnd();
		glColor3f(1, 1, 1);
	}

	static void RENDER_View(const Player& player)
	{
		glLoadIdentity();
		player.getCamera().look();

		if (d6Wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		RENDER_Blocks(0, d6World.Blocks);
		RENDER_Sprites();
		ELEV_DrawAll();
		d6SpriteList.render();
		BONUS_DrawAll();
		RENDER_InvulRings();
		RENDER_Water();
		EXPL_DrawAll();

		if (d6Wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	static void RENDER_FullScreen()
	{
		if (d6Winner > 0)
		{
			float overlay = d6GameOverWait / D6_GAME_OVER_WAIT;
			glColor3f(1, overlay, overlay);
		}

		const Player& player = d6Players.front();
		RENDER_SetView(player.View);
		RENDER_Background();
		D6_SetGLMode(D6_GL_PERSPECTIVE);
		RENDER_View(player);
	}

	static void RENDER_SplitScreen()
	{
		for (const Player& player : d6Players)
		{
			D6_SetGLMode(D6_GL_ORTHO);
			RENDER_SplitBox(player.View);

			if (player.isDead())
				glColor3f(1.0f, 0.5f, 0.5f);

			RENDER_SetView(player.View);
			RENDER_Background();

			D6_SetGLMode(D6_GL_PERSPECTIVE);
			RENDER_View(player);

			glColor3f(1, 1, 1);
		}
	}

	void RENDER_InitScreen()
	{
		glDrawBuffer(GL_FRONT_AND_BACK);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawBuffer(GL_BACK);
	}

	void RENDER_DrawScene(ScreenMode screenMode)
	{
		if (d6Wireframe || screenMode == ScreenMode::SplitScreen)
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			glClear(GL_DEPTH_BUFFER_BIT);

		glColor3f(1.0f, 1.0f, 1.0f);
		if (screenMode == ScreenMode::FullScreen)
		{
			RENDER_FullScreen();
		}
		else
		{
			RENDER_SplitScreen();
		}

		D6_SetGLMode(D6_GL_ORTHO);
		RENDER_SetView(0, 0, g_vid.cl_width, g_vid.cl_height);
		glColor3f(1.0f, 1.0f, 1.0f);
		
		for (const Player& player : d6Players)
		{
			RENDER_PlayerStatus(player);
		}

		if (d6ShowFps)
		{
			RENDER_FpsCounter();
		}

		if (screenMode == ScreenMode::FullScreen)
		{
			d6MessageQueue.renderAllMessages(d6Players.front().View);
		}
		else
		{
			for (const Player& player : d6Players)
			{
				d6MessageQueue.renderPlayerMessages(player);
			}
		}

		if (d6ShowRanking && screenMode == ScreenMode::FullScreen)
		{
			RENDER_PlayerRankings();
		}
	}
}