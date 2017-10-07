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

#ifndef DUEL6_VIDEO_H
#define DUEL6_VIDEO_H

#include <SDL2/SDL.h>
#include "console/console.h"
#include "Type.h"
#include "ScreenParameters.h"
#include "ViewParameters.h"

#include "renderer/Renderer.h" // TODO: Remove

namespace Duel6 {
    extern Renderer *globRenderer;  // TODO: Glob fix

    class Video {
    public:
        enum class Mode {
            Orthogonal,
            Perspective
        };

    private:
        SDL_Window *window;
        SDL_GLContext glContext;
        Float32 fps;
        ScreenParameters screen;
        ViewParameters view;

    public:
        ~Video();

        void initialize(const std::string &name, const std::string &icon, Console &console);

        void screenUpdate(Console &console, const Font &font);

        const ScreenParameters &getScreen() const {
            return screen;
        }

        const ViewParameters &getView() const {
            return view;
        }

        Float32 getFps() const {
            return fps;
        }

        void setMode(Mode mode) const;

    private:
        void renderConsole(Console &console, const Font &font);

        void swapBuffers();

        void calculateFps();

        SDL_Window *createWindow(const std::string &name, const std::string &icon, const ScreenParameters &params,
                                 Console &console);

        SDL_GLContext createContext(const ScreenParameters &params, Console &console);

        Renderer *createRenderer();
    };
}

#endif