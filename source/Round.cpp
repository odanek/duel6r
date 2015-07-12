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

#include "Round.h"
#include "Game.h"
#include "GameException.h"
#include "Util.h"

namespace Duel6
{
	Round::Round(Game& game, Int32 roundNumber, std::vector<Player>& players, const std::string& levelPath, bool mirror, Size background)
		: game(game), roundNumber(roundNumber), world(game, levelPath, mirror, background),
		  deathMode(false), waterFillWait(0), showYouAreHere(D6_YOU_ARE_HERE_DURATION), gameOverWait(0),
		  winner(false)
	{
        game.getMode().initialize(world, game);
		preparePlayers();
	}

	void Round::splitScreenView(Player& player, Int32 x, Int32 y)
	{
		const Video& video = game.getAppService().getVideo();
		PlayerView view(x, y, video.getScreen().getClientWidth() / 2 - 4, video.getScreen().getClientHeight() / 2 - 4);
		player.setView(view);
		player.setInfoBarPosition(x + view.getWidth() / 2 - 76, y + 35);
	}

	void Round::setPlayerViews()
	{
		const Video& video = game.getAppService().getVideo();
		std::vector<Player>& players = world.getPlayers();

		for (Player& player : players)
		{
			player.prepareCam(video, game.getSettings().getScreenMode(), game.getSettings().getScreenZoom(), world.getLevel().getWidth(), world.getLevel().getHeight());
		}

		if (game.getSettings().getScreenMode() == ScreenMode::FullScreen)
		{
			Size index = 0;
			Size partition = video.getScreen().getClientWidth() / players.size();

			for (Player& player : players)
			{
				player.setView(PlayerView(0, 40, video.getScreen().getClientWidth(), video.getScreen().getClientHeight() - 40));
				player.setInfoBarPosition(partition * index + partition / 2 - 80, 35);
				index++;
			}

			return;
		}
		else
		{
			if (players.size() == 2)
			{
				splitScreenView(players[0], video.getScreen().getClientWidth() / 4 + 2, 2);
				splitScreenView(players[1], video.getScreen().getClientWidth() / 4 + 2, video.getScreen().getClientHeight() / 2 + 2);
			}

			if (players.size() == 3)
			{
				splitScreenView(players[0], 2, 2);
				splitScreenView(players[1], video.getScreen().getClientWidth() / 2 + 2, 2);
				splitScreenView(players[2], video.getScreen().getClientWidth() / 4 + 2, video.getScreen().getClientHeight() / 2 + 2);
			}

			if (players.size() == 4)
			{
				splitScreenView(players[0], 2, 2);
				splitScreenView(players[1], video.getScreen().getClientWidth() / 2 + 2, 2);
				splitScreenView(players[2], 2, video.getScreen().getClientHeight() / 2 + 2);
				splitScreenView(players[3], video.getScreen().getClientWidth() / 2 + 2, video.getScreen().getClientHeight() / 2 + 2);
			}
		}
	}

	bool Round::isPossibleStartingPosition(const Level& level, Int32 x, Int32 y)
	{
		if (level.isWall(x, y, true) || level.isWater(x, y))
		{
			return false;
		}

		while (y-- > 0)
		{
			if (level.isWater(x, y))
			{
				return false;
			}
			if (level.isWall(x, y, true))
			{
				return true;
			}
		}

		return false;
	}

	void Round::findStartingPositions(std::queue<std::pair<Int32, Int32>>& startingPositions)
	{
		const Level& level = world.getLevel();
		std::vector<std::pair<Int32, Int32>> possibleStartingPositions;

		for (Int32 y = 1; y < level.getHeight(); y++)
		{
			for (Int32 x = 0; x < level.getWidth(); x++)
			{
				if (isPossibleStartingPosition(level, x, y))
				{
					possibleStartingPositions.push_back(std::pair<Int32, Int32>(x, y));
				}
			}
		}

		if (possibleStartingPositions.empty())
		{
			D6_THROW(GameException, "No acceptable starting positions found in this level");
		}

		for (Size i = 0; i < world.getPlayers().size(); ++i)
		{
			Int32 arbitraryPosition = rand() % possibleStartingPositions.size();
			startingPositions.push(possibleStartingPositions[arbitraryPosition]);
		}
	}

