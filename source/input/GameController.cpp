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

#include "GameController.h"
#include "InputException.h"
#include <SDL2/SDL_gamecontroller.h>

namespace Duel6 {
    GameController::GameController(Instance instance, DeviceIndex deviceIndex) :
            open(SDL_bool::SDL_TRUE == SDL_JoystickGetAttached(instance)),
            instance(instance),
            instanceID(SDL_JoystickInstanceID(instance)),
            guid(SDL_JoystickGetGUID(instance)) {
        auto joyName = SDL_JoystickName(instance);
        name = joyName != NULL ? std::string(joyName) : "<unknown>"; // unlikely to happen
        openGameController(instance, deviceIndex);
    }

    const GameController::ControllerGUID &GameController::getGUID() const {
        return guid;
    }

    GameController::InstanceID GameController::getInstanceID() const {
        return instanceID;
    }

    GameController::AxisPosition GameController::getAxis(GameControllerAxis axis) const {
        if (!open) {
            return 0;
        }

        if (supportsGameControllerAPI) {
            SDL_GameControllerAxis gameControllerAxis;
            switch (axis) {
                case CONTROLLER_AXIS_LEFTX:
                    gameControllerAxis = SDL_CONTROLLER_AXIS_LEFTX;
                    break;
                case CONTROLLER_AXIS_LEFTY:
                    gameControllerAxis = SDL_CONTROLLER_AXIS_LEFTY;
                    break;
                case CONTROLLER_AXIS_RIGHTX:
                    gameControllerAxis = SDL_CONTROLLER_AXIS_RIGHTX;
                    break;
                case CONTROLLER_AXIS_RIGHTY:
                    gameControllerAxis = SDL_CONTROLLER_AXIS_RIGHTY;
                    break;
                case CONTROLLER_AXIS_TRIGGERLEFT:
                    gameControllerAxis = SDL_CONTROLLER_AXIS_TRIGGERLEFT;
                    break;
                case CONTROLLER_AXIS_TRIGGERRIGHT:
                    gameControllerAxis = SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
                    break;
                default:
                    D6_THROW(InputException, "Invalid joystick axis");
            }
            return SDL_GameControllerGetAxis(gameControllerInstance, gameControllerAxis);
        }
        return SDL_JoystickGetAxis(instance, axis);
    }

    const std::string &GameController::getName() const {
        return name;
    }

    bool GameController::isPressed(GameControllerButton button) const {
        if (!open) {
            return false;
        }
        if (supportsGameControllerAPI) {
            SDL_GameControllerButton bt;
            switch (button) {
                case CONTROLLER_BUTTON_X:
                    bt = SDL_CONTROLLER_BUTTON_X;
                    break;
                case CONTROLLER_BUTTON_A:
                    bt = SDL_CONTROLLER_BUTTON_A;
                    break;
                case CONTROLLER_BUTTON_B:
                    bt = SDL_CONTROLLER_BUTTON_B;
                    break;
                case CONTROLLER_BUTTON_Y:
                    bt = SDL_CONTROLLER_BUTTON_Y;
                    break;
                case CONTROLLER_BUTTON_BACK:
                    bt = SDL_CONTROLLER_BUTTON_BACK;
                    break;
                case CONTROLLER_BUTTON_GUIDE:
                    bt = SDL_CONTROLLER_BUTTON_GUIDE;
                    break;
                case CONTROLLER_BUTTON_START:
                    bt = SDL_CONTROLLER_BUTTON_START;
                    break;
                case CONTROLLER_BUTTON_LEFTSTICK:
                    bt = SDL_CONTROLLER_BUTTON_LEFTSTICK;
                    break;
                case CONTROLLER_BUTTON_RIGHTSTICK:
                    bt = SDL_CONTROLLER_BUTTON_RIGHTSTICK;
                    break;
                case CONTROLLER_BUTTON_LEFTSHOULDER:
                    bt = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
                    break;
                case CONTROLLER_BUTTON_RIGHTSHOULDER:
                    bt = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
                    break;
                case CONTROLLER_BUTTON_DPAD_UP:
                    bt = SDL_CONTROLLER_BUTTON_DPAD_UP;
                    break;
                case CONTROLLER_BUTTON_DPAD_DOWN:
                    bt = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
                    break;
                case CONTROLLER_BUTTON_DPAD_LEFT:
                    bt = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
                    break;
                case CONTROLLER_BUTTON_DPAD_RIGHT:
                    bt = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
                    break;
                default:
                    D6_THROW(InputException, "Invalid joystick button");
            }
            return SDL_GameControllerGetButton(gameControllerInstance, bt) == 1;
        }

        return SDL_JoystickGetButton(instance, button) == 1;
    }

    void GameController::close() {
        open = false;
        SDL_JoystickClose(instance); // This breaks everything, I don't know why
        if (supportsGameControllerAPI) {
            SDL_GameControllerClose(gameControllerInstance);
            supportsGameControllerAPI = false;
        }
    }

    void GameController::reset(Instance instance, DeviceIndex deviceIndex) {
        this->instance = instance;
        open = SDL_bool::SDL_TRUE == SDL_JoystickGetAttached(instance);
        instanceID = SDL_JoystickInstanceID(instance);
        openGameController(instance, deviceIndex);
    }

    void GameController::openGameController(Instance instance, DeviceIndex deviceIndex) {
        gameControllerInstance = SDL_GameControllerOpen(deviceIndex);
        supportsGameControllerAPI = gameControllerInstance != nullptr;
    }

    bool operator==(const GameController::ControllerGUID &l, const GameController::ControllerGUID &r) {
        for (std::size_t i = 0; i < std::size(l.data); ++i) {
            if (l.data[i] != r.data[i]) {
                return false;
            }
        }
        return true;
    }
}
