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
        Int16 axisPosition = SDL_JoystickGetAxis(joypad, (axis == Axis::Horizontal) ? 0 : 1);
        return (direction == Direction::Left) ? axisPosition < -1000 : axisPosition > 1000;
    }

    bool JoypadButton::isPressed() const {
        return (SDL_JoystickGetButton(joypad, (int) button) == 1);
    }

    std::unique_ptr<PlayerControls>
    PlayerControls::keyboardControls(const std::string &name, const Input &input, SDL_Keycode left, SDL_Keycode right,
                                     SDL_Keycode up, SDL_Keycode down, SDL_Keycode shoot, SDL_Keycode pick,
                                     SDL_Keycode status) {
        auto cleft = new KeyboardButton(input, left);
        auto cright = new KeyboardButton(input, right);
        auto cup = new KeyboardButton(input, up);
        auto cdown = new KeyboardButton(input, down);
        auto cshoot = new KeyboardButton(input, shoot);
        auto cpick = new KeyboardButton(input, pick);
        auto cstatus = new KeyboardButton(input, status);
        auto controls = std::make_unique<PlayerControls>(name, cleft, cright, cup, cdown, cshoot, cpick, cstatus);
        return controls;
    }

    std::unique_ptr<PlayerControls>
    PlayerControls::joypadControls(const std::string &name, const Input &input, SDL_Joystick *joypad) {
        auto left = new JoypadAxis(input, joypad, JoypadAxis::Axis::Horizontal, JoypadAxis::Direction::Left);
        auto right = new JoypadAxis(input, joypad, JoypadAxis::Axis::Horizontal, JoypadAxis::Direction::Right);
        auto up = new JoypadButton(input, joypad, 0);
        auto down = new JoypadAxis(input, joypad, JoypadAxis::Axis::Vertical, JoypadAxis::Direction::Right);
        auto shoot = new JoypadButton(input, joypad, 1);
        auto pick = new JoypadButton(input, joypad, 2);
        auto status = new JoypadButton(input, joypad, 3);
        auto controls = std::make_unique<PlayerControls>(name, left, right, up, down, shoot, pick, status);
        return controls;
    }

    PlayerControlsManager::PlayerControlsManager(const Input &input)
            : input(input) {
        controls.push_back(
                PlayerControls::keyboardControls("K1: Arrows", input, SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN,
                                                 SDLK_RCTRL,
                                                 SDLK_RSHIFT, SDLK_APPLICATION));
        controls.push_back(
                PlayerControls::keyboardControls("K2: WSAD", input, SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_q,
                                                 SDLK_1, SDLK_2));
        controls.push_back(
                PlayerControls::keyboardControls("K3: IKJL", input, SDLK_j, SDLK_l, SDLK_i, SDLK_k, SDLK_u,
                                                 SDLK_7, SDLK_8));
        controls.push_back(
                PlayerControls::keyboardControls("K4: 5213", input, SDLK_KP_1, SDLK_KP_3, SDLK_KP_5, SDLK_KP_2,
                                                 SDLK_KP_0,
                                                 SDLK_KP_PERIOD, SDLK_KP_ENTER));
        controls.push_back(
                PlayerControls::keyboardControls("K5: TGFH", input, SDLK_f, SDLK_h, SDLK_t, SDLK_g,
                                                 SDLK_r,
                                                 SDLK_4, SDLK_5));
        controls.push_back(
                PlayerControls::keyboardControls("K6: [';ENTER", input, SDLK_SEMICOLON, SDLK_RETURN, SDLK_LEFTBRACKET, SDLK_QUOTE,
                                                 SDLK_p,
                                                 SDLK_0, SDLK_MINUS));
    }

    void PlayerControlsManager::detectJoypads() {
        controls.resize(4);
        for (Int32 i = 0; i < input.getNumJoypads(); i++) {
            auto joypad = input.getJoypad(i);
            auto name = SDL_JoystickName(joypad);
            std::string label = Format("J{0}: {1}") << (i + 1) << name;
            if (label.size() > 18) {
                label.resize(18);
            }
            controls.push_back(PlayerControls::joypadControls(label, input, joypad));
        }
    }

    Size PlayerControlsManager::getNumAvailable() const {
        return controls.size();
    }
}
