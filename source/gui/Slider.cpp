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

#include <SDL2/SDL_opengl.h>
#include "Slider.h"

namespace Duel6
{
	namespace Gui
	{
		namespace
		{
			Color bcgColor(192, 192, 192);
		}

		Slider::Slider(Desktop& desk)
				: Control(desk)
		{
			up = new Button(desk);
			up->setCaption(" ");
			down = new Button(desk);
			down->setCaption(" ");
			pos = nullptr;
		}

		void Slider::setPosition(int X, int Y, int H)
		{
			up->setPosition(X, Y, 16, 16);
			down->setPosition(X, Y - H + 16, 16, 16);
			x = X;
			y = Y - 17;
			height = H - 32;
		}

		void Slider::connect(Slider::Position* to)
		{
			pos = to;
		}

		void Slider::check(const GuiContext& context)
		{
			Int32 h = height, o = pos->start;

			if (!up->isPressed() && !down->isPressed())
				pWait = 0;
			else
			if (pWait)
				pWait--;

			if (up->isPressed() && !pWait)
			{
				pWait = 6;
				pos->start--;
			}

			if (down->isPressed() && !pWait)
			{
				pWait = 6;
				pos->start++;
			}

			const MouseState& ms = context.getMouseState();

			if (ms.isPressed() && ms.isInside(x, y, 16, height))
			{
				if (pos->items)
				{
					h = pos->showCount * height / pos->items;
					if (h > height)
						h = height;
					if (h < 5)
						h = 5;
				}
				h = y - (h >> 1) - ms.getY();
				pos->start = h * pos->items / height;
			}

			if (pos->start > pos->items - pos->showCount)
				pos->start = pos->items - pos->showCount;
			if (pos->start < 0)
				pos->start = 0;

			if (pos->start != o)
			{
				// EventType::Change;
			}
		}

		void Slider::draw(const Font& font) const
		{
			int px, py, h = height, s = 0;

			px = up->getX() + 7 + (up->isPressed() ? 1 : 0);
			py = up->getY() - 4 - (up->isPressed() ? 1 : 0);
			glBegin(GL_TRIANGLES);
			glColor3ub(0, 0, 0);
			glVertex2i(px, py);
			glVertex2i(px + 3, py - 6);
			glVertex2i(px - 3, py - 6);
			glEnd();

			px = down->getX() + 7 + (down->isPressed() ? 1 : 0);
			py = down->getY() - 4 + (down->isPressed() ? 1 : 0);
			glBegin(GL_TRIANGLES);
			glColor3ub(0, 0, 0);
			glVertex2i(px - 3, py);
			glVertex2i(px + 3, py);
			glVertex2i(px, py - 6);
			glEnd();

			glBegin(GL_QUADS);
			glColor3ub(230, 230, 230);
			glVertex2i(x, y);
			glVertex2i(x + 15, y);
			glVertex2i(x + 15, y - height + 1);
			glVertex2i(x, y - height + 1);
			glEnd();

			if (pos->items)
			{
				h = pos->showCount * height / pos->items;
				if (h > height)
					h = height;
				if (h < 5)
					h = 5;

				s = pos->start * height / pos->items;
				if (s > height - h)
					s = height - h;
			}

			glBegin(GL_QUADS);
			glColor3ub(bcgColor.getRed(), bcgColor.getGreen(), bcgColor.getBlue());
			glVertex2i(x, y - s);
			glVertex2i(x + 15, y - s);
			glVertex2i(x + 15, y - s - h + 1);
			glVertex2i(x, y - s - h + 1);
			glEnd();

			drawFrame(x, y - s, 16, h, false);
		}
	}
}
