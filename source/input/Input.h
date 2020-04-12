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

#ifndef DUEL6_INPUT_INPUT_H
#define DUEL6_INPUT_INPUT_H

#include <unordered_set>
#include <list>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_joystick.h>
#include "../console/Console.h"
#include "GameController.h"
#include "../Type.h"

namespace Duel6 {

    class Input {
    private:
        std::unordered_set<SDL_Keycode> pressedKeys;
        std::list<GameController> gameControllers;
        Console &console;

    public:
        Input(Console &console);

        void setPressed(SDL_Keycode keyCode, bool pressed);

        bool isPressed(SDL_Keycode keyCode) const {
            if(console.isActive()) {
                return false;
            }
            return pressedKeys.find(keyCode) != pressedKeys.end();
        }

        const std::list<GameController> &getJoys() const {
            return gameControllers;
        }

        // handles reattaching
        void joyAttached(GameController::Instance instance, GameController::DeviceIndex deviceIndex) {
            auto GUID = GameController::toGUID(instance);
            auto instanceID = GameController::toInstanceID(instance);
            console.printLine(Format("Joy attached {0}") << instanceID);
            for (auto &gameController: gameControllers) {
                if (!gameController.isOpen() && gameController.getGUID() == GUID) {
                    gameController.reset(instance, deviceIndex);
                    console.printLine(Format("reattaching {0}") << gameController.getName());
                    return;
                }
            }

            gameControllers.emplace_back(instance, deviceIndex);
        }

        // handles reattaching
        void joyDetached(GameController::InstanceID instanceID) {
            console.printLine(Format("Joy detached {0}") << instanceID);
            for (auto &gameController: gameControllers) {
                if (gameController.getInstanceID() == instanceID) {
                    console.printLine(Format("closing joy {0}") << instanceID);
                    gameController.close();
                    return;
                }
            }
        }
    };
}

#endif
