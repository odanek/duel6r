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

#ifndef DUEL6_SOUND_H
#define DUEL6_SOUND_H

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "Type.h"
#include "console/console.h"

namespace Duel6
{
	enum
	{
		D6_SND_DEAD,
		D6_SND_BNPICK,
		D6_SND_SHOT_BL,
		D6_SND_BOOM_BL,
		D6_SND_SHOT_LS,
		D6_SND_SHOT_PI,
		D6_SND_SHOT_PL,
		D6_SND_SHOT_SG,
		D6_SND_SHOT_BZ,
		D6_SND_BOOM_BZ,
		D6_SND_WATER,
		D6_SND_SHOT_TZ,
		D6_SND_SHOT_KM,
		D6_SND_HIT,
		D6_SND_SHOT_LK,
		D6_SND_SHOT_SM,
		D6_SND_SHOT_KD1,
		D6_SND_SHOT_KD2,
		D6_SND_SHOT_SPRAY,
		D6_SND_SHOT_SLING,
		D6_SND_SHOT_SPUNT,
		D6_SND_SHOT_SHIT,
		D6_SND_SHOT_SHIT_HIT,
		D6_SND_LETS_ROCK,
		D6_SND_GAME_OVER
	};

	class Sound
	{
	public:
		typedef Size Sample;
		typedef Size Track;

	private:
		Console& console;
		bool playing;
		Size channels;
		std::vector<Mix_Music*> modules;
		std::vector<Mix_Chunk*> samples;

	public:
		Sound(Size channels, Console& console);
		~Sound();

		Track loadModule(const std::string& fileName);
		Sample loadSample(const std::string& fileName);
		void stopMusic();
		void startMusic(Track track, bool loop);
		void playSample(Sample sample);
		void volume(Int32 volume);
	};
}

#endif
