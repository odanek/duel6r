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
#include "Render.h"

namespace Duel6
{
	void Renderer::setView(const PlayerView& view) const
	{
		glViewport(view.getX(), view.getY(), view.getWidth(), view.getHeight());
	}

	void Renderer::setView(int x, int y, int width, int height) const
	{
		glViewport(x, y, width, height);
	}

	void Renderer::water(const FaceList& water) const
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

	void Renderer::sprites(const FaceList& sprites) const
	{
		glEnable(GL_ALPHA_TEST);
		glDisable(GL_CULL_FACE);

		sprites.render();

		glDisable(GL_ALPHA_TEST);
		glEnable(GL_CULL_FACE);
	}

	void Renderer::background(GLuint texture) const
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2i(0, video.getScreen().getClientHeight());
		glTexCoord2f(1, 0); glVertex2i(video.getScreen().getClientWidth(), video.getScreen().getClientHeight());
		glTexCoord2f(1, 1); glVertex2i(video.getScreen().getClientWidth(), 0);
		glTexCoord2f(0, 1); glVertex2i(0, 0);
		glEnd();

		glDisable(GL_TEXTURE_2D);
	}
	
	void Renderer::playerRankings() const
	{
		std::vector<const Player*> ladder = game.getLadder();
		Size maxNameLength = 0;;

		for (const Player* player : ladder)
		{
			maxNameLength = std::max(maxNameLength, 5 + player->getPerson().getName().size());
		}

		const PlayerView& view = game.getPlayers().front().getView();
		int posX = view.getX() + view.getWidth() - 8 * maxNameLength - 3;
		int posY = view.getY() + view.getHeight() - (ladder.size() > 4 ? 50 : 20);

		for (const Player* player : ladder)
		{			
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

			Color fontColor(255, player->isDead() ? 0 : 255, 0);
			font.print(posX, posY, fontColor, player->getPerson().getName());
			font.print(posX + 8 * (maxNameLength - 5), posY, fontColor, Format("|{0,4}") << player->getPerson().getTotalPoints());

			posY -= 16;
		}
	}

	void Renderer::gameOverSummary() const
	{
		int width = 200;
		int height = 50 + game.getPlayers().size() * 16;
		int x = video.getScreen().getClientWidth() / 2 - width / 2;
		int y = video.getScreen().getClientHeight() / 2 - height / 2;

		glColor4f(1, 1, 1, 0.7f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
		glBegin(GL_QUADS);
			glVertex2i(x - 2, y + height + 2);
			glVertex2i(x + width + 2, y + height + 2);
			glVertex2i(x + width + 2, y - 2);
			glVertex2i(x - 2, y - 2);
		glEnd();

		glColor4f(0, 0, 1, 0.7f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
		glBegin(GL_QUADS);
			glVertex2i(x, y + height);
			glVertex2i(x + width, y + height);
			glVertex2i(x + width, y );
			glVertex2i(x, y);
		glEnd();

		Color fontColor(255, 255, 255);
		font.print(x + width / 2 - 35, y + height - 20, fontColor, D6_L("Game Over"));
		
		int count = 0;
		int ladderY = y + height - 50;
		for (const Player* player: game.getLadder())
		{
			font.print(x + 10, ladderY - 16*count, fontColor, player->getPerson().getName());
			font.print(x + width - 40, ladderY - 16*count, fontColor, Format("{0,4}") << player->getPerson().getTotalPoints());
			count++;
		}		
	}

	void Renderer::roundsPlayed() const
	{
		int width = 135;
		int x = video.getScreen().getClientWidth() / 2 - width / 2;
		int y = video.getScreen().getClientHeight() - 20;
		
		glBegin(GL_QUADS);
			glColor3f(0.0f, 0.0f, 0.0f);
			glVertex2i(x - 1, y + 17);
			glVertex2i(x + width, y + 17);
			glVertex2i(x + width, y - 1);
			glVertex2i(x - 1, y - 1);
		glEnd();

		font.print(x + 8, y, Color(255, 255, 255), Format(D6_L("Rounds: {0,3}|{1,3}")) << game.getPlayedRounds() << game.getMaxRounds());
	}

	void Renderer::playerStatus(const Player& player) const
	{
		if (player.isDead())
		{
			return;
		}

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
		Color fontColor(0, 0, 255);
		font.print(ibp[0] + 5, ibp[1] - 13, fontColor, std::to_string(player.getAmmo()));
		font.print(ibp[0] + 76 - 4 * playerName.length(), ibp[1] - 13, fontColor, playerName);

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

	void Renderer::fpsCounter() const
	{
		std::string fpsCount = Format("FPS - {0}") << Int32(video.getFps());
		Size width = 8 * fpsCount.size() + 2;

		int x = video.getScreen().getClientWidth() - width;
		int y = video.getScreen().getClientHeight() - 20;

		glBegin(GL_QUADS);
			glColor3f(0.0f, 0.0f, 0.0f);
			glVertex2i(x - 1, y + 17);
			glVertex2i(x + width, y + 17);
			glVertex2i(x + width, y - 1);
			glVertex2i(x - 1, y - 1);
		glEnd();

		font.print(x, y, Color(255, 255, 255), fpsCount);
	}

	void Renderer::notifications() const
	{
                float remainingTime = game.getRemainingYouAreHere();
                if(remainingTime <= 0) return;
                
                glColor3ub(255, 0, 0);
                glDisable(GL_TEXTURE_2D);
                glPointSize(4.0f);
                glDisable(GL_DEPTH_TEST);

                for (const Player& player : game.getPlayers())
		{
                        glBegin(GL_POINTS);
                        for (int u = 0; u < 360; u++)
                        {
                                float X = player.getX() + 0.5f + remainingTime * Math::fastCos(90 + u);
                                float Y = player.getY() + 0.5f + remainingTime * Math::fastCos(u);
                                glVertex3f(X, Y, 0.5f);
                        }
                        glEnd();
		}
                glEnable(GL_TEXTURE_2D);
                glPointSize(2.0f);
                glColor3ub(255, 255, 255);
                glEnable(GL_DEPTH_TEST);
	}

	void Renderer::roundKills() const
	{
                glColor3ub(255, 0, 0);
                glDisable(GL_TEXTURE_2D);
                glPointSize(4.0f);
                for (const Player& player : game.getPlayers())
		{
                        glBegin(GL_POINTS);
                        for (int i = 0; i < player.getRoundKills(); i++)
                        {
                                Int32 kills = player.getRoundKills();
                                Float32 X = player.getX() + (i * 0.2) - ((kills - 1) * 0.1) + 0.5;
                                glVertex3f(X, player.getY() + 1.2, 0);
                        }
                        glEnd();
		}
                glEnable(GL_TEXTURE_2D);
                glPointSize(2.0f);
                glColor3ub(255, 255, 255);
	}
        
	void Renderer::invulRing(const Player& player) const
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

	void Renderer::invulRings(const std::vector<Player>& players) const
	{
		for (const Player& player : players)
		{
			if (player.isInvulnerable())
			{
				invulRing(player);
			}
		}
	}

	void Renderer::splitBox(const PlayerView& view) const
	{
		glViewport(view.getX() - 2, view.getY() - 2, view.getWidth() + 4, view.getHeight() + 4);
		glColor3f(1, 0, 0);
		glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(0, video.getScreen().getClientHeight());
		glVertex2i(video.getScreen().getClientWidth(), video.getScreen().getClientHeight());
		glVertex2i(video.getScreen().getClientWidth(), 0);
		glEnd();
		glColor3f(1, 1, 1);
	}

	void Renderer::view(const Player& player) const
	{
		glLoadIdentity();
		player.getCamera().look();

		if (wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		game.getWorld().getWalls().render();
		sprites(game.getWorld().getSprites());
		ELEV_DrawAll();
		d6SpriteList.render();
		BONUS_DrawAll();
		invulRings(game.getPlayers());
		water(game.getWorld().getWater());
                notifications();
                roundKills();

		EXPL_DrawAll();

		if (wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	void Renderer::fullScreen() const
	{
		if (game.hasWinner())
		{
			Color overlayColor = game.getGameOverOverlay();			
			glColor3ub(overlayColor.getRed(), overlayColor.getGreen(), overlayColor.getBlue());			
		}

		const Player& player = game.getPlayers().front();
		setView(player.getView());
		background(game.getWorld().getBackgroundTexture());
		video.setMode(Video::Mode::Perspective);
		view(player);		
	}

	void Renderer::splitScreen() const
	{
		for (const Player& player : game.getPlayers())
		{
			video.setMode(Video::Mode::Orthogonal);
			splitBox(player.getView());

			if (player.isDead())
			{
				glColor3f(1.0f, 0.5f, 0.5f);
			}

			setView(player.getView());
			background(game.getWorld().getBackgroundTexture());

			video.setMode(Video::Mode::Perspective);
			view(player);

			glColor3f(1, 1, 1);
		}
	}

	void Renderer::initScreen()
	{
		glDrawBuffer(GL_FRONT_AND_BACK);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawBuffer(GL_BACK);
	}

	void Renderer::render() const
	{
		if (wireframe || game.getScreenMode() == ScreenMode::SplitScreen)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		glColor3f(1.0f, 1.0f, 1.0f);
		if (game.getScreenMode() == ScreenMode::FullScreen)
		{
			fullScreen();
		}
		else
		{
			splitScreen();
		}

		video.setMode(Video::Mode::Orthogonal);
		setView(0, 0, video.getScreen().getClientWidth(), video.getScreen().getClientHeight());
		glColor3f(1.0f, 1.0f, 1.0f);
		
		for (const Player& player : game.getPlayers())
		{
			playerStatus(player);
		}

		if (showFps)
		{
			fpsCounter();
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

		if (showRanking && game.getScreenMode() == ScreenMode::FullScreen)
		{
			playerRankings();
		}
		
		if (game.getMaxRounds() > 0)
		{
			roundsPlayed();
		}

		if(game.hasWinner() && game.isOver())
		{
			gameOverSummary();
		}
	}
}
