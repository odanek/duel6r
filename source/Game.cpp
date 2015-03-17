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

#include "Sound.h"
#include "BonusList.h"
#include "Weapon.h"
#include "Util.h"
#include "ElevatorList.h"
#include "World.h"
#include "InfoMessageQueue.h"
#include "Explosion.h"
#include "Render.h"
#include "Fire.h"
#include "Video.h"
#include "Game.h"
#include "GameException.h"

namespace Duel6
{
	Game::Game(AppService& appService)
		: appService(appService), video(appService.getVideo()), messageQueue(D6_INFO_DURATION),
		world(D6_ANM_SPEED, D6_WAVE_HEIGHT, appService.getConsole()),
		renderer(*this, appService.getFont(), video, spriteList, messageQueue),
		ammoRange(15, 15), playedRounds(0), maxRounds(0),
		gameService(appService, spriteList, messageQueue, world, players)
	{}

	void Game::initialize()
	{
		appService.getConsole().printLine(D6_L("\n===Game initialization==="));
		Sound& sound = appService.getSound();
		waterSet = Water::createWaterSet(sound, appService.getTextureManager());
		roundStartSound = sound.loadSample("sound/game/round-start.wav");
		gameOverSound = sound.loadSample("sound/game/game-over.wav");		
		renderer.initialize(appService.getTextureManager());
		world.initialize(D6_FILE_BLOCK_META);
	}

	void Game::splitScreenView(Player& player, Int32 x, Int32 y)
	{
		PlayerView view(x, y, video.getScreen().getClientWidth() / 2 - 4, video.getScreen().getClientHeight() / 2 - 4);
		player.setView(view);
		player.setInfoBarPosition(x + view.getWidth() / 2 - 76, y + 35);
	}

