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

#ifndef DUEL6_APPLICATION_H
#define DUEL6_APPLICATION_H

#include <memory>
#include "Context.h"
#include "AppService.h"
#include "Menu.h"
#include "Game.h"
#include "Video.h"

namespace Duel6 {
    class Application {
    private:
        Font font;
        Console console;
        TextureManager textureManager;
        Video video;
        Input input;
        Sound sound;
        AppService service;
        Menu menu;
        GameSettings gameSettings;
        std::unique_ptr<GameResources> gameResources;
        std::unique_ptr<Game> game;
        bool requestClose;

    public:
        Application();

        ~Application();

        void setup(Int32 argc, char **argv);

        void run();

        void tearDown();

    private:
        void processEvents(Context &context);

        void textInputEvent(Context &context, const TextInputEvent &event);

        void keyEvent(Context &context, const KeyPressEvent &event);

        void mouseButtonEvent(Context &context, const MouseButtonEvent &event);

        void mouseMotionEvent(Context &context, const MouseMotionEvent &event);

        void mouseWheelEvent(Context &context, const MouseWheelEvent &event);

        void syncUpdateAndRender(Context &context);
    };
}

#endif
