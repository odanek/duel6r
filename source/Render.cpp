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
#include "Game.h"
#include "GameMode.h"
#include "Explosion.h"

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

		water.render(game.getResources().getBlockTextures());

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
	}

	void Renderer::sprites(const FaceList& sprites) const
	{
		glEnable(GL_ALPHA_TEST);
		glDisable(GL_CULL_FACE);

		sprites.render(game.getResources().getBlockTextures());

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
		Ranking ranking = game.getMode().getRanking(game.getPlayers());
		Int32 maxNameLength = 0;

		for (const auto& rankingEntry : ranking)
		{
			maxNameLength = std::max(maxNameLength, Int32(5 + rankingEntry.name.size()));
		}

		const PlayerView& view = game.getPlayers().front().getView();
		int posX = view.getX() + view.getWidth() - 8 * maxNameLength - 3;
		int posY = view.getY() + view.getHeight() - 20;

		for (const auto& rankingEntry : ranking)
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

			Color fontColor(rankingEntry.color);
			font.print(posX, posY, fontColor, rankingEntry.name);
			font.print(posX + 8 * (maxNameLength - 5), posY, fontColor, Format("|{0,4}") << rankingEntry.points);

			posY -= 16;
		}
	}

    void Renderer::roundOverSummary() const
    {
        int width = 200;
        int height = 120;
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
		glDisable(GL_BLEND);

        Color fontColor(255, 255, 255);
        font.print(x + width / 2 - 35, y + height - 30, fontColor, D6_L("Round Over"));

        for (const Player& player : game.getPlayers())
        {
            if (!player.isDead())
            {
                font.print(x + 15, y + height - 70, fontColor, Format(D6_L("Winner is: {0}")) << player.getPerson().getName());
                break;
            }
        }

        font.print(x + width / 2 - 2, y + height - 95, fontColor, Format("{0}") << (Int32) game.getRound().getRemainingGameOverWait());
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
		glDisable(GL_BLEND);

		Color fontColor(255, 255, 255);
		font.print(x + width / 2 - 35, y + height - 20, fontColor, D6_L("Game Over"));
		
		int count = 0;
		int ladderY = y + height - 50;

		Ranking ranking = game.getMode().getRanking(game.getPlayers());
		for (const auto& rankingEntry : ranking)
		{
			font.print(x + 10, ladderY - 16*count, fontColor, rankingEntry.name);
			font.print(x + width - 40, ladderY - 16*count, fontColor, Format("{0,4}") << rankingEntry.points);
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

		font.print(x + 8, y, Color(255, 255, 255), Format(D6_L("Rounds: {0,3}|{1,3}")) << game.getPlayedRounds() << game.getSettings().getMaxRounds());
	}

	void Renderer::playerStatuses() const
	{
		if (game.getSettings().getScreenMode() == ScreenMode::FullScreen)
		{
			glBegin(GL_QUADS);
			glColor3ub(0, 0, 0);
			glVertex2i(0, 40);
			glVertex2i(video.getScreen().getClientWidth(), 40);
			glVertex2i(video.getScreen().getClientWidth(), 0);
			glVertex2i(0, 0);
			glEnd();
		}

		for (const Player& player : game.getPlayers())
		{
			playerStatus(player);
		}
	}

	void Renderer::playerStatus(const Player& player) const
	{
		Int32 alpha = 180, green = player.isDead() ? 0 : 1;
		Int32 airBarLength = Int32((player.getAir() * 101) / D6_MAX_AIR);
		Int32 lifeBarLength = Int32((player.getLife() * 101) / D6_MAX_LIFE);
		Int32 reloadBarLength = 101 - Int32((player.getReloadTime() * 101) / player.getReloadInterval());
		const Int32 *ibp = player.getInfoBarPosition();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBegin(GL_QUADS);
		glColor4ub(255, green * 255, 0, alpha);
		glVertex2i(ibp[0] + 32, ibp[1] + 2);
		glVertex2i(ibp[0] + 157, ibp[1] + 2);
		glVertex2i(ibp[0] + 157, ibp[1] - 32);
		glVertex2i(ibp[0] + 32, ibp[1] - 32);

		glColor4ub(150, green * 150, 0, alpha);
		glVertex2i(ibp[0] + 53, ibp[1] + 1);
		glVertex2i(ibp[0] + 136, ibp[1] + 1);
		glVertex2i(ibp[0] + 136, ibp[1] - 13);
		glVertex2i(ibp[0] + 53, ibp[1] - 13);

		glColor4ub(255, 0, 0, alpha);
		glVertex2i(ibp[0] + 35, ibp[1] - 14);
		glVertex2i(ibp[0] + 35 + lifeBarLength, ibp[1] - 14);
		glVertex2i(ibp[0] + 35 + lifeBarLength, ibp[1] - 18);
		glVertex2i(ibp[0] + 35, ibp[1] - 18);

		glColor4ub(0, 0, 255, alpha);
		glVertex2i(ibp[0] + 35, ibp[1] - 20);
		glVertex2i(ibp[0] + 35 + airBarLength, ibp[1] - 20);
		glVertex2i(ibp[0] + 35 + airBarLength, ibp[1] - 24);
		glVertex2i(ibp[0] + 35, ibp[1] - 24);

		glColor4ub(0, 100, 0, alpha);
		glVertex2i(ibp[0] + 35, ibp[1] - 26);
		glVertex2i(ibp[0] + 35 + reloadBarLength, ibp[1] - 26);
		glVertex2i(ibp[0] + 35 + reloadBarLength, ibp[1] - 30);
		glVertex2i(ibp[0] + 35, ibp[1] - 30);
		glEnd();

		if (game.getSettings().getScreenMode() == ScreenMode::FullScreen)
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_ALPHA_TEST);
			glBindTexture(GL_TEXTURE_2D, player.getSkin().getTextures().getGlTextures()[3]);
			glColor3ub(255, 255, 255);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2i(ibp[0] + 3, ibp[1] + 1);
			glTexCoord2f(1.0f, 0.0f); glVertex2i(ibp[0] + 33, ibp[1] + 1);
			glTexCoord2f(1.0f, 1.0f); glVertex2i(ibp[0] + 33, ibp[1] - 31);
			glTexCoord2f(0.0f, 1.0f); glVertex2i(ibp[0] + 3, ibp[1] - 31);
			glEnd();
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_ALPHA_TEST);
		}

		const std::string& playerName = player.getPerson().getName();
		Color fontColor(0, 0, 255);
		font.print(ibp[0] + 35, ibp[1] - 13, fontColor, std::to_string(player.getAmmo()));
		font.print(ibp[0] + 92 - 4 * playerName.length(), ibp[1] - 13, fontColor, playerName);

		if (player.getBonus() != -1)
		{
			Int32 bonusBarLength = Int32(0.5f + (player.getBonusRemainingTime() * 16) / player.getBonusDuration());

			glEnable(GL_TEXTURE_2D);
			glEnable(GL_ALPHA_TEST);
			glBindTexture(GL_TEXTURE_2D, BONUS_GetTexture(player.getBonus()));
			glColor3ub(255, 255, 255);
			glBegin(GL_QUADS);
				glTexCoord2f(0.3f, 0.3f); glVertex2i(ibp[0] + 139, ibp[1] + 2);
				glTexCoord2f(0.7f, 0.3f); glVertex2i(ibp[0] + 154, ibp[1] + 2);
				glTexCoord2f(0.7f, 0.7f); glVertex2i(ibp[0] + 154, ibp[1] - 13);
				glTexCoord2f(0.3f, 0.7f); glVertex2i(ibp[0] + 139, ibp[1] - 13);
			glEnd();
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_ALPHA_TEST);

			glBegin(GL_QUADS);
				glColor4ub(200, 0, 200, alpha);
				glVertex2i(ibp[0] + 139, ibp[1] - 30 + bonusBarLength);
				glVertex2i(ibp[0] + 154, ibp[1] - 30 + bonusBarLength);
				glVertex2i(ibp[0] + 154, ibp[1] - 30);
				glVertex2i(ibp[0] + 139, ibp[1] - 30);
			glEnd();
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

	void Renderer::youAreHere() const
	{
		Float32 remainingTime = game.getRound().getRemainingYouAreHere();
		if(remainingTime <= 0) return;
                
		glColor3ub(255, 255, 0);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glLineWidth(3.0f);
		
		Float32 radius = 0.5f + 0.5f * std::abs(D6_YOU_ARE_HERE_DURATION / 2 - remainingTime);
		for (const Player& player : game.getPlayers())
		{
			Vector playerCentre = player.getCentre();
			glBegin(GL_LINE_LOOP);
			for (Int32 u = 0; u < 36; u++)
			{				
				Float32 spike = (u % 2 == 0) ? 0.95f : 1.05f;
				Vector pos = playerCentre + spike * radius * Vector::direction(u * 10);
				glVertex3f(pos.x, pos.y, 0.5f);
			}
			glEnd();
		}		

		glEnable(GL_TEXTURE_2D);
		glLineWidth(1.0f);
		glColor3ub(255, 255, 255);
		glEnable(GL_DEPTH_TEST);
	}

	void Renderer::hpBars() const
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_QUADS);

		for (const Player& player : game.getPlayers())
		{
			if (!player.isDead() && player.getHPBarDuration() > 0)
			{
				Rectangle rect = player.getCollisionRect();
				Float32 width = player.getLife() / D6_MAX_LIFE * rect.getSize().x;
				Float32 X = rect.left.x;
				Float32 Y = rect.right.y + 0.25f;

				Float32 alpha = 1.0f;
				if(player.getHPBarDuration() > 2.0f)
				{
					alpha = (D6_PLAYER_HPBAR - player.getHPBarDuration());
				}
				else if(player.getHPBarDuration() < 1.0f)
				{
					alpha = player.getHPBarDuration();
				}

				glColor4f(1, 0, 0, alpha);
				glVertex3f(X, Y, 0.5f);
				glVertex3f(X + width, Y, 0.5f);
				glVertex3f(X + width, Y - 0.1f, 0.5f);
				glVertex3f(X, Y - 0.1f, 0.5f);

				glColor4f(0, 0, 0, alpha);
				glVertex3f(X - 0.03f, Y + 0.03f, 0.5f);
				glVertex3f(X + 1.03f, Y + 0.03f, 0.5f);
				glVertex3f(X + 1.03f, Y - 0.13f, 0.5f);
				glVertex3f(X - 0.03f, Y - 0.13f, 0.5f);
			}
		}

		glEnd();
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glColor3ub(255, 255, 255);
	}

	void Renderer::roundKills() const
	{
		glColor3ub(0, 0, 255);
		glDisable(GL_TEXTURE_2D);
		glPointSize(5.0f);

		glBegin(GL_POINTS);
		for (const Player& player : game.getPlayers())
		{
			if (!player.isDead())
			{
				Float32 width = (2 * player.getRoundKills() - 1) * 0.1f;
				Rectangle rect = player.getCollisionRect();

				Float32 X = rect.getCentre().x + 0.05f - width / 2;
				Float32 Y = rect.right.y + 0.4f;
				for (Int32 i = 0; i < player.getRoundKills(); i++, X += 0.2f)
				{
					glVertex3f(X, Y, 0.5f);
				}
			}
		}
		glEnd();

		glEnable(GL_TEXTURE_2D);
		glPointSize(1.0f);
		glColor3ub(255, 255, 255);
	}
        
	void Renderer::invulRing(const Player& player) const
	{
		Vector playerCentre = player.getCentre();
		Float32 radius = player.getDimensions().length() / 2.0f;
		Int32 p = Int32(player.getBonusRemainingTime() * 30) % 360;

		glColor3ub(255, 0, 0);
		glDisable(GL_TEXTURE_2D);
		glPointSize(2.0f);
		glBegin(GL_POINTS);

		for (Int32 uh = p; uh < 360 + p; uh += 15)
		{
			Int32 u = uh % 360;
			Vector pos = playerCentre + radius * Vector::direction(u);
			glVertex3f(pos.x, pos.y, 0.5f);
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

	void Renderer::infoMessages() const
	{
		const InfoMessageQueue& messageQueue = game.getRound().getWorld().getMessageQueue();

		if (game.getSettings().getScreenMode() == ScreenMode::FullScreen)
		{
			messageQueue.renderAllMessages(game.getPlayers().front().getView(), 20, font);
		}
		else
		{
			for (const Player& player : game.getPlayers())
			{
				messageQueue.renderPlayerMessages(player, font);
			}
		}
	}

	void Renderer::view(const Player& player) const
	{
		glLoadIdentity();
		player.getCamera().look();

		if (game.getSettings().isWireframe())
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		const World& world = game.getRound().getWorld();
		world.getLevelRenderData().getWalls().render(game.getResources().getBlockTextures());
		sprites(world.getLevelRenderData().getSprites());
		ELEV_DrawAll();
		world.getSpriteList().render();
		BONUS_DrawAll();
		invulRings(game.getPlayers());
		water(world.getLevelRenderData().getWater());
		youAreHere();
		roundKills();
		hpBars();

		world.getExplosionList().render();

		if (game.getSettings().isWireframe())
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	Color Renderer::getGameOverOverlay() const
	{
		Uint8 overlay = Uint8(255.0f * game.getRound().getRemainingGameOverWait() / D6_GAME_OVER_WAIT);
		if (game.isOver())
		{
			overlay = (Uint8)std::min(overlay + 51, 255);
			return Color(overlay);
		}

		return Color(255, overlay, overlay);
	}

	void Renderer::fullScreen() const
	{
		if (game.getRound().hasWinner())
		{
			Color overlayColor = getGameOverOverlay();
			glColor3ub(overlayColor.getRed(), overlayColor.getGreen(), overlayColor.getBlue());			
		}

		const Player& player = game.getPlayers().front();
		setView(player.getView());
		background(game.getResources().getBcgTextures().getGlTextures()[game.getRound().getWorld().getBackground()]);
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
			background(game.getResources().getBcgTextures().getGlTextures()[game.getRound().getWorld().getBackground()]);

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
		const GameSettings& settings = game.getSettings();

		if (settings.isWireframe() || settings.getScreenMode() == ScreenMode::SplitScreen)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		glColor3f(1.0f, 1.0f, 1.0f);
		if (settings.getScreenMode() == ScreenMode::FullScreen)
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

		playerStatuses();
		infoMessages();

		if (settings.isShowFps())
		{
			fpsCounter();
		}

		if (settings.isShowRanking() && settings.getScreenMode() == ScreenMode::FullScreen)
		{
			playerRankings();
		}
		
		if (settings.isRoundLimit())
		{
			roundsPlayed();
		}

		if (game.getRound().hasWinner())
		{
            if (game.isOver())
            {
				gameOverSummary();
            }
            else
            {
				roundOverSummary();
            }
        }
    }
}
