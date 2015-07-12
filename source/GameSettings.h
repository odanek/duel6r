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

#ifndef DUEL6_GAMESETTINGS_H
#define DUEL6_GAMESETTINGS_H

#include <utility>
#include "Type.h"
#include "ScreenMode.h"

namespace Duel6
{
	class GameSettings
	{
	private:
		std::pair<Int32, Int32> ammoRange;
		Int32 maxRounds;
		ScreenMode screenMode;
		Int32 screenZoom;
		bool wireframe;
		bool showFps;
		bool showRanking;
		bool ghostMode;

	public:
		GameSettings()
			: ammoRange(15, 15), maxRounds(0), screenMode(ScreenMode::FullScreen),
			  screenZoom(13), wireframe(false), showFps(false), showRanking(true), ghostMode(true)
		{}

		ScreenMode getScreenMode() const
		{
			return screenMode;
		}

		GameSettings& setScreenMode(const ScreenMode& screenMode)
		{
			this->screenMode = screenMode;
			return *this;
		}

		Int32 getScreenZoom() const
		{
			return screenZoom;
		}

		GameSettings& setScreenZoom(Int32 screenZoom)
		{
			this->screenZoom = screenZoom;
			return *this;
		}

		const std::pair<Int32, Int32>& getAmmoRange() const
		{
			return ammoRange;
		}

		GameSettings& setAmmoRange(const std::pair<Int32, Int32>& range)
		{
			ammoRange = range;
			return *this;
		}

		Int32 getMaxRounds() const
		{
			return maxRounds;
		}

		bool isRoundLimit() const
		{
			return maxRounds > 0;
		}

		GameSettings& setMaxRounds(Int32 maxRounds)
		{
			this->maxRounds = maxRounds;
			return *this;
		}

		GameSettings& setGhostEnabled(bool enabled)
		{
			this->ghostMode = enabled;
			return *this;
		}

		bool getGhostEnabled()
		{
			return this->ghostMode;
		}

		bool isWireframe() const
		{
			return wireframe;
		}

		GameSettings& setWireframe(bool wireframe)
		{
			this->wireframe = wireframe;
			return *this;
		}

		bool isShowFps() const
		{
			return showFps;
		}

		GameSettings& setShowFps(bool showFps)
		{
			this->showFps = showFps;
			return *this;
		}

		bool isShowRanking() const
		{
			return showRanking;
		}

		GameSettings& setShowRanking(bool showRanking)
		{
			this->showRanking = showRanking;
			return *this;
		}
	};
}


#endif
