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
#include "core/co_core.h"
#include "Type.h"

namespace Duel6
{
	class Control
	{
	public:
		virtual ~Control()
		{}
		virtual bool isPressed() const = 0;
	};

	class KeyboardButton
		: public Control
	{
	private:
		SDL_Keycode keyCode;

	public:
		KeyboardButton(SDL_Keycode keyCode)
			: keyCode(keyCode)
		{}

		bool isPressed() const override
		{
			return g_inp.isPressed(keyCode);
		}
	};

	class JoypadAxis
		: public Control
	{
	public:
		enum class Axis
		{
			Horizontal,
			Vertical
		};

		enum class Direction
		{
			Left,
			Right
		};

	private:
		Size joypadIndex;
		Axis axis;
		Direction direction;

	public:
		JoypadAxis(Size joypadIndex, Axis axis, Direction direction)
			: joypadIndex(joypadIndex), axis(axis), direction(direction)
		{}

		bool isPressed() const override
		{
			if (joypadIndex < g_inp.joysticks.size())
			{
				Int16 axisPosition = SDL_JoystickGetAxis(g_inp.joysticks[joypadIndex], (axis == Axis::Horizontal) ? 0 : 1);
				return (direction == Direction::Left) ? axisPosition < -1000 : axisPosition > 1000;

			}
			return false;
		}
	};

	class JoypadButton
		: public Control
	{
	private:
		Size joypadIndex;
		Size button;

	public:
		JoypadButton(Size joypadIndex, Size button)
			: joypadIndex(joypadIndex), button(button)
		{}

		bool isPressed() const override
		{
			return (joypadIndex < g_inp.joysticks.size() && SDL_JoystickGetButton(g_inp.joysticks[joypadIndex], button) == 1);
		}
	};


	class PlayerControls
	{
	private:
		std::unique_ptr<Control> left;
		std::unique_ptr<Control> right;
		std::unique_ptr<Control> up;
		std::unique_ptr<Control> down;
		std::unique_ptr<Control> shoot;
		std::unique_ptr<Control> pick;

	public:
		PlayerControls(Control* left, Control* right, Control* up, Control* down, Control* shoot, Control* pick)
			: left(left), right(right), up(up), down(down), shoot(shoot), pick(pick)
		{}

		const Control& getLeft() const
		{
			return *left;
		}

		const Control& getRight() const
		{
			return *right;
		}

		const Control& getUp() const
		{
			return *up;
		}

		const Control& getDown() const
		{
			return *down;
		}

		const Control& getShoot() const
		{
			return *shoot;
		}

		const Control& getPick() const
		{
			return *pick;
		}
	};

	extern std::vector<std::unique_ptr<PlayerControls>> d6Controls;
	void CONTROLS_Init();
}

#endif