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
#include <algorithm>
#include <SDL2/SDL.h>
#include "PlayerControls.h"

namespace Duel6 {
    std::vector<std::unique_ptr<PlayerControls>> PlayerControlsManager::controls;
    std::unique_ptr<PlayerControls> PlayerControlsManager::noopControls;

    bool KeyboardButton::isPressed() const {
        return input.isPressed(keyCode);
    }

    bool JoypadAxis::isPressed() const {
        auto axisPosition = gameController.getAxis(axis);
        return (direction == Direction::Left) ? axisPosition < -1000 : axisPosition > 1000;
    }

    bool JoypadButton::isPressed() const {
        return gameController.isPressed(button);
    }

    ComplexControl::ComplexControl(Control * c1, Control * c2):
        control1(c1), control2(c2) {
    }

    bool ComplexControl::isPressed() const {
        return control1->isPressed() || control2->isPressed();
    }
    std::unique_ptr<PlayerControls>
    PlayerControls::noOpControls(const std::string &name) {
        return std::make_unique<PlayerControls>(name,
            new NoOpControl(),
            new NoOpControl(),
            new NoOpControl(),
            new NoOpControl(),
            new NoOpControl(),
            new NoOpControl(),
            new NoOpControl());
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
    PlayerControls::joypadControls(const std::string &name, const GameController & gameController) {
        auto left = new ComplexControl(new JoypadAxis(gameController, GameController::GameControllerAxis::CONTROLLER_AXIS_LEFTX, JoypadAxis::Direction::Left),
                                       new JoypadButton(gameController, GameController::GameControllerButton::CONTROLLER_BUTTON_DPAD_LEFT));
        auto right = new ComplexControl(new JoypadAxis(gameController, GameController::GameControllerAxis::CONTROLLER_AXIS_LEFTX, JoypadAxis::Direction::Right),
                                        new JoypadButton(gameController, GameController::GameControllerButton::CONTROLLER_BUTTON_DPAD_RIGHT));
        auto up = new JoypadButton(gameController, GameController::GameControllerButton::CONTROLLER_BUTTON_X);
        auto down = new ComplexControl(new JoypadAxis(gameController, GameController::GameControllerAxis::CONTROLLER_AXIS_LEFTY, JoypadAxis::Direction::Right),
                                       new JoypadButton(gameController, GameController::GameControllerButton::CONTROLLER_BUTTON_DPAD_DOWN));
        auto shoot = new ComplexControl(new JoypadButton(gameController, GameController::GameControllerButton::CONTROLLER_BUTTON_A),
                                        new JoypadAxis(gameController, GameController::GameControllerAxis::CONTROLLER_AXIS_TRIGGERRIGHT, JoypadAxis::Direction::Right));
        auto pick = new ComplexControl(new JoypadButton(gameController, GameController::GameControllerButton::CONTROLLER_BUTTON_B),
                                       new JoypadAxis(gameController, GameController::GameControllerAxis::CONTROLLER_AXIS_TRIGGERLEFT, JoypadAxis::Direction::Right));
        auto status = new JoypadButton(gameController, GameController::GameControllerButton::CONTROLLER_BUTTON_Y);
        auto controls = std::make_unique<PlayerControls>(name, left, right, up, down, shoot, pick, status);
        return controls;
    }

    PlayerControlsManager::PlayerControlsManager(const Input &input)
            : input(input) {
        controls.push_back(
                PlayerControls::keyboardControls("K1: Arrows", input, SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN,
                                                 SDLK_RCTRL,
                                                 SDLK_RSHIFT, SDLK_RETURN));
        controls.push_back(
                PlayerControls::keyboardControls("K2: WSAD", input, SDLK_a, SDLK_d, SDLK_w, SDLK_s, 
                                                 SDLK_q,
                                                 SDLK_1, SDLK_2));
        controls.push_back(
                PlayerControls::keyboardControls("K3: IKJL", input, SDLK_j, SDLK_l, SDLK_i, SDLK_k, 
                                                 SDLK_u,
                                                 SDLK_7, SDLK_8));
        controls.push_back(
                PlayerControls::keyboardControls("K4: 5213", input, SDLK_KP_1, SDLK_KP_3, SDLK_KP_5, SDLK_KP_2,
                                                 SDLK_KP_PERIOD,
                                                 SDLK_KP_0, SDLK_KP_ENTER));
      	controls.push_back(
				        PlayerControls::keyboardControls("K5: TGFH", input, SDLK_f, SDLK_h, SDLK_t, SDLK_g,
                                                 SDLK_r,
                                                 SDLK_4, SDLK_5));
        controls.push_back(
                PlayerControls::keyboardControls("K6: /879", input, SDLK_KP_7, SDLK_KP_9, SDLK_KP_DIVIDE, SDLK_KP_8,
                                                 SDLK_PAGEDOWN,
                                                 SDLK_PAGEUP, SDLK_END));
        noopControls = PlayerControls::noOpControls("no-op");
    }

    void PlayerControlsManager::detectJoypads() {
        size_t joys = 0;
        for(auto const & c : input.getJoys()){
            std::string label = Format("J{0}: {1}") << joys++ << c.getName();
            if (label.size() > 18) {
                label.resize(18);
            }
            if (std::find_if(controls.begin(), controls.end(), [label](std::unique_ptr<PlayerControls> & control) {
                    return control->getDescription() == label;
                }) == std::end(controls)) {
                controls.push_back(PlayerControls::joypadControls(label, c));
            }

        }
    }

    Size PlayerControlsManager::getNumAvailable() const {
        return controls.size();
    }
}
