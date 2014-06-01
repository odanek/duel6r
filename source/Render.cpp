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

#include "BonusList.h"
#include "ElevatorList.h"
#include "Math.h"
#include "Game.h"
#include "FaceList.h"
#include "TextureManager.h"
#include "Game.h"
#include "InfoMessageQueue.h"
#include "Explosion.h"
#include "Video.h"
#include "Font.h"
#include "Globals.h"

namespace Duel6
{
	void RENDER_SetView(const PlayerView& view)
	{
		glViewport(view.getX(), view.getY(), view.getWidth(), view.getHeight());
	}

	void RENDER_SetView(int x, int y, int width, int height)
	{
		glViewport(x, y, width, height);
	}

	static void RENDER_Water(const FaceList& water)
	{
		glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

		water.render();

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
	}

	static void RENDER_Sprites(const FaceList& sprites)
	{
		glEnable(GL_ALPHA_TEST);
		glDisable(GL_CULL_FACE);

		sprites.render();

		glDisable(GL_ALPHA_TEST);
		glEnable(GL_CULL_FACE);
	}

	static void RENDER_Background(GLuint texture)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2i(0, d6Video.getScreen().getClientHeight());
		glTexCoord2f(1, 0); glVertex2i(d6Video.getScreen().getClientWidth(), d6Video.getScreen().getClientHeight());
		glTexCoord2f(1, 1); glVertex2i(d6Video.getScreen().getClientWidth(), 0);
		glTexCoord2f(0, 1); glVertex2i(0, 0);
		glEnd();

