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
		setView(view.getX(), view.getY(), view.getWidth(), view.getHeight());
	}

	void Renderer::setView(int x, int y, int width, int height) const
	{
		TheRenderer->setViewport(x, y, width, height);
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
		glAlphaFunc(GL_GEQUAL, 1);
		glDisable(GL_CULL_FACE);

		sprites.render(game.getResources().getBlockTextures());

		glDisable(GL_ALPHA_TEST);
		glEnable(GL_CULL_FACE);
	}

	void Renderer::background(Texture texture) const
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture.getId());

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

        Color fontColor = Color::WHITE;
        font.print(x + width / 2 - 35, y + height - 30, fontColor, "Round Over");

        for (const Player& player : game.getPlayers())
        {
            if (player.isAlive())
            {
                font.print(x + 15, y + height - 70, fontColor, Format("Winner is: {0}") << player.getPerson().getName());
                break;
            }
        }

        font.print(x + width / 2 - 2, y + height - 95, fontColor, Format("{0}") << (Int32) game.getRound().getRemainingGameOverWait());
    }

	void Renderer::gameOverSummary() const
	{
		Int32 width = 200;
		Int32 height = 50 + Int32(game.getPlayers().size()) * 16;
		Int32 x = video.getScreen().getClientWidth() / 2 - width / 2;
		Int32 y = video.getScreen().getClientHeight() / 2 - height / 2;

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

		Color fontColor = Color::WHITE;
		font.print(x + width / 2 - 35, y + height - 20, fontColor, "Game Over");
		
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

		font.print(x + 8, y, Color::WHITE, Format("Rounds: {0,3}|{1,3}") << game.getPlayedRounds() << game.getSettings().getMaxRounds());
	}

	void Renderer::fpsCounter() const
	{
		std::string fpsCount = Format("FPS - {0}") << Int32(video.getFps());
		Int32 width = 8 * Int32(fpsCount.size()) + 2;

		Int32 x = Int32(video.getScreen().getClientWidth()) - width;
		Int32 y = Int32(video.getScreen().getClientHeight()) - 20;

		glBegin(GL_QUADS);
			glColor3f(0.0f, 0.0f, 0.0f);
			glVertex2i(x - 1, y + 17);
			glVertex2i(x + width, y + 17);
			glVertex2i(x + width, y - 1);
			glVertex2i(x - 1, y - 1);
		glEnd();

		font.print(x, y, Color::WHITE, fpsCount);
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

	Float32 Renderer::playerIndicator(const Player& player, const Indicator& indicator, const Color& color, Float32 value, Float32 xOfs, Float32 yOfs) const
	{
		glDepthMask(GL_FALSE);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_QUADS);

		Float32 width = value * 0.98f;
		Float32 X = xOfs - 0.5f;
		Float32 Y = yOfs;

		Uint8 alpha = Uint8(255 * indicator.getAlpha());

		glColor4ub(0, 0, 0, alpha);
		glVertex3f(X, Y, 0.5f);
		glVertex3f(X + 1.0f, Y, 0.5f);
		glVertex3f(X + 1.0f, Y - 0.1f, 0.5f);
		glVertex3f(X, Y - 0.1f, 0.5f);

		glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), alpha);
		glVertex3f(X + 0.01f, Y - 0.01f, 0.5f);
		glVertex3f(X + 0.01f + width, Y - 0.01f, 0.5f);
		glVertex3f(X + 0.01f + width, Y - 0.08f, 0.5f);
		glVertex3f(X + 0.01f, Y - 0.08f, 0.5f);

		glEnd();
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glDepthMask(GL_TRUE);
		glColor4ub(255, 255, 255, 255);

		return 0.1f;
	}

	void Renderer::playerName(const Player& player, const Indicator& indicator, Float32 xOfs, Float32 yOfs) const
	{
		const std::string& name = player.getPerson().getName();

		Float32 width = 0.15f * name.size();
		Float32 X = xOfs - width / 2;
		Float32 Y = yOfs;

		Uint8 alpha = Uint8(255 * indicator.getAlpha());

		glDepthMask(GL_FALSE);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_QUADS);

		Color color(0, 0, 255, alpha);
		glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());

		glVertex3f(X, Y, 0.5f);
		glVertex3f(X, Y + 0.3f, 0.5f);
		glVertex3f(X + width, Y + 0.3f, 0.5f);
		glVertex3f(X + width, Y, 0.5f);

		glEnd();
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glColor4ub(255, 255, 255, 255);

		Color fontColor(255, 255, 0, alpha);
		font.print(X, Y, 0.5f, fontColor, name, 0.3f);

		glDepthMask(GL_TRUE);
	}

	void Renderer::bulletIndicator(const Player& player, const Indicator& indicator, Float32 xOfs, Float32 yOfs) const
	{
		std::string bulletCount = Format("{0}") << player.getAmmo();

		Float32 width = 0.15f * bulletCount.size();
		Float32 X = xOfs - width / 2;
		Float32 Y = yOfs;

		Uint8 alpha = Uint8(255 * indicator.getAlpha());

		glDepthMask(GL_FALSE);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_QUADS);

		Color color(255, 255, 0, alpha);
		glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());

		glVertex3f(X, Y, 0.5f);
		glVertex3f(X, Y + 0.3f, 0.5f);
		glVertex3f(X + width, Y + 0.3f, 0.5f);
		glVertex3f(X + width, Y, 0.5f);

		glEnd();
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glColor4ub(255, 255, 255, 255);

		Color fontColor(0, 0, 255, alpha);
		font.print(X, Y, 0.5f, fontColor, bulletCount, 0.3f);

		glDepthMask(GL_TRUE);
	}

	void Renderer::bonusIndicator(const Player& player, const Indicator& indicator, Float32 xOfs, Float32 yOfs) const
	{
		Uint8 alpha = Uint8(255 * indicator.getAlpha());
		glColor4ub(255, 255, 255, alpha);

		Float32 size = 0.3f;
		Float32 X = xOfs - size / 2;
		Float32 Y = yOfs;

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_ALPHA_TEST);
		glDepthMask(GL_FALSE);
		glAlphaFunc(GL_GEQUAL, 1);
		glBindTexture(GL_TEXTURE_2D, player.getBonus().getTexture().getId());

		glBegin(GL_QUADS);
			glTexCoord2f(0.3f, 0.3f);
			glVertex3f(X, Y + size, 0.5f);
			glTexCoord2f(0.7f, 0.3f);
			glVertex3f(X + size, Y + size, 0.5f);
			glTexCoord2f(0.7f, 0.7f);
			glVertex3f(X + size, Y, 0.5f);
			glTexCoord2f(0.3f, 0.7f);
			glVertex3f(X, Y, 0.5f);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_ALPHA_TEST);
		glDepthMask(GL_TRUE);

		glColor4ub(255, 255, 255, 255);
	}

	void Renderer::playerStatus(const Player& player) const
	{
		const auto& indicators = player.getIndicators();

		if (player.isAlive())
		{
			Rectangle rect = player.getCollisionRect();
			Float32 xOfs = rect.getCentre().x;
			Float32 yOfs= rect.right.y + 0.15f;

			const auto& reload = indicators.getReload();
			if (reload.isVisible())
			{
				yOfs += playerIndicator(player, reload, Color::GREEN, 1.0f - player.getReloadTime() / player.getReloadInterval(), xOfs, yOfs);
			}

			const auto& air = indicators.getAir();
			if (air.isVisible())
			{
				yOfs += playerIndicator(player, air, Color::BLUE, player.getAir() / D6_MAX_AIR, xOfs, yOfs);
			}

			const auto& bonus = indicators.getBonus();
			if (bonus.isVisible())
			{
				yOfs += playerIndicator(player, bonus, Color::MAGENTA, player.getBonusRemainingTime() / player.getBonusDuration(), xOfs, yOfs);
			}

			const auto& health = indicators.getHealth();
			if (health.isVisible())
			{
				yOfs += playerIndicator(player, health, Color::RED, player.getLife() / D6_MAX_LIFE, xOfs, yOfs);
			}

			const auto& name = indicators.getName();
			const auto& bullets = indicators.getBullets();

			Float32 space = 0.08f;
			Float32 nameWidth = name.isVisible() ? space + player.getPerson().getName().size() * 0.15f : 0;
			std::string bulletString = Format("{0}") << player.getAmmo();
			Float32 bulletWidth = bullets.isVisible() ? space + bulletString.size() * 0.15f : 0;
			Float32 bonusWidth = bonus.isVisible() ? space + 0.3f : 0;
			Float32 totalWidth = nameWidth + bulletWidth + bonusWidth;
			Float32 halfWidth = totalWidth / 2;
			Float32 xStart = xOfs - halfWidth;

			if (name.isVisible())
			{
				Float32 nameX = xStart + nameWidth / 2;
				playerName(player, name, nameX, yOfs);
			}

			if (bullets.isVisible())
			{
				Float32 bulletX = xStart + nameWidth + bulletWidth / 2;
				bulletIndicator(player, bullets, bulletX, yOfs);
			}

			if (bonus.isVisible())
			{
				Float32 bonusX = xStart+ nameWidth + bulletWidth + bonusWidth / 2;
				bonusIndicator(player, bonus, bonusX, yOfs);
			}

			if (name.isVisible() || bullets.isVisible() || bonus.isVisible())
			{
				yOfs += 0.4f;
			}

			roundKills(player, xOfs, yOfs);
		}
	}

	void Renderer::roundKills(const Player& player, Float32 xOfs, Float32 yOfs) const
	{
		glColor3ub(0, 0, 255);
		glDisable(GL_TEXTURE_2D);
		glPointSize(5.0f);

		Float32 width = (2 * player.getRoundKills() - 1) * 0.1f;
		Float32 X = xOfs + 0.05f - width / 2;
		Float32 Y = yOfs + 0.1f;

		glBegin(GL_POINTS);
			for (Int32 i = 0; i < player.getRoundKills(); i++, X += 0.2f)
			{
				glVertex3f(X, Y, 0.5f);
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
		TheRenderer->setViewport(view.getX() - 2, view.getY() - 2, view.getWidth() + 4, view.getHeight() + 4);

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

	void Renderer::shotCollisionBox(const ShotList& shotList) const
	{
		glColor3f(1.0f, 0.0f, 0.0f);

		shotList.forEach([](const Shot& shot) -> bool {
			const auto& rect = shot.getCollisionRect();
			glBegin(GL_LINE_LOOP);
				glVertex3f(rect.left.x, rect.left.y, 0.6f);
				glVertex3f(rect.left.x, rect.right.y, 0.6f);
				glVertex3f(rect.right.x, rect.right.y, 0.6f);
				glVertex3f(rect.right.x, rect.left.y, 0.6f);
			glEnd();
			return true;
		});

		glColor3f(1.0f, 1.0f, 1.0f);
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
		world.getElevatorList().render();
		world.getSpriteList().render();
		world.getBonusList().render();
		invulRings(game.getPlayers());
		water(world.getLevelRenderData().getWater());
		youAreHere();

		for (const Player& hpPlayer : game.getPlayers())
		{
			playerStatus(hpPlayer);
		}
		//shotCollisionBox(world.getShotList());

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
		background(game.getResources().getBcgTextures().at(game.getRound().getWorld().getBackground()));
		video.setMode(Video::Mode::Perspective);
		view(player);		
	}

	void Renderer::splitScreen() const
	{
		for (const Player& player : game.getPlayers())
		{
			video.setMode(Video::Mode::Orthogonal);
			splitBox(player.getView());

			if (!player.isAlive())
			{
				glColor3f(1.0f, 0.5f, 0.5f);
			}

			setView(player.getView());
			background(game.getResources().getBcgTextures().at(game.getRound().getWorld().getBackground()));

			video.setMode(Video::Mode::Perspective);
			view(player);

			glColor3f(1, 1, 1);
		}
	}

	void Renderer::render() const
	{
		const GameSettings& settings = game.getSettings();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
