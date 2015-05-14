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
#include "ListBox.h"

namespace Duel6
{
	namespace Gui
	{
		Listbox::Listbox(Desktop& desk, bool sb)
			: Control(desk)
		{
			listPos.items = 0;
			listPos.start = 0;
			now = -1;
			scrollBar = sb;
			if (sb)
			{
				slider = new Slider(desk);
				slider->connect(&listPos);
			}
		}

		Listbox::~Listbox()
		{
			clear();
		}

		void Listbox::clear()
		{
			items.clear();
			listPos.items = 0;
			listPos.start = 0;
			now = -1;
		}

		int Listbox::curItem()
		{
			return now;
		}

		void Listbox::setCur(int n)
		{
			now = n;
			listPos.start = now - listPos.showCount / 2;
		}

		void Listbox::delItem(int n)
		{
			if (!listPos.items || n < 0 || n >= listPos.items)
				return;

			items.erase(items.begin() + n);
			listPos.items--;
			if (now >= listPos.items)
				now = listPos.items - 1;
		}

		void Listbox::delItem(const std::string& item)
		{
			for(Int32 i = 0; i < size(); i++)
			{
				if(item.compare(getItem(i)) == 0)
				{
					delItem(i);
					return;
				}
			}
		}

		void Listbox::addItem(const std::string& item)
		{
			listPos.items++;
			items.push_back(item);
			if (listPos.items == 1)
				now = 0;
		}

		std::string& Listbox::getItem(int n)
		{
			return items.at(n);
		}

		long Listbox::size()
		{
			return items.size();
		}

		void Listbox::setPosition(int X, int Y, int W, int H, int fH)
		{
			x = X + 2;
			y = Y - 2;
			width = W << 3;
			field_height = fH;
			height = H * fH;
			listPos.showCount = H;
			if (scrollBar)
				slider->setPosition(X + (W << 3) + 4, Y, H * fH + 4);
		}

		void Listbox::check(const GuiContext& context)
		{
			const MouseState& ms = context.getMouseState();

			if (ms.isPressed() && ms.isInside(x, y, width, height))
			{
				now = listPos.start + ((y - ms.getY()) / field_height);
				if (now >= listPos.items)
					now = listPos.items - 1;
				// Event: Change
			}
		}

		void Listbox::draw(const Font& font) const
		{
			int     Y, i, shift;

			drawFrame(x - 2, y + 2, width + 4, height + 4, true);
			glBegin(GL_QUADS);
			glColor3ub(255, 255, 255);
			glVertex2i(x, y);
			glVertex2i(x + width, y);
			glVertex2i(x + width, y - height + 1);
			glVertex2i(x, y - height + 1);
			glEnd();

			if (items.empty())
				return;

			Y = y;
			shift = 15 + (field_height - 16) / 2;

			for (i = listPos.start; i < listPos.start + listPos.showCount; i++, Y -= field_height)
			{
				if (i >= listPos.items)
					break;
				if (i == now)
				{
					glBegin(GL_QUADS);
					glColor3ub(0, 0, 200);
					glVertex2i(x, Y);
					glVertex2i(x + width - 1, Y);
					glVertex2i(x + width - 1, Y - (field_height - 1));
					glVertex2i(x, Y - (field_height - 1));
					glEnd();
				}

				font.print(x, Y - shift, (i == now) ? Color(255) : Color(0), items[i]);
			}
		}
	}
}