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

#include <SDL2/SDL.h>
#include "PlayerControls.h"

namespace Duel6 {
    std::vector<std::unique_ptr<PlayerControls>> PlayerControlsManager::controls;

    bool KeyboardButton::isPressed() const {
        return input.isPressed(keyCode);
    }

    bool JoypadAxis::isPressed() const {
        if (joypadIndex < input.getNumJoypads()) {
            Int16 axisPosition = SDL_JoystickGetAxis(input.getJoypad(joypadIndex), (axis == Axis::Horizontal) ? 0 : 1);
            return (direction == Direction::Left) ? axisPosition < -1000 : axisPosition > 1000;

        }
        return false;
    }

    bool JoypadButton::isPressed() const {
        return (joypadIndex < input.getNumJoypads() &&
                SDL_JoystickGetButton(input.getJoypad(joypadIndex), (int) button) == 1);
    }

    PlayerControlsManager::PlayerControlsManager(const Input &input)
            : input(input) {
        controls.push_back(
                std::make_unique<PlayerControls>("Keys 1",
                                                 new KeyboardButton(input, SDLK_LEFT),
                                                 new KeyboardButton(input, SDLK_RIGHT),
                                                 new KeyboardButton(input, SDLK_UP),
                                                 new KeyboardButton(input, SDLK_DOWN),
                                                 new KeyboardButton(input, SDLK_RCTRL),
                                                 new KeyboardButton(input, SDLK_RSHIFT),
                                                 new KeyboardButton(input, SDLK_RETURN)
                ));
        controls.push_back(
                std::make_unique<PlayerControls>("Keys 2",
                                                 new KeyboardButton(input, SDLK_a),
                                                 new KeyboardButton(input, SDLK_d),
                                                 new KeyboardButton(input, SDLK_w),
                                                 new KeyboardButton(input, SDLK_s),
                                                 new KeyboardButton(input, SDLK_q),
                                                 new KeyboardButton(input, SDLK_e),
                                                 new KeyboardButton(input, SDLK_TAB)
                ));
        controls.push_back(
                std::make_unique<PlayerControls>("Keys 3",
                                                 new KeyboardButton(input, SDLK_j),
                                                 new KeyboardButton(input, SDLK_l),
                                                 new KeyboardButton(input, SDLK_i),
                                                 new KeyboardButton(input, SDLK_k),
                                                 new KeyboardButton(input, SDLK_y),
                                                 new KeyboardButton(input, SDLK_h),
                                                 new KeyboardButton(input, SDLK_u)
                ));
        controls.push_back(
                std::make_unique<PlayerControls>("Keys 4",
                                                 new KeyboardButton(input, SDLK_KP_1),
                                                 new KeyboardButton(input, SDLK_KP_3),
                                                 new KeyboardButton(input, SDLK_KP_5),
                                                 new KeyboardButton(input, SDLK_KP_2),
                                                 new KeyboardButton(input, SDLK_KP_0),
                                                 new KeyboardButton(input, SDLK_KP_PERIOD),
                                                 new KeyboardButton(input, SDLK_KP_ENTER)
                ));

        for (Int32 i = 0; i < 10; i++) {
            controls.push_back(
                    std::make_unique<PlayerControls>(Format("Joypad {0}") << i + 1,
                                                     new JoypadAxis(input, i,
                                                                    JoypadAxis::Axis::Horizontal,
                                                                    JoypadAxis::Direction::Left),
                                                     new JoypadAxis(input, i,
                                                                    JoypadAxis::Axis::Horizontal,
                                                                    JoypadAxis::Direction::Right),
                                                     new JoypadButton(input, i, 0),
                                                     new JoypadAxis(input, i,
                                                                    JoypadAxis::Axis::Vertical,
                                                                    JoypadAxis::Direction::Right),
                                                     new JoypadButton(input, i, 1),
                                                     new JoypadButton(input, i, 2),
                                                     new JoypadButton(input, i, 3)
                    ));
        }
    }

    Size PlayerControlsManager::getNumAvailable() const {
        return 4 + input.getNumJoypads();
    }
}