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

#ifndef DUEL6_CONSOLECOMMANDS_H
#define DUEL6_CONSOLECOMMANDS_H

#include "console/console.h"
#include "Menu.h"
#include "Game.h"
#include "Sound.h"

namespace Duel6 {
    class ConsoleCommands {
    private:
        static void maxRounds(Console &console, const Console::Arguments &args, GameSettings &gameSettings);

        static void volume(Console &console, const Console::Arguments &args, Sound &sound);

        static void toggleRenderMode(Console &console, const Console::Arguments &args, GameSettings &gameSettings);

        static void toggleShowFps(Console &console, const Console::Arguments &args, GameSettings &gameSettings);

        static void musicOnOff(Console &console, const Console::Arguments &args, Menu &menu);

        static void joyScan(Console &console, const Console::Arguments &args, Menu &menu);

        static void loadSkin(Console &console, const Console::Arguments &args, Menu &menu);

        static void enableWeapon(Console &console, const Console::Arguments &args, GameSettings &gameSettings);

        static void ammoRange(Console &console, const Console::Arguments &args, GameSettings &gameSettings);

        static void openGLInfo(Console &console, const Console::Arguments &args);

        static void vsync(Console &console, const Console::Arguments &args);

        static void ghostMode(Console &console, const Console::Arguments &args, GameSettings &gameSettings);

        static void levelSelectionMode(Console &console, const Console::Arguments &args, GameSettings &gameSettings);

        static void shotCollision(Console &console, const Console::Arguments &args, GameSettings &gameSettings);

    public:
        static void registerCommands(Console &console, AppService &appService, Menu &menu, GameSettings &gameSettings);
    };
}

#endif
