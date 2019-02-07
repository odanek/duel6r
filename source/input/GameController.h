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

#ifndef DUEL6_INPUT_GAMECONTROLLER_H
#define DUEL6_INPUT_GAMECONTROLLER_H

#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_gamecontroller.h>
#include <string>
#include <array>
#include <iterator>

namespace Duel6 {
    class GameController {

    public:
        enum GameControllerAxis {
            CONTROLLER_AXIS_LEFTX = 0,
            CONTROLLER_AXIS_LEFTY,
            CONTROLLER_AXIS_RIGHTX,
            CONTROLLER_AXIS_RIGHTY,
            CONTROLLER_AXIS_TRIGGERLEFT,
            CONTROLLER_AXIS_TRIGGERRIGHT
        };
        enum GameControllerButton {
            CONTROLLER_BUTTON_X = 0,
            CONTROLLER_BUTTON_A,
            CONTROLLER_BUTTON_B,
            CONTROLLER_BUTTON_Y,

            CONTROLLER_BUTTON_BACK,
            CONTROLLER_BUTTON_GUIDE,
            CONTROLLER_BUTTON_START,
            CONTROLLER_BUTTON_LEFTSTICK,
            CONTROLLER_BUTTON_RIGHTSTICK,
            CONTROLLER_BUTTON_LEFTSHOULDER,
            CONTROLLER_BUTTON_RIGHTSHOULDER,
            CONTROLLER_BUTTON_DPAD_UP,
            CONTROLLER_BUTTON_DPAD_DOWN,
            CONTROLLER_BUTTON_DPAD_LEFT,
            CONTROLLER_BUTTON_DPAD_RIGHT
        };

        using DeviceIndex = Sint16;
        using InstanceID = SDL_JoystickID;
        using Instance = SDL_Joystick *;
        using GameControllerInstance = SDL_GameController *;
        using ControllerGUID = SDL_JoystickGUID;
        using AxisPosition = Sint16;

        explicit GameController(Instance instance, DeviceIndex deviceIndex);

        virtual ~GameController() = default;

        bool isPressed(GameControllerButton button) const;

        AxisPosition getAxis(GameControllerAxis axis) const;

        const ControllerGUID &getGUID() const;

        InstanceID getInstanceID() const;

        const std::string &getName() const;

        void close();

        void reset(Instance instance, DeviceIndex deviceIndex);

        bool isOpen() { return open; }

        static InstanceID toInstanceID(Instance instance) {
            return SDL_JoystickInstanceID(instance);
        }

        static ControllerGUID toGUID(Instance instance) {
            return SDL_JoystickGetGUID(instance);
        }

    private:
        bool open = false;
        bool supportsGameControllerAPI = false;
        Instance instance;
        InstanceID instanceID;
        const ControllerGUID guid;
        GameControllerInstance gameControllerInstance;
        std::string name;

        void openGameController(Instance instance, DeviceIndex deviceIndex);

    public:
        GameController(const GameController &) = delete;

        GameController(GameController &&) = delete;

        GameController &operator=(const GameController &) = delete;
    };

    bool operator==(const GameController::ControllerGUID &l, const GameController::ControllerGUID &r);
}

#endif
