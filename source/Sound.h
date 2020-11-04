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
#include "console/Console.h"

namespace Duel6 {
    class Sound {
    public:
        class Sample {
        private:
            friend class Sound;

            Sound *sound;
            Mix_Chunk *chunk;

        private:
            Sample(Sound *sound, Mix_Chunk *chunk);

        public:
            Sample();

            ~Sample();

            void play() const;

            void play(float panning) const;
        };

        class Track {
        private:
            friend class Sound;

            Sound *sound;
            Mix_Music *module;

        private:
            Track(Sound *sound, Mix_Music *module);

        public:
            Track();

            ~Track();

            void play(bool loop) const;
        };

    private:
        Console &console;
        Int32 channels;
        bool playing;
        std::vector<Mix_Music *> modules;
        std::vector<Mix_Chunk *> samples;
        bool headless = false;
    public:
        Sound(Int32 channels, Console &console);

        ~Sound();

        Track loadModule(const std::string &fileName);

        Sample loadSample(const std::string &fileName);

        void volume(Int32 volume);

        void stopMusic();

    private:
        void startMusic(Mix_Music *music, bool loop);

        void playSample(Mix_Chunk *chunk, Uint8 left = 255, Uint8 right = 255);
    };
}

#endif
