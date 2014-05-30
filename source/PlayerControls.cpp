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
#include "Globals.h"
#include "PlayerControls.h"

namespace Duel6
{
	std::vector<std::unique_ptr<PlayerControls>> d6Controls;

	bool KeyboardButton::isPressed() const
	{
		return g_inp.isPressed(keyCode);
	}

	bool JoypadAxis::isPressed() const
	{
		if (joypadIndex < g_inp.joysticks.size())
		{
			Int16 axisPosition = SDL_JoystickGetAxis(g_inp.joysticks[joypadIndex], (axis == Axis::Horizontal) ? 0 : 1);
			return (direction == Direction::Left) ? axisPosition < -1000 : axisPosition > 1000;

		}
		return false;
	}

	bool JoypadButton::isPressed() const
	{
		return (joypadIndex < g_inp.joysticks.size() && SDL_JoystickGetButton(g_inp.joysticks[joypadIndex], button) == 1);
	}

	void CONTROLS_Init()
	{
		d6Controls.push_back(
			std::unique_ptr<PlayerControls>(new PlayerControls(
			new KeyboardButton(SDLK_LEFT), new KeyboardButton(SDLK_RIGHT), 
			new KeyboardButton(SDLK_UP), new KeyboardButton(SDLK_DOWN),
			new KeyboardButton(SDLK_RCTRL), new KeyboardButton(SDLK_RSHIFT)
			)));
		d6Controls.push_back(
			std::unique_ptr<PlayerControls>(new PlayerControls(
			new KeyboardButton(SDLK_a), new KeyboardButton(SDLK_d), 
			new KeyboardButton(SDLK_w), new KeyboardButton(SDLK_s),
			new KeyboardButton(SDLK_q), new KeyboardButton(SDLK_e)
			)));
		d6Controls.push_back(
			std::unique_ptr<PlayerControls>(new PlayerControls(
			new KeyboardButton(SDLK_h), new KeyboardButton(SDLK_k), 
			new KeyboardButton(SDLK_u), new KeyboardButton(SDLK_j),
			new KeyboardButton(SDLK_o), new KeyboardButton(SDLK_l)
			)));
		d6Controls.push_back(
			std::unique_ptr<PlayerControls>(new PlayerControls(
			new KeyboardButton(SDLK_KP_1), new KeyboardButton(SDLK_KP_3), 
			new KeyboardButton(SDLK_KP_5), new KeyboardButton(SDLK_KP_2),
			new KeyboardButton(SDLK_KP_0), new KeyboardButton(SDLK_KP_PERIOD)
			)));
		d6Controls.push_back(
			std::unique_ptr<PlayerControls>(new PlayerControls(
			new JoypadAxis(0, JoypadAxis::Axis::Horizontal, JoypadAxis::Direction::Left),
			new JoypadAxis(0, JoypadAxis::Axis::Horizontal, JoypadAxis::Direction::Right),
			new JoypadButton(0, 0),
			new JoypadAxis(0, JoypadAxis::Axis::Vertical, JoypadAxis::Direction::Right),
			new JoypadButton(0, 1), new JoypadButton(0, 2)
			)));
		d6Controls.push_back(
			std::unique_ptr<PlayerControls>(new PlayerControls(
			new JoypadAxis(1, JoypadAxis::Axis::Horizontal, JoypadAxis::Direction::Left),
			new JoypadAxis(1, JoypadAxis::Axis::Horizontal, JoypadAxis::Direction::Right),
			new JoypadButton(1, 0),
			new JoypadAxis(1, JoypadAxis::Axis::Vertical, JoypadAxis::Direction::Right),
			new JoypadButton(1, 1), new JoypadButton(1, 2)
			)));
		d6Controls.push_back(
			std::unique_ptr<PlayerControls>(new PlayerControls(
			new JoypadAxis(2, JoypadAxis::Axis::Horizontal, JoypadAxis::Direction::Left),
			new JoypadAxis(2, JoypadAxis::Axis::Horizontal, JoypadAxis::Direction::Right),
			new JoypadButton(2, 0),
			new JoypadAxis(2, JoypadAxis::Axis::Vertical, JoypadAxis::Direction::Right),
			new JoypadButton(2, 1), new JoypadButton(2, 2)
			)));
		d6Controls.push_back(
			std::unique_ptr<PlayerControls>(new PlayerControls(
			new JoypadAxis(3, JoypadAxis::Axis::Horizontal, JoypadAxis::Direction::Left),
			new JoypadAxis(3, JoypadAxis::Axis::Horizontal, JoypadAxis::Direction::Right),
			new JoypadButton(3, 0),
			new JoypadAxis(3, JoypadAxis::Axis::Vertical, JoypadAxis::Direction::Right),
			new JoypadButton(3, 1), new JoypadButton(3, 2)
			)));
	}
}