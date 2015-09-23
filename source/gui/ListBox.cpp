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
#include <algorithm>
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
			selected = -1;
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
			selected = -1;
		}

		Int32 Listbox::selectedIndex() const
		{
			return selected;
		}

		const std::string& Listbox::selectedItem() const
		{
			return items[selected];
		}

		void Listbox::setCur(Int32 n)
		{
			selected = n;
			listPos.start = selected - listPos.showCount / 2;
		}

		void Listbox::delItem(Int32 index)
		{
			if (index < 0 || index >= listPos.items)
				return;

			items.erase(items.begin() + index);
			listPos.items--;
			if (selected >= listPos.items) {
				selected = listPos.items - 1;
			}
		}

		void Listbox::delItem(const std::string& item)
		{
			auto iter = std::find(items.begin(), items.end(), item);
			if (iter != items.end()) {
				delItem(iter - items.begin());
			}
		}

		void Listbox::addItem(const std::string& item)
		{
			listPos.items++;
			items.push_back(item);
			if (listPos.items == 1)
				selected = 0;
		}

		const std::string& Listbox::getItem(Size n) const
		{
			return items.at(n);
		}

		Size Listbox::size() const
		{
			return items.size();
		}

		void Listbox::setPosition(Int32 x, Int32 y, Int32 width, Int32 height, Int32 itemHeight)
		{
			this->x = x + 2;
			this->y = y - 2;
			this->width = width << 3;
			this->itemHeight = itemHeight;
			this->height = height * itemHeight;
			listPos.showCount = height;
			if (scrollBar)
			{
				slider->setPosition(x + (width << 3) + 4, y, height * itemHeight + 4);
			}
		}

		void Listbox::check(const GuiContext& context)
		{
			const MouseState& ms = context.getMouseState();

			if (ms.isPressed() && ms.isInside(x, y, width, height))
			{
				selected = listPos.start + ((y - ms.getY()) / itemHeight);
				if (selected >= listPos.items)
					selected = listPos.items - 1;
				// Event: Change
			}
		}

		void Listbox::draw(const Font& font) const
		{
			Int32 Y, i, shift;

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
			shift = 15 + (itemHeight - 16) / 2;

			for (i = listPos.start; i < listPos.start + listPos.showCount; i++, Y -= itemHeight)
			{
				if (i >= listPos.items)
					break;
				if (i == selected)
				{
					glBegin(GL_QUADS);
					glColor3ub(0, 0, 200);
					glVertex2i(x, Y);
					glVertex2i(x + width - 1, Y);
					glVertex2i(x + width - 1, Y - (itemHeight - 1));
					glVertex2i(x, Y - (itemHeight - 1));
					glEnd();
				}

				font.print(x, Y - shift, (i == selected) ? Color(255) : Color(0), items[i]);
			}
		}
	}
}