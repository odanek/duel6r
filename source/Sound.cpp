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

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "core/co_core.h"
#include "Type.h"

#define SND_TEST(x) if (x == -1) { SOUND_Error (); return; }

namespace Duel6
{
	namespace Sound
	{
		namespace
		{
			struct sndSound_s
			{
				bool inited;
				bool playing;
				Size channels;
				std::vector<Mix_Music*> modules;
				std::vector<Mix_Chunk*> samples;
			};

			// Inicializace struktury
			sndSound_s snd = { false };
		}

		/*
		==================================================
		Chyba zvukoveho systemu
		==================================================
		*/
		static void SOUND_Error()
		{
			snd.inited = false;
			snd.playing = false;
			Mix_CloseAudio();
			MY_Err(MY_ErrDump(MY_L("APP00001|SDL_mixer chyba : %s"), Mix_GetError()));
		}

		/*
		==================================================
		Inicializace zvukoveho systemu
		==================================================
		*/
		void init(Size channels)
		{
			if (snd.inited)
			{
				return;
			}

			g_app.con->printf(MY_L("APP00002|\n===Inicializace zvukoveho systemu===\n"));
			g_app.con->printf(MY_L("APP00003|...Startuji knihovnu SDL_mixer\n"));

			// Init SDL audio sub sytem
			if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1)
			{
				g_app.con->printf(MY_L("APP00004|SDL_mixer chyba: neuspesna inicializace\n"));
				return;
			}

			// Init SDL_mixer library
			SND_TEST(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024))

			// Allocate channels
			channels = Mix_AllocateChannels(channels);

			g_app.con->printf(MY_L("APP00005|...Frekvence: %d\n...Kanalu: %d\n"), MIX_DEFAULT_FREQUENCY, channels);

			// Set up variables
			snd.channels = channels;
			snd.playing = false;
			snd.inited = true;
		}

		/*
		==================================================
		Nahrani modulu
		==================================================
		*/
		Size loadModule(const std::string& nm)
		{
			if (snd.inited)
			{
				Mix_Music *module = Mix_LoadMUS(nm.c_str());
				if (module == nullptr)
				{
					g_app.con->printf(MY_L("APP00006|SDL_mixer chyba: nelze nacist %s\n"), nm.c_str());
					return -1;
				}
				snd.modules.push_back(module);
				g_app.con->printf(MY_L("APP00007|...Nacten modul %s\n"), nm.c_str());
				return snd.modules.size() - 1;
			}

			return Size(-1);
		}

		/*
		==================================================
		Nahrani zvukoveho samplu
		==================================================
		*/
		Size loadSample(const std::string& nm)
		{
			if (snd.inited)
			{
				Mix_Chunk* sample = Mix_LoadWAV(nm.c_str());					
				if (sample == nullptr)
				{
					g_app.con->printf(MY_L("APP00008|SDL_mixer chyba: nelze nacist %s\n"), nm.c_str());
					return -1;
				}
				snd.samples.push_back(sample);
				g_app.con->printf(MY_L("APP00009|...Nacten sampl %s\n"), nm.c_str());
				return snd.samples.size() - 1;
			}

			return Size(-1);
		}

		/*
		==================================================
		Zastaveni hudby
		==================================================
		*/
		void stopMusic()
		{
			if (snd.playing && snd.inited)
			{
				Mix_HaltMusic();
				snd.playing = false;
			}
		}

		/*
		==================================================
		Spusteni vybraneho modulu (hudby)
		==================================================
		*/
		void startMusic(Size i, bool loop)
		{
			if (i < snd.modules.size() && snd.inited)
			{
				stopMusic();
				SND_TEST(Mix_PlayMusic(snd.modules[i], loop ? -1 : 0))
				snd.playing = true;
			}
		}

		/*
		==================================================
		Zahrani samplu
		==================================================
		*/
		void playSample(Size i)
		{
			if (i < snd.samples.size() && snd.inited)
			{
				if (Mix_Playing(-1) >= snd.channels)
					return;

				for (Size j = 0; j < snd.channels; j++)
				{
					if (!Mix_Playing(j))
					{
						if (!Mix_Paused(j))
						{
							SND_TEST(Mix_PlayChannel(j, snd.samples[i], 0))
							return;
						}
					}
				}
			}
		}

		/*
		==================================================
		Nastaveni hlasitosti hudby a samplu
		==================================================
		*/
		void volume(int volume)
		{
			if (snd.inited)
			{
				g_app.con->printf(MY_L("APP00010|...Nastaveni hlasitosti %d\n"), volume);
				Mix_VolumeMusic(volume);
				Mix_Volume(-1, volume);
			}
		}

		/*
		==================================================
		Deinicializace zvukoveho systemu
		==================================================
		*/
		void deInit()
		{
			if (!snd.inited)
			{
				// Stop and free modules
				stopMusic();

				for (Mix_Music* module : snd.modules)
				{
					Mix_FreeMusic(module);
				}

				// Free all mixing channels and samples
				Mix_AllocateChannels(0);

				for (Mix_Chunk* sample : snd.samples)
				{
					Mix_FreeChunk(sample);
				}

				snd.modules.clear();
				snd.samples.clear();

				Mix_CloseAudio();

				snd.inited = false;
			}
		}
	}
}