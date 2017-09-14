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
#include "SoundException.h"
#include "Sound.h"

namespace Duel6 {
    Sound::Sample::Sample(Sound *sound, Mix_Chunk *chunk)
            : sound(sound), chunk(chunk) {}

    Sound::Sample::Sample()
            : Sample(nullptr, nullptr) {}

    Sound::Sample::~Sample() {}

    void Sound::Sample::play() const {
        if (sound != nullptr && chunk != nullptr) {
            sound->playSample(chunk);
        }
    }

    Sound::Track::Track(Sound *sound, Mix_Music *module)
            : sound(sound), module(module) {}

    Sound::Track::Track()
            : Track(nullptr, nullptr) {}

    Sound::Track::~Track() {}

    void Sound::Track::play(bool loop) const {
        if (sound != nullptr && module != nullptr) {
            sound->startMusic(module, loop);
        }
    }

    Sound::Sound(Int32 channels, Console &console)
            : console(console), channels(channels), playing(false) {
        console.printLine("\n===Initialization of sound sub-system===");
        console.printLine("...Starting SDL_mixer library");

        // Init SDL audio sub sytem
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
            D6_THROW(SoundException, Format("SDL_Mixer error: {0}") << SDL_GetError());
        }

        // Init SDL_mixer library
        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
            D6_THROW(SoundException, Format("SDL_Mixer error: {0}") << Mix_GetError());
        }

        // Allocate channels
        channels = Mix_AllocateChannels(channels);
        console.print(Format("...Frequency: {0}\n...Channels: {0}\n") << MIX_DEFAULT_FREQUENCY << channels);
    }

    Sound::~Sound() {
        // Stop and free modules
        stopMusic();

        for (Mix_Music *module : modules) {
            Mix_FreeMusic(module);
        }

        // Free all mixing channels and samples
        Mix_AllocateChannels(0);

        for (Mix_Chunk *sample : samples) {
            Mix_FreeChunk(sample);
        }

        modules.clear();
        samples.clear();

        Mix_CloseAudio();
    }

    Sound::Track Sound::loadModule(const std::string &fileName) {
        Mix_Music *module = Mix_LoadMUS(fileName.c_str());
        if (module == nullptr) {
            D6_THROW(SoundException,
                     Format("SDL_mixer error: unable to load module {0} ({1})") << fileName << Mix_GetError());
        }
        modules.push_back(module);
        console.printLine(Format("...Module loaded: {0}") << fileName);
        return Track(this, module);
    }

    Sound::Sample Sound::loadSample(const std::string &fileName) {
        Mix_Chunk *sample = Mix_LoadWAV(fileName.c_str());
        if (sample == nullptr) {
            D6_THROW(SoundException,
                     Format("SDL_mixer error: unable to load sample {0} ({1})") << fileName << Mix_GetError());
        }
        samples.push_back(sample);
        console.printLine(Format("...Sample loaded: {0}") << fileName);
        return Sample(this, sample);
    }

    void Sound::stopMusic() {
        if (playing) {
            Mix_HaltMusic();
            playing = false;
        }
    }

    void Sound::startMusic(Mix_Music *music, bool loop) {
        stopMusic();
        if (Mix_PlayMusic(music, loop ? -1 : 0) == -1) {
            D6_THROW(SoundException, Format("SDL_Mixer error: {0}") << Mix_GetError());
        }
        playing = true;
    }

    void Sound::playSample(Mix_Chunk *chunk) {
        for (Int32 j = 0; j < channels; j++) {
            if (!Mix_Playing(j) && !Mix_Paused(j)) {
                if (Mix_PlayChannel(j, chunk, 0) == -1) {
                    D6_THROW(SoundException, Format("SDL_Mixer error: {0}") << Mix_GetError());
                }
                return;
            }
        }
    }

    void Sound::volume(Int32 volume) {
        console.printLine(Format("...Volume set to {0}") << volume);
        Mix_VolumeMusic(volume);
        Mix_Volume(-1, volume);
    }
}