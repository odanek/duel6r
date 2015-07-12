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

#ifndef DUEL6_GAME_H
#define DUEL6_GAME_H

#include <utility>
#include <vector>
#include <queue>
#include <string>
#include <algorithm> 
#include "Type.h"
#include "ScreenMode.h"
#include "Context.h"
#include "World.h"
#include "Player.h"
#include "Render.h"
#include "Water.h"
#include "AppService.h"
#include "GameSettings.h"
#include "GameResources.h"
#include "Round.h"
#include "GameMode.h"

namespace Duel6
{
	class Game
		: public Context
	{
	public:
		class PlayerDefinition
		{
		private:
			Person& person;
			const PlayerSkinColors& colors;
			const PlayerSounds& sounds;
			const PlayerControls& controls;

		public:
			PlayerDefinition(Person& person, const PlayerSkinColors& colors, const PlayerSounds& sounds, const PlayerControls& controls)
				: person(person), colors(colors), sounds(sounds), controls(controls)
			{}

			Person& getPerson() const
			{
				return person;
			}

			const PlayerSkinColors& getColors() const
			{
				return colors;
			}

			const PlayerSounds& getSounds() const
			{
				return sounds;
			}

			const PlayerControls& getControls() const
			{
				return controls;
			}
		};

	private:
		AppService& appService;
		GameResources resources;
		GameSettings& settings;
		GameMode* gameMode;
		std::unique_ptr<Round> round;
		Renderer renderer;

		std::vector<std::string> levels;
		std::vector<Size> backgrounds;
		Int32 playedRounds;

		std::vector<Player> players;
		std::vector<std::unique_ptr<PlayerSkin>> skins;

	public:
		Game(AppService& appService, GameSettings& settings);

		void start(const std::vector<PlayerDefinition>& playerDefinitions, const std::vector<std::string>& levels, const std::vector<Size>& backgrounds, ScreenMode screenMode, Int32 screenZoom, GameMode& gameMode);
		void keyEvent(SDL_Keycode keyCode, Uint16 keyModifiers) override;
		void textInputEvent(const char* text) override;
		void update(Float32 elapsedTime) override;
		void render() const override;

		AppService& getAppService() const
		{
			return appService;
		}

		std::vector<Player>& getPlayers()
		{
			return players;
		}

		const std::vector<Player>& getPlayers() const
		{
			return players;
		}

		GameResources& getResources()
		{
			return resources;
		}

		const GameResources& getResources() const
		{
			return resources;
		}

		GameSettings& getSettings()
		{
			return settings;
		}

		const GameSettings& getSettings() const
		{
			return settings;
		}

		Round& getRound()
		{
			return *round;
		}

		const Round& getRound() const
		{
			return *round;
		}

		Int32 getPlayedRounds() const
		{
			return playedRounds;
		}

        bool isOver() const
		{
			return getRound().isLast() && getRound().isOver();
		}

		Renderer& getRenderer()
		{
			return renderer;
		}

		GameMode& getMode()
		{
			return *gameMode;
		}

		const Renderer& getRenderer() const
		{
			return renderer;
		}

	private:
		void beforeStart(Context* prevContext) override;
		void beforeClose(Context* nextContext) override;
		void nextRound();
	};	
}

#endif