	void Game::setPlayerViews()
	{
		for (Player& player : players)
		{
			player.prepareCam(video, getScreenMode(), getScreenZoom(), world.getSizeX(), world.getSizeY());
		}

		if (screenMode == ScreenMode::FullScreen)
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

	bool Game::isPossibleStartingPosition(Int32 x, Int32 y)
	{
		if (world.isWall(x, y, true) || world.isWater(x, y))
		{
			return false;
		}

		while (y-- > 0)
		{
			if (world.isWater(x, y))
			{
				return false;
			}
			if (world.isWall(x, y, true))
			{
				return true;
			}
		}

		return false;
	}

	void Game::findStartingPositions(std::queue<std::pair<Int32, Int32>>& startingPositions)
	{
		std::vector<std::pair<Int32, Int32>> possibleStartingPositions;

		for (Int32 y = 1; y < world.getSizeY(); y++)
		{
			for (Int32 x = 0; x < world.getSizeX(); x++)
			{
				if (isPossibleStartingPosition(x, y))
				{
					possibleStartingPositions.push_back(std::pair<Int32, Int32>(x, y));
				}
			}
		}

		if (possibleStartingPositions.empty())
		{
			D6_THROW(GameException, "No acceptable starting positions found in this level");
		}

		for (Size i = 0; i < players.size(); ++i)
		{
			Int32 arbitraryPosition = rand() % possibleStartingPositions.size();
			startingPositions.push(possibleStartingPositions[arbitraryPosition]);
		}
	}

	void Game::preparePlayers()
	{
		std::queue<std::pair<Int32, Int32>> startingPositions;
		findStartingPositions(startingPositions);

		for (Player& player : players)
		{
			Int32 ammo = ammoRange.first + rand() % (ammoRange.second - ammoRange.first + 1);
			std::pair<Int32, Int32>& position = startingPositions.front();
			player.startGame(position.first, position.second, ammo);
			startingPositions.pop();
                        
			player.setBonus(D6_BONUS_INVUL, 2);
		}

		setPlayerViews();
	}

	void Game::checkWinner()
	{
		int numAlive = 0;
		Player* lastAlive = nullptr;

		for (Player& player : players)
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
				messageQueue.add(*lastAlive, D6_L("You have won - press ESC to quit or F1 for another round"));
				lastAlive->getPerson().addWins(1);
			}
			else
			{
				for (const Player& player : players)
				{
					messageQueue.add(player, D6_L("End of round - no winner"));
				}
			}
			gameOverSound.play();
		}
		else if (numAlive == 2 && players.size() > 2)
		{
			deathMode = true;
		}
	}

	void Game::beforeStart(Context* prevContext)
	{
		SDL_ShowCursor(SDL_DISABLE);
	}

	void Game::beforeClose(Context* nextContext)
	{
	}

	void Game::update(Float32 elapsedTime)
	{
        // Check if there's a winner
        if (winner == -1)
        {
            checkWinner();
        }

        if (hasWinner())
        {
            if(gameOverWait > 0)
            {
                gameOverWait -= elapsedTime;
            }
            else if (!isOver())
            {
				nextRound();
            }

            if(gameOverWait < (D6_GAME_OVER_WAIT - D6_ROUND_OVER_WAIT))
            {
                return;
            }
        }

		for (Player& player : players)
		{
			player.update(world, getScreenMode(), elapsedTime);
		}

		world.update(elapsedTime);
		spriteList.update(elapsedTime * D6_SPRITE_SPEED_COEF);
		WPN_MoveShots(gameService, elapsedTime);
		EXPL_MoveAll(elapsedTime);
		ELEV_MoveAll(elapsedTime);
		messageQueue.update(elapsedTime);

		// Add new bonuses
		if (rand() % int(3.0f / elapsedTime) == 0)
		{
			BONUS_AddNew(world);
		}

		if(deathMode)
		{
			waterFillWait += elapsedTime;
			if(waterFillWait > D6_RAISE_WATER_WAIT)
			{
				waterFillWait = 0;
				world.raiseWater();
			}
		}

		updateNotifications(elapsedTime);
	}
        
	void Game::updateNotifications(Float32 elapsedTime)
	{
		if (showYouAreHere > 0)
		{
			showYouAreHere -= elapsedTime * 3;
		}
		else
		{
			showYouAreHere = 0;
		}                
	}
        
	void Game::render() const
	{
		renderer.render();
	}

	void Game::keyEvent(SDL_Keycode keyCode, Uint16 keyModifiers)
	{
		bool roundLimit = (maxRounds > 0) && (playedRounds >= maxRounds);

		if (keyCode == SDLK_ESCAPE && (roundLimit || (keyModifiers & KMOD_SHIFT) != 0))
		{
			close();
		}

		// Restart game
		if (keyCode == SDLK_F1 && !roundLimit && (hasWinner() || (keyModifiers & KMOD_SHIFT) != 0))
		{
			nextRound();
			return;
		}

		if (!isOver() && hasWinner() && ((D6_GAME_OVER_WAIT - gameOverWait) > 3.0f))
		{
			gameOverWait = 0.0f;
		}

		// Switch between fullscreen and split screen mode
		if (keyCode == SDLK_F2 && players.size() < 5)
		{
			switchScreenMode();
		}

		// Turn on/off player statistics
		if (keyCode == SDLK_F4)
		{
			renderer.setShowRanking(!renderer.getShowRanking());
		}

		// Save screenshot
		if (keyCode == SDLK_F10)
		{
			std::string name = Util::saveScreenTga(video);
			appService.getConsole().printLine(Format(D6_L("Screenshot saved to {0}")) << name);
		}
	}

	void Game::textInputEvent(const char* text)
	{
	}

	void Game::switchScreenMode()
	{
		screenMode = (screenMode == ScreenMode::FullScreen) ? ScreenMode::SplitScreen : ScreenMode::FullScreen;
		setPlayerViews();
		renderer.initScreen();
	}

	void Game::start(const std::vector<PlayerDefinition>& playerDefinitions, const std::vector<std::string>& levels, const std::vector<Size>& backgrounds, ScreenMode screenMode, Int32 screenZoom)
	{
		appService.getConsole().printLine("\n=== Starting new game ===");
		appService.getConsole().printLine(Format("...Rounds: {0}") << maxRounds);
		TextureManager& textureManager = appService.getTextureManager();
		players.clear();
		skins.clear();
		
		for (const PlayerDefinition& playerDef : playerDefinitions)
		{
			appService.getConsole().printLine(Format("...Generating player for person: {0}") << playerDef.getPerson().getName());
			skins.push_back(std::make_unique<PlayerSkin>(D6_TEXTURE_MAN_PATH, playerDef.getColors(), textureManager));
			players.push_back(Player(playerDef.getPerson(), *skins.back(), playerDef.getSounds(), playerDef.getControls(), spriteList, messageQueue, waterSet));
		}

		this->levels = levels;
		this->backgrounds = backgrounds;
		this->screenMode = screenMode;
		this->screenZoom = screenZoom;

		playedRounds = 0;
		nextRound();
	}

	void Game::nextRound()
	{
        lastLevel = rand() % levels.size();
		deathMode = false;
		waterFillWait = 0;

		const std::string levelPath = levels[lastLevel];
		Console& console = appService.getConsole();
		console.printLine(Format(D6_L("\n===Loading level {0}===")) << levelPath);
		bool mirror = rand() % 2 == 0;
		world.loadLevel(levelPath, backgrounds[rand() % backgrounds.size()], mirror);
		world.prepareFaces();
		console.printLine(Format(D6_L("...Width   : {0}")) << world.getSizeX());
		console.printLine(Format(D6_L("...Height  : {0}")) << world.getSizeY());
		console.printLine(Format(D6_L("...Walls   : {0}")) << world.getWalls().getFaces().size());
		console.printLine(Format(D6_L("...Sprites : {0}")) << world.getSprites().getFaces().size());
		console.printLine(Format(D6_L("...Water   : {0}")) << world.getWater().getFaces().size());

		spriteList.clear();

		console.printLine(D6_L("...Preparing players"));
		preparePlayers();

		console.printLine(D6_L("...Level initialization"));
		WPN_LevelInit();
		EXPL_Clear();
		BONUS_Clear();
		FIRE_Find(world.getSprites());
		renderer.initScreen();
		winner = -1;
		playedRounds++;
		showYouAreHere = D6_YOU_ARE_HERE_DURATION;

		roundStartSound.play();
	}
		
	std::vector<const Player*> Game::getLadder() const
	{
		std::vector<const Player*> ladder;

		for (const Player& player : players)
		{
			ladder.push_back(&player);
		}
		
		std::sort(ladder.begin(), ladder.end(), [](const Player* pl1, const Player* pl2) {
			return pl2->getPerson().getTotalPoints() < pl1->getPerson().getTotalPoints();
		});
		return ladder;
	}

	
	Color Game::getGameOverOverlay() const
	{
		Uint8 overlay = Uint8(255.0f * gameOverWait / D6_GAME_OVER_WAIT);
		if (isOver())
		{
			overlay = std::min(overlay + 51, 255);
			return Color(overlay);
		}

		return Color(255, overlay, overlay);
	}

	World& Game::getWorld()
	{
		return world;
	}

	const World& Game::getWorld() const
	{
		return world;
	}	
}