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

#ifndef DUEL6_RENDERER_H
#define DUEL6_RENDERER_H

#include <vector>
#include "Type.h"
#include "Player.h"
#include "Font.h"
#include "Video.h"
#include "FaceList.h"
#include "SpriteList.h"
#include "TextureManager.h"
#include "InfoMessageQueue.h"

namespace Duel6
{
	class Game;

	class Renderer
	{
	private:
		const Game& game;
		const Font& font;
		const Video& video;
		const SpriteList& spriteList;
		const InfoMessageQueue& messageQueue;

		TextureManager::Texture blockTextures;
		TextureManager::Texture bcgTextures;

		bool wireframe;
		bool showFps;	
		bool showRanking;

	public:
		Renderer(const Game& game, const Font& font, const Video& video, 
			const SpriteList& spriteList, const InfoMessageQueue& messageQueue)
			: game(game), font(font), video(video), spriteList(spriteList), 
			messageQueue(messageQueue), wireframe(false), showFps(false), showRanking(true)
		{}

		void initialize(TextureManager& textureManager);
		void initScreen();
		void render() const;

		bool getWireframe() const
		{
			return wireframe;
		}

		Renderer& setWireframe(bool wireframe)
		{
			this->wireframe = wireframe;
			return *this;
		}

		bool getShowFps() const
		{
			return showFps;
		}

		Renderer& setShowFps(bool showFps)
		{
			this->showFps = showFps;
			return *this;
		}

		bool getShowRanking() const
		{
			return showRanking;
		}

		Renderer& setShowRanking(bool showRanking)
		{
			this->showRanking = showRanking;
			return *this;
		}

	private:
		void setView(const PlayerView& view) const;
		void setView(int x, int y, int width, int height) const;
		void view(const Player& player) const;
		void fullScreen() const;
		void splitScreen() const;
		void water(const FaceList& water) const;
		void sprites(const FaceList& sprites) const;
		void background(GLuint texture) const;
		void playerRankings() const;
		void gameOverSummary() const;
		void roundsPlayed() const;
		void playerStatuses() const;
		void playerStatus(const Player& player) const;
		void fpsCounter() const;
		void notifications() const;
		void roundKills() const;
		void invulRings(const std::vector<Player>& players) const;
		void invulRing(const Player& player) const;
		void splitBox(const PlayerView& view) const;
		void infoMessages() const;
	};
}

#endif
