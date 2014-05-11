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

#include <SDL/SDL_mixer.h>
#include "../project.h"

#define SND_TEST(x) if (x == -1) { SOUND_Error (); return; }

namespace Duel6
{
	struct sndSound_s
	{
		bool        inited;
		bool        playing;
		int         channels;
		int         modules;
		int         samples;
		Mix_Music   **module;
		Mix_Chunk   **sample;
	};

	// Inicializace struktury
	static sndSound_s   snd = { false, false, 0, 0, 0, NULL, NULL };

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
	void SOUND_Init(int channels, int samples, int modules)
	{
		int     i;

		if (snd.inited)
			return;

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
		snd.modules = modules;
		snd.samples = samples;
		snd.module = (Mix_Music **)MY_Alloc(modules * sizeof (Mix_Music *));
		snd.sample = (Mix_Chunk **)MY_Alloc(samples * sizeof (Mix_Chunk *));
		snd.playing = false;

		for (i = 0; i < modules; i++)
			snd.module[i] = NULL;

		for (i = 0; i < samples; i++)
			snd.sample[i] = NULL;

		snd.inited = true;
	}

	/*
	==================================================
	Nahrani modulu
	==================================================
	*/
	int SOUND_LoadModule(const char *nm)
	{
		int     i;

		if (!snd.inited)
			return -1;

		for (i = 0; i < snd.modules; i++)
			if (snd.module[i] == NULL)
			{
				snd.module[i] = Mix_LoadMUS(nm);
				if (snd.module[i] == NULL)
				{
					g_app.con->printf(MY_L("APP00006|SDL_mixer chyba: nelze nacist %s\n"), nm);
					return -1;
				}
				g_app.con->printf(MY_L("APP00007|...Nacten modul %s\n"), nm);
				return i;
			}

		return -1;
	}

	/*
	==================================================
	Nahrani zvukoveho samplu
	==================================================
	*/
	int SOUND_LoadSample(const char *nm)
	{
		int     i;

		if (!snd.inited)
			return -1;

		for (i = 0; i < snd.samples; i++)
			if (snd.sample[i] == NULL)
			{
				snd.sample[i] = Mix_LoadWAV(nm);
				if (snd.sample[i] == NULL)
				{
					g_app.con->printf(MY_L("APP00008|SDL_mixer chyba: nelze nacist %s\n"), nm);
					return -1;
				}
				g_app.con->printf(MY_L("APP00009|...Nacten sampl %s\n"), nm);
				return i;
			}

		return -1;
	}

	/*
	==================================================
	Zastaveni hudby
	==================================================
	*/
	void SOUND_StopMusic()
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
	void SOUND_StartMusic(int i, bool loop)
	{
		if (i >= 0 && i < snd.modules && snd.inited)
			if (snd.module[i] != NULL)
			{
				SOUND_StopMusic();
				SND_TEST(Mix_PlayMusic(snd.module[i], loop ? -1 : 0))
					snd.playing = true;
			}
	}

	/*
	==================================================
	Zahrani samplu
	==================================================
	*/
	void SOUND_PlaySample(int i)
	{
		int     j;

		if (i >= 0 && i < snd.samples && snd.inited)
			if (snd.sample[i] != NULL)
			{
				if (Mix_Playing(-1) >= snd.channels)
					return;

				for (j = 0; j < snd.channels; j++)
					if (!Mix_Playing(j))
						if (!Mix_Paused(j))
						{
							SND_TEST(Mix_PlayChannel(j, snd.sample[i], 0))
								return;
						}
			}
	}

	/*
	==================================================
	Nastaveni hlasitosti hudby a samplu
	==================================================
	*/
	void SOUND_Volume(int volume)
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
	void SOUND_DeInit()
	{
		int     i;

		if (!snd.inited)
			return;

		// Stop and free modules
		SOUND_StopMusic();

		for (i = 0; i < snd.modules; i++)
			if (snd.module[i] != NULL)
				Mix_FreeMusic(snd.module[i]);

		// Free all mixing channels and samples
		Mix_AllocateChannels(0);

		for (i = 0; i < snd.samples; i++)
			if (snd.sample[i] != NULL)
				Mix_FreeChunk(snd.sample[i]);

		MY_Free(snd.module);
		MY_Free(snd.sample);

		Mix_CloseAudio();

		snd.inited = false;
	}
}