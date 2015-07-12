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
#include "Render.h"
#include "Game.h"

namespace Duel6
{
	Game::Game(AppService& appService, GameSettings& settings)
		: appService(appService), resources(appService), settings(settings), renderer(appService, *this), playedRounds(0)
	{}

	void Game::beforeStart(Context* prevContext)
	{
		SDL_ShowCursor(SDL_DISABLE);
	}

	void Game::beforeClose(Context* nextContext)
	{
	}

	void Game::render() const
	{
		renderer.render();
	}

	void Game::update(Float32 elapsedTime)
	{
		if (getRound().isOver() && !getRound().isLast())
		{
			nextRound();
			return;
		}

		getRound().update(elapsedTime);
	}

	void Game::keyEvent(SDL_Keycode keyCode, Uint16 keyModifiers)
	{
		if (keyCode == SDLK_ESCAPE && (isOver() || (keyModifiers & KMOD_SHIFT) != 0))
		{
			close();
			return;
		}

		if (!getRound().isLast())
		{
			if (keyCode == SDLK_F1 && (getRound().hasWinner() || (keyModifiers & KMOD_SHIFT) != 0))
			{
				nextRound();
				return;
			}

			if (getRound().hasWinner() && ((D6_GAME_OVER_WAIT - getRound().getRemainingGameOverWait()) > 3.0f))
			{
				nextRound();
				return;
			}
		}

		getRound().keyEvent(keyCode, keyModifiers);
	}

	void Game::textInputEvent(const char* text)
	{
	}

	void Game::start(const std::vector<PlayerDefinition>& playerDefinitions, const std::vector<std::string>& levels, const std::vector<Size>& backgrounds, ScreenMode screenMode, Int32 screenZoom, GameMode& gameMode)
	{
		appService.getConsole().printLine("\n=== Starting new game ===");
		appService.getConsole().printLine(Format("...Rounds: {0}") << settings.getMaxRounds());
		TextureManager& textureManager = appService.getTextureManager();
		players.clear();
		skins.clear();
		
		for (const PlayerDefinition& playerDef : playerDefinitions)
		{
			appService.getConsole().printLine(Format("...Generating player for person: {0}") << playerDef.getPerson().getName());
			skins.push_back(std::make_unique<PlayerSkin>(D6_TEXTURE_MAN_PATH, playerDef.getColors(), textureManager));
			players.push_back(Player(playerDef.getPerson(), *skins.back(), playerDef.getSounds(), playerDef.getControls()));
		}

		this->levels = levels;
		this->backgrounds = backgrounds;
		this->gameMode = &gameMode;
		settings.setScreenMode(screenMode);
		settings.setScreenZoom(screenZoom);

		playedRounds = 0;
		nextRound();
	}

	void Game::nextRound()
	{
        Int32 level = rand() % Int32(levels.size());
		const std::string levelPath = levels[level];
		bool mirror = rand() % 2 == 0;
		Size background = backgrounds[rand() % backgrounds.size()];

		Console& console = appService.getConsole();
		console.printLine(Format(D6_L("\n===Loading level {0}===")) << levelPath);
		console.printLine(Format("...Parameters: mirror: {0}, background: {1}") << mirror << background);
		round = std::make_unique<Round>(*this, playedRounds, players, levelPath, mirror, background);

		playedRounds++;
		resources.getRoundStartSound().play();

		renderer.initScreen();
	}
}