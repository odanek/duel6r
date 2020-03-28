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

#ifndef DUEL6_INPUT_PLAYERCONTROLS_H
#define DUEL6_INPUT_PLAYERCONTROLS_H

#include <memory>
#include <vector>
#include "../Type.h"
#include "Input.h"
#include "GameController.h"

namespace Duel6 {
    class Control {
    public:
        virtual ~Control() {}

        virtual bool isPressed() const = 0;

        virtual bool isJoyPadAxis() const {
            return false;
        }
    };
    class NoOpControl: public Control {
        bool isPressed() const override {
            return false;
        }
    };

    class ComplexControl: public Control {
    private:
        std::unique_ptr<Control> control1;
        std::unique_ptr<Control> control2;
    public:

        ComplexControl(Control * c1, Control * c2);

        bool isPressed() const override;
    private:

    };
    class KeyboardButton
            : public Control {
    private:
        const Input &input;
        SDL_Keycode keyCode;

    public:
        KeyboardButton(const Input &input, SDL_Keycode keyCode)
                : input(input), keyCode(keyCode) {}

        bool isPressed() const override;
    };

    class JoypadAxis
            : public Control {
    public:
        enum class Direction {
            Left,
            Right
        };

    private:
        const GameController & gameController;
        GameController::GameControllerAxis axis;
        Direction direction;

    public:
        JoypadAxis( const GameController & gameController, GameController::GameControllerAxis axis, Direction direction)
                : gameController(gameController), axis(axis), direction(direction) {}

        bool isPressed() const override;
        virtual bool isJoyPadAxis() const override {
            return true;
        }
    };

    class JoypadButton
            : public Control {
    private:
        const GameController & gameController;
        GameController::GameControllerButton button;

    public:
        JoypadButton(const GameController & gameController, GameController::GameControllerButton button)
                : gameController(gameController), button(button) {}

        bool isPressed() const override;
    };

    class PlayerControls {
    private:
        std::string description;
        std::unique_ptr<Control> left;
        std::unique_ptr<Control> right;
        std::unique_ptr<Control> up;
        std::unique_ptr<Control> down;
        std::unique_ptr<Control> shoot;
        std::unique_ptr<Control> pick;
        std::unique_ptr<Control> status;

    public:
        PlayerControls(const std::string &description, Control *left, Control *right, Control *up, Control *down,
                       Control *shoot, Control *pick, Control *status)
                : description(description), left(left), right(right), up(up), down(down), shoot(shoot), pick(pick),
                  status(status) {}

        const std::string &getDescription() const {
            return description;
        }

        const Control &getLeft() const {
            return *left;
        }

        const Control &getRight() const {
            return *right;
        }

        const Control &getUp() const {
            return *up;
        }

        const Control &getDown() const {
            return *down;
        }

        const Control &getShoot() const {
            return *shoot;
        }

        const Control &getPick() const {
            return *pick;
        }

        const Control &getStatus() const {
            return *status;
        }

    public:
        static std::unique_ptr<PlayerControls>
        keyboardControls(const std::string &name, const Input &input, SDL_Keycode left, SDL_Keycode right, SDL_Keycode up, SDL_Keycode down,
                         SDL_Keycode shoot, SDL_Keycode pick, SDL_Keycode status);
        static std::unique_ptr<PlayerControls> joypadControls(const std::string& name, const GameController & gameController);
        static std::unique_ptr<PlayerControls> noOpControls(const std::string& name);
    };

    class PlayerControlsManager {
    private:
        static std::vector<std::unique_ptr<PlayerControls>> controls;
        static std::unique_ptr<PlayerControls> noopControls;
        const Input &input;

    public:
        PlayerControlsManager(const Input &input);

        void detectJoypads();

        Size getSize() const {
            return controls.size();
        }

        Size getNumAvailable() const;

        const PlayerControls &get(Size index) const {
            return *controls[index];
        }
        static const PlayerControls &getNoop() {
            return *PlayerControlsManager::noopControls;
        }
    };
}

#endif
