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

#ifndef DUEL6_PLAYERCONTROLS_H
#define DUEL6_PLAYERCONTROLS_H

#include <memory>
#include <vector>
#include "Type.h"
#include "Input.h"

namespace Duel6 {
    class Control {
    public:
        virtual ~Control() {}

        virtual bool isPressed() const = 0;
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
        enum class Axis {
            Horizontal,
            Vertical
        };

        enum class Direction {
            Left,
            Right
        };

    private:
        const Input &input;
        Size joypadIndex;
        Axis axis;
        Direction direction;

    public:
        JoypadAxis(const Input &input, Size joypadIndex, Axis axis, Direction direction)
                : input(input), joypadIndex(joypadIndex), axis(axis), direction(direction) {}

        bool isPressed() const override;
    };

    class JoypadButton
            : public Control {
    private:
        const Input &input;
        Size joypadIndex;
        Size button;

    public:
        JoypadButton(const Input &input, Size joypadIndex, Size button)
                : input(input), joypadIndex(joypadIndex), button(button) {}

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
    };

    class PlayerControlsManager {
    private:
        static std::vector<std::unique_ptr<PlayerControls>> controls;
        const Input &input;

    public:
        PlayerControlsManager(const Input &input);

        Size getSize() const {
            return controls.size();
        }

        Size getNumAvailable() const;

        const PlayerControls &get(Size index) const {
            return *controls[index];
        }
    };
}

#endif