		glDisable(GL_TEXTURE_2D);
	}

	void RENDER_PlayerRankings(const std::vector<Player>& players)
	{
		std::vector<std::string> rankNames;
		std::vector<Int32> rankPoints;
		Size maxNameLength = 0;;

		for (const Player& player : players)
		{
			rankNames.push_back(player.getPerson().getName());
			rankPoints.push_back(player.getPerson().getTotalPoints());
			maxNameLength = MY_Max(maxNameLength, 5 + rankNames.back().size());
		}

		const PlayerView& view = players.front().getView();
		int posX = view.getX() + view.getWidth() - 8 * maxNameLength - 3;
		int posY = view.getY() + view.getHeight() - (players.size() > 4 ? 50 : 20);

		for (Size i = 0; i < players.size(); i++)
		{
			Size best = 0;

			for (Size j = 1; j < players.size() - i; j++)
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

			d6Font.setColor(Color(255, players[i].isDead() ? 0 : 255, 0));
			d6Font.print(posX, posY, rankNames[best].c_str());
			d6Font.printf(posX + 8 * (maxNameLength - 5), posY, "|%4d", rankPoints[best]);

			if (best < players.size() - i - 1)
			{
				rankNames[best] = rankNames[players.size() - i - 1];
				rankPoints[best] = rankPoints[players.size() - i - 1];
			}

			posY -= 16;
		}
	}

	void RENDER_RoundsPlayed(const Game& game)
	{
		int width = 120;
		int x = d6Video.getScreen().getClientWidth() / 2 - width / 2;
		int y = d6Video.getScreen().getClientHeight() - 20;
		
		glBegin(GL_QUADS);
			glColor3f(0.0f, 0.0f, 0.0f);
			glVertex2i(x - 1, y + 17);
			glVertex2i(x + width, y + 17);
			glVertex2i(x + width, y - 1);
			glVertex2i(x - 1, y - 1);
		glEnd();

		d6Font.setColor(Color(255, 255, 255));
		d6Font.printf(x + 8, y, "Rounds: %d|%d", game.getPlayedRounds(), game.getMaxRounds());
	}

	// TODO: Do zvlastni tridy
	static void RENDER_PlayerStatus(const Player& player)
	{
		if (player.isDead())
			return;

		int alpha = 180;
		int al = int((player.getAir() * 125) / D6_MAX_AIR);
		int ll = int((player.getLife() * 125) / D6_MAX_LIFE);
		const int *ibp = player.getInfoBarPosition();

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
		d6Font.setColor(Color(0, 0, 255));
		d6Font.printf(ibp[0] + 5, ibp[1] - 13, "%d", player.getAmmo());
		d6Font.printf(ibp[0] + 76 - 4 * playerName.length(), ibp[1] - 13, playerName.c_str());

		if (player.getBonus() != 0)
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_ALPHA_TEST);
			glBindTexture(GL_TEXTURE_2D, d6TextureManager.get(D6_TEXTURE_BLOCK_KEY)[player.getBonus()]);
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
		int x = d6Video.getScreen().getClientWidth() - 80;
		int y = d6Video.getScreen().getClientHeight() - 20;
 		int fps = (int)d6Video.getFps();
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

		d6Font.setColor(Color(255, 255, 255));
		d6Font.printf(x, y, "FPS - %d", (int)d6Video.getFps());
	}

	static void RENDER_InvulRing(const Player& player)
	{
		float   x, y, X, Y;
		int     p, uh, u;

		x = player.getX() + 0.5f;  // TODO: Coord
		y = player.getY() + 0.5f;  // TODO: Coord
		p = int(player.getBonusDuration() * 30) % 360;

		glColor3ub(255, 0, 0);
		glDisable(GL_TEXTURE_2D);
		glPointSize(2.0f);
		glBegin(GL_POINTS);

		for (uh = p; uh < 360 + p; uh += 15)
		{
			u = uh % 360;
			X = x + 0.7f * Math::fastCos(90 + u);
			Y = y + 0.7f * Math::fastCos(u);
			glVertex3f(X, Y, 0.5f);
		}

		glEnd();
		glEnable(GL_TEXTURE_2D);
		glPointSize(1.0f);
		glColor3ub(255, 255, 255);
	}

	static void RENDER_InvulRings(const std::vector<Player>& players)
	{
		for (const Player& player : players)
		{
			if (player.isInvulnerable())
			{
				RENDER_InvulRing(player);
			}
		}
	}

	static void RENDER_SplitBox(const PlayerView& view)
	{
		glViewport(view.getX() - 2, view.getY() - 2, view.getWidth() + 4, view.getHeight() + 4);
		glColor3f(1, 0, 0);
		glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(0, d6Video.getScreen().getClientHeight());
		glVertex2i(d6Video.getScreen().getClientWidth(), d6Video.getScreen().getClientHeight());
		glVertex2i(d6Video.getScreen().getClientWidth(), 0);
		glEnd();
		glColor3f(1, 1, 1);
	}

	static void RENDER_View(const Game& game, const Player& player)
	{
		glLoadIdentity();
		player.getCamera().look();

		if (d6Wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		game.getWorld().getWalls().render();
		RENDER_Sprites(game.getWorld().getSprites());
		ELEV_DrawAll();
		d6SpriteList.render();
		BONUS_DrawAll();
		RENDER_InvulRings(game.getPlayers());
		RENDER_Water(game.getWorld().getWater());
		EXPL_DrawAll();

		if (d6Wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	static void RENDER_FullScreen(const Game& game)
	{
		if (game.hasWinner())
		{
			Color overlayColor = game.getGameOverOverlay();
			glColor3ub(overlayColor.getRed(), overlayColor.getGreen(), overlayColor.getBlue());
		}

		const Player& player = game.getPlayers().front();
		RENDER_SetView(player.getView());
		RENDER_Background(game.getWorld().getBackgroundTexture());
		d6Video.setMode(Video::Mode::Perspective);
		RENDER_View(game, player);
	}

	static void RENDER_SplitScreen(const Game& game)
	{
		for (const Player& player : game.getPlayers())
		{
			d6Video.setMode(Video::Mode::Orthogonal);
			RENDER_SplitBox(player.getView());

			if (player.isDead())
				glColor3f(1.0f, 0.5f, 0.5f);

			RENDER_SetView(player.getView());
			RENDER_Background(game.getWorld().getBackgroundTexture());

			d6Video.setMode(Video::Mode::Perspective);
			RENDER_View(game, player);

			glColor3f(1, 1, 1);
		}
	}

	void RENDER_InitScreen()
	{
		glDrawBuffer(GL_FRONT_AND_BACK);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawBuffer(GL_BACK);
	}

	void RENDER_DrawScene(const Game& game)
	{
		if (d6Wireframe || game.getScreenMode() == ScreenMode::SplitScreen)
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			glClear(GL_DEPTH_BUFFER_BIT);

		glColor3f(1.0f, 1.0f, 1.0f);
		if (game.getScreenMode() == ScreenMode::FullScreen)
		{
			RENDER_FullScreen(game);
		}
		else
		{
			RENDER_SplitScreen(game);
		}

		d6Video.setMode(Video::Mode::Orthogonal);
		RENDER_SetView(0, 0, d6Video.getScreen().getClientWidth(), d6Video.getScreen().getClientHeight());
		glColor3f(1.0f, 1.0f, 1.0f);
		
		for (const Player& player : game.getPlayers())
		{
			RENDER_PlayerStatus(player);
		}

		if (d6ShowFps)
		{
			RENDER_FpsCounter();
		}

		if (game.getScreenMode() == ScreenMode::FullScreen)
		{
			d6MessageQueue.renderAllMessages(game.getPlayers().front().getView(), (game.getPlayers().size() > 4 ? 50 : 20));
		}
		else
		{
			for (const Player& player : game.getPlayers())
			{
				d6MessageQueue.renderPlayerMessages(player);
			}
		}

		if (d6ShowRanking && game.getScreenMode() == ScreenMode::FullScreen)
		{
			RENDER_PlayerRankings(game.getPlayers());
		}
		
		if (game.getMaxRounds() > 0)
		{
			RENDER_RoundsPlayed(game);
		}

	}
}