	void Round::preparePlayers()
	{
		game.getAppService().getConsole().printLine(D6_L("...Preparing players"));
		std::queue<std::pair<Int32, Int32>> startingPositions;
		findStartingPositions(startingPositions);

		std::vector<Player>& players = world.getPlayers();
	    for (Uint32 i=0; i < players.size(); i++)
		{
			Player& player = players.at(i);

			auto& ammoRange = game.getSettings().getAmmoRange();
			Int32 ammo = ammoRange.first + rand() % (ammoRange.second - ammoRange.first + 1);
			std::pair<Int32, Int32>& position = startingPositions.front();
			player.startGame(world, position.first, position.second, ammo);
			startingPositions.pop();
			game.getMode().preparePlayer(player, i, players);
		}

		setPlayerViews();
	}

	void Round::checkWinner()
	{
        std::vector<Player>& allPlayers = world.getPlayers();

        alivePlayers.clear();

        // todo: rewrite to copy_if if it is possible to do it that way without billion lines of compile errors:-)
        for (Player& player : allPlayers)
        {
            if (!player.isDead())
            {
                alivePlayers.push_back(&player);
            }
        }
        if (alivePlayers.size() == 2 && allPlayers.size() > 2)
        {
            deathMode = true;
        }

		if (game.getMode().checkRoundOver(world, alivePlayers))
		{
			winner = true;
			gameOverWait = D6_GAME_OVER_WAIT;

			game.getResources().getGameOverSound().play();
		}
	}

	void Round::update(Float32 elapsedTime)
	{
		// Check if there's a winner
		if (!winner)
		{
			checkWinner();
		}

		if (hasWinner())
		{
			gameOverWait = std::max(gameOverWait - elapsedTime, 0.0f);
			if (gameOverWait < (D6_GAME_OVER_WAIT - D6_ROUND_OVER_WAIT))
			{
				return;
			}
		}

		for (Player& player : world.getPlayers())
		{
			player.update(world, game.getSettings().getScreenMode(), elapsedTime);
		}

		world.update(elapsedTime);

		if (deathMode)
		{
			waterFillWait += elapsedTime;
			if(waterFillWait > D6_RAISE_WATER_WAIT)
			{
				waterFillWait = 0;
				world.raiseWater();
			}
		}

		showYouAreHere = std::max(showYouAreHere - 3 * elapsedTime, 0.0f);
	}

	void Round::keyEvent(SDL_Keycode keyCode, Uint16 keyModifiers)
	{
		// Switch between fullscreen and split screen mode
		if (keyCode == SDLK_F2 && world.getPlayers().size() < 5)
		{
			switchScreenMode();
		}

		// Turn on/off player statistics
		if (keyCode == SDLK_F4)
		{
			game.getSettings().setShowRanking(!game.getSettings().isShowRanking());
		}

		// Save screenshot
		if (keyCode == SDLK_F10)
		{
			std::string name = Util::saveScreenTga(game.getAppService().getVideo());
			game.getAppService().getConsole().printLine(Format(D6_L("Screenshot saved to {0}")) << name);
		}
	}

	void Round::switchScreenMode()
	{
		GameSettings& settings = game.getSettings();
		settings.setScreenMode((settings.getScreenMode() == ScreenMode::FullScreen) ? ScreenMode::SplitScreen : ScreenMode::FullScreen);
		setPlayerViews();
		game.getRenderer().initScreen();
	}

	bool Round::isOver() const
	{
		return hasWinner() && gameOverWait <= 0;
	}

	bool Round::isLast() const
	{
		return game.getSettings().isRoundLimit() && roundNumber + 1 == game.getSettings().getMaxRounds();
	}
}
