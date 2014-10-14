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

namespace Duel6
{
	class Menu; // Forward, TODO: Remove

	class Game
		: public Context
	{
	public:
		class PlayerDefinition
		{
		private:
			Person& person;
			const PlayerSkinColors& colors;
			const PlayerControls& controls;

		public:
			PlayerDefinition(Person& person, const PlayerSkinColors& colors, const PlayerControls& controls)
				: person(person), colors(colors), controls(controls)
			{}

			Person& getPerson() const
			{
				return person;
			}

			const PlayerSkinColors& getColors() const
			{
				return colors;
			}

			const PlayerControls& getControls() const
			{
				return controls;
			}
		};

	private:
		Video& video;
		Menu* menu;
		Renderer renderer;
		std::vector<std::string> levels;
		std::vector<Size> backgrounds;
		Size lastLevel;
		ScreenMode screenMode;
		Int32 screenZoom;

		Int32 winner;
		std::pair<Int32, Int32> ammoRange;
		Int32 playedRounds;
		Int32 maxRounds;
		
		Float32 showYouAreHere;
		Float32 gameOverWait;

		World world;
		std::vector<Player> players;

	public:
		Game(Video& video, const Font& font);

		void setMenuReference(Menu* menu)
		{
			this->menu = menu;
		}

		void start(const std::vector<PlayerDefinition>& playerDefinitions, const std::vector<std::string>& levels, const std::vector<Size>& backgrounds, ScreenMode screenMode, Int32 screenZoom);
		void keyEvent(SDL_Keycode keyCode, Uint16 keyModifiers) override;
		void textInputEvent(const char* text) override;
		void update(Float32 elapsedTime) override;
        void updateNotifications(Float32 elapsedTime);

		void render() const override;

		World& getWorld();

		const World& getWorld() const;

		std::vector<Player>& getPlayers()
		{
			return players;
		}

		const std::vector<Player>& getPlayers() const
		{
			return players;
		}

		ScreenMode getScreenMode() const
		{
			return screenMode;
		}

		Int32 getScreenZoom() const
		{
			return screenZoom;
		}

		bool hasWinner() const
		{
			return winner > 0;
		}

		const std::pair<Int32, Int32>& getAmmoRange() const
		{
			return ammoRange;
		}

		void setAmmoRange(const std::pair<Int32, Int32>& range)
		{
			ammoRange = range;
		}

		Int32 getPlayedRounds() const
		{
			return playedRounds;
		}

		Int32 getMaxRounds() const
		{
			return maxRounds;
		}

		void setMaxRounds(Int32 maxRounds)
		{
			this->maxRounds = maxRounds;
		}

		Float32 getRemainingYouAreHere() const
		{
			return showYouAreHere;
		}

		bool isOver() const
		{
			return (maxRounds > 0) && (playedRounds >= maxRounds);
		}

		Renderer& getRenderer()
		{
			return renderer;
		}

		Color getGameOverOverlay() const;
		std::vector<const Player *> getLadder() const;	

	private:
		void beforeStart(Context* prevContext) override;
		void beforeClose(Context* nextContext) override
		{}

		void preparePlayers();
		void findStartingPositions(std::queue<std::pair<Int32, Int32>>& startingPositions);
		bool isPossibleStartingPosition(int x, int y);
		void checkWinner();
		void setPlayerViews();
		void splitScreenView(Player& player, Int32 x, Int32 y);
		void switchScreenMode();
		void nextRound(bool sameLevel);
	};	
}

#endif