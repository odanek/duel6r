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
#include "Sound.h"
#include "BonusList.h"
#include "Weapon.h"
#include "Util.h"
#include "ElevatorList.h"
#include "Menu.h"
#include "World.h"
#include "InfoMessageQueue.h"
#include "Explosion.h"
#include "Render.h"
#include "Fire.h"
#include "Globals.h"
#include "Game.h"

namespace Duel6
{
	void Game::splitScreenView(Player& player, Int32 x, Int32 y)
	{
		PlayerView view(x, y, g_vid.cl_width / 2 - 4, g_vid.cl_height / 2 - 4);
		player.setView(view);
		player.setInfoBarPosition(x + view.getWidth() / 2 - 76, y + 30);
	}

	void Game::setPlayerViews()
	{
		for (Player& player : getPlayers())
		{
			player.prepareCam(getScreenMode(), getScreenZoom(), d6World.getSizeX(), d6World.getSizeY());
		}

		if (screenMode == ScreenMode::FullScreen)
		{
			Int32 xShift = (g_vid.cl_width / 4) / 2 - 70;
			Size index = 0;

			for (Player& player : getPlayers())
			{
				player.setView(PlayerView(0, 0, g_vid.cl_width, g_vid.cl_height));

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
		else
		{
			if (getPlayers().size() == 2)
			{
				splitScreenView(getPlayers()[0], g_vid.cl_width / 4 + 2, 2);
				splitScreenView(getPlayers()[1], g_vid.cl_width / 4 + 2, g_vid.cl_height / 2 + 2);
			}

			if (getPlayers().size() == 3)
			{
				splitScreenView(getPlayers()[0], 2, 2);
				splitScreenView(getPlayers()[1], g_vid.cl_width / 2 + 2, 2);
				splitScreenView(getPlayers()[2], g_vid.cl_width / 4 + 2, g_vid.cl_height / 2 + 2);
			}

			if (getPlayers().size() == 4)
			{
				splitScreenView(getPlayers()[0], 2, 2);
				splitScreenView(getPlayers()[1], g_vid.cl_width / 2 + 2, 2);
				splitScreenView(getPlayers()[2], 2, g_vid.cl_height / 2 + 2);
				splitScreenView(getPlayers()[3], g_vid.cl_width / 2 + 2, g_vid.cl_height / 2 + 2);
			}
		}
	}

	bool Game::isPossibleStartingPosition(Int32 x, Int32 y)
	{
		if (d6World.isWall(x, y, true) || d6World.isWater(x, y))
		{
			return false;
		}

		while (y-- > 0)
		{
			if (d6World.isWater(x, y))
			{
				return false;
			}
			if (d6World.isWall(x, y, true))
			{
				return true;
			}
		}

		return false;
	}

	void Game::findStartingPositions()
	{
		std::vector<std::pair<Int32, Int32>> possibleStartingPositions;

		for (Int32 y = 1; y < d6World.getSizeY(); y++)
		{
			for (Int32 x = 0; x < d6World.getSizeX(); x++)
			{
				if (isPossibleStartingPosition(x, y))
				{
					possibleStartingPositions.push_back(std::pair<Int32, Int32>(x, y));
				}
			}
		}

		// TODO: If possibleStartingPositions.empty() -> error

		for (Player& player : getPlayers())
		{
			Int32 arbitraryPosition = rand() % possibleStartingPositions.size();
			const std::pair<Int32, Int32>& position = possibleStartingPositions[arbitraryPosition];
			player.startGame(position.first, position.second);
		}
	}

	void Game::preparePlayers()
	{
		findStartingPositions();
		setPlayerViews();
	}

	void Game::checkWinner()
	{
		int numAlive = 0;
		Player* lastAlive = nullptr;

		for (Player& player : getPlayers())
		{
			if (!player.isDead())
			{
				numAlive++;
				lastAlive = &player;
			}
		}

		if (numAlive < 2)
		{
			winner = 1;
			gameOverWait = D6_GAME_OVER_WAIT;

			if (numAlive == 1)
			{
				d6MessageQueue.add(*lastAlive, MY_L("APP00024|Jsi vitez - stiskni ESC pro konec nebo F1 pro novou hru"));
				lastAlive->getPerson().setWins(lastAlive->getPerson().getWins() + 1);
			}
			else
			{
				for (const Player& player : getPlayers())
				{
					d6MessageQueue.add(player, MY_L("APP00025|Konec hry - bez viteze"));
				}
			}
		}
	}

	void Game::startContext()
	{
		makeCurrent();
		SDL_ShowCursor(SDL_DISABLE);
	}

	void Game::update(Float32 elapsedTime)
	{
		for (Player& player : getPlayers())
		{
			player.update(d6World, getScreenMode(), elapsedTime);
		}

		d6World.update(elapsedTime);
		d6SpriteList.update(elapsedTime * D6_SPRITE_SPEED_COEF);
		WPN_MoveShots(*this, elapsedTime);
		EXPL_MoveAll(elapsedTime);
		ELEV_MoveAll(elapsedTime);
		d6MessageQueue.update(elapsedTime);

		// Add new bonuses
		if (rand() % int(3.0f / elapsedTime) == 0)
		{
			BONUS_AddNew(d6World);
		}

		// Check if there's a winner
		if (winner == -1)
		{
			checkWinner();
		}
		else if (winner == 1)
		{
			gameOverWait -= elapsedTime;
			if (gameOverWait <= 0)
			{
				gameOverWait = 0;
				winner = 2;
				Sound::playSample(D6_SND_GAME_OVER);
			}
		}
	}

	void Game::render() const
	{
		RENDER_DrawScene(*this);
	}

	void Game::keyEvent(SDL_Keycode keyCode, Uint16 keyModifiers)
	{
		if (keyCode == SDLK_ESCAPE)
		{
			d6Menu.savePersonData();
			d6Menu.startContext();
		}

		// Restart game
		bool roundLimit = (maxRounds > 0) && (playedRounds >= maxRounds);
		if (keyCode == SDLK_F1 && !roundLimit)
		{
			d6Menu.savePersonData();
			nextRound((keyModifiers & KMOD_SHIFT) != 0);
			return;
		}

		// Switch between fullscreen and split screen mode
		if (keyCode == SDLK_F2 && getPlayers().size() < 5)
		{
			switchScreenMode();
		}

		// Turn on/off player statistics
		if (keyCode == SDLK_F4)
		{
			d6ShowRanking = !d6ShowRanking;
		}

		// Save screenshot
		if (keyCode == SDLK_F10)
		{
			Util::saveScreenTga();
		}
	}

	void Game::textInputEvent(const char* text)
	{
	}

	void Game::switchScreenMode()
	{
		screenMode = (screenMode == ScreenMode::FullScreen) ? ScreenMode::SplitScreen : ScreenMode::FullScreen;
		setPlayerViews();
		RENDER_InitScreen();
	}

	void Game::start(const std::vector<PlayerDefinition>& playerDefinitions, const std::vector<std::string>& levels, const std::vector<Int32>& backgrounds, ScreenMode screenMode, Int32 screenZoom)
	{
		players.clear();
		PlayerSkin::freeAll();
		for (const PlayerDefinition& playerDef : playerDefinitions)
		{			
			players.push_back(Player(playerDef.getPerson(), PlayerSkin::create(D6_TEXTURE_MAN_PATH, playerDef.getColors()), playerDef.getControls()));
		}
		this->levels = levels;
		this->backgrounds = backgrounds;
		this->screenMode = screenMode;
		this->screenZoom = screenZoom;

		playedRounds = 0;
		nextRound(false);
	}

	void Game::nextRound(bool sameLevel)
	{
		if (!sameLevel)
		{
			lastLevel = rand() % levels.size();
		}

		const std::string levelPath = levels[lastLevel];
		g_app.con->printf(MY_L("APP00060|\n===Nahravam uroven %s===\n"), levelPath.c_str());
		std::vector<Bonus> bonuses;
		bool mirror = rand() % 2 == 0;
		d6World.loadLevel(levelPath, backgrounds[rand() % backgrounds.size()], mirror);
		d6World.findBonuses(bonuses);
		d6World.prepareFaces();
		g_app.con->printf(MY_L("APP00061|...Sirka   : %d\n"), d6World.getSizeX());
		g_app.con->printf(MY_L("APP00062|...Vyska   : %d\n"), d6World.getSizeY());
		g_app.con->printf(MY_L("APP00063|...Sten    : %d\n"), d6World.getWalls().getFaces().size());
		g_app.con->printf(MY_L("APP00064|...Spritu  : %d\n"), d6World.getSprites().getFaces().size());
		g_app.con->printf(MY_L("APP00065|...Voda    : %d\n"), d6World.getWater().getFaces().size());

		d6SpriteList.clear();

		g_app.con->printf(MY_L("APP00066|...Pripravuji hrace\n"));
		preparePlayers();

		g_app.con->printf(MY_L("APP00067|...Inicializace urovne\n"));
		WPN_LevelInit();
		EXPL_Init();
		BONUS_Init(bonuses);
		FIRE_Find(d6World.getSprites());
		RENDER_InitScreen();
		winner = -1;
		playedRounds++;

		Sound::playSample(D6_SND_LETS_ROCK);
	}

	Color Game::getGameOverOverlay() const
	{
		Uint8 overlay = Uint8(255.0f * gameOverWait / D6_GAME_OVER_WAIT);
		bool roundLimit = (maxRounds > 0) && (playedRounds >= maxRounds);

		if (roundLimit)
		{
			overlay = std::min(overlay + 51, 255);
			return Color(overlay);
		}

		return Color(255, overlay, overlay);
	}

	World& Game::getWorld()
	{
		return d6World;
	}

	const World& Game::getWorld() const
	{
		return d6World;
	}
}