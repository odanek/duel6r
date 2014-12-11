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
#include <SDL2/SDL_opengl.h>
#include "Font.h"
#include "Globals.h"
#include "Gui.h"

namespace Duel6
{
	namespace Gui
	{
		static Color glibCBack(192, 192, 192), glibCFL(235, 235, 235), glibCFD(0, 0, 0);

		void Control::drawFrame(Int32 x, Int32 y, Int32 w, Int32 h, bool p) const
		{
			w--;
			h--;

			glBegin(GL_LINES);
			if (p)
			{
				glColor3ub(glibCFD.getRed(), glibCFD.getGreen(), glibCFD.getBlue());
			}
			else
			{
				glColor3ub(glibCFL.getRed(), glibCFL.getGreen(), glibCFL.getBlue());;
			}

			glVertex2i(x, y); glVertex2i(x, y - h);
			glVertex2i(x + 1, y); glVertex2i(x + 1, y - h + 1);
			glVertex2i(x, y); glVertex2i(x + w, y);
			glVertex2i(x, y - 1); glVertex2i(x + w - 1, y - 1);

			if (p)
			{
				glColor3ub(glibCFL.getRed(), glibCFL.getGreen(), glibCFL.getBlue());;
			}
			else
			{
				glColor3ub(glibCFD.getRed(), glibCFD.getGreen(), glibCFD.getBlue());
			}


			glVertex2i(x + w, y - h); glVertex2i(x + w, y);
			glVertex2i(x + w - 1, y - h); glVertex2i(x + w - 1, y - 1);
			glVertex2i(x + w, y - h); glVertex2i(x, y - h);
			glVertex2i(x + w, y - h + 1); glVertex2i(x + 1, y - h + 1);
			glEnd();
		}

		/*
		================
		button_c
		================
		*/
		Button::Button(Desktop& desk)
			: Control(desk)
		{
			pressed = false;
		}

		Button::~Button()
		{
		}

		void Button::setCaption(const std::string& caption)
		{
			this->caption = caption;
		}

		void Button::setPosition(int X, int Y, int W, int H)
		{
			x = X;
			y = Y;
			width = W;
			height = H;
		}

		void Button::check(const GuiContext& context)
		{
			const MouseState& ms = context.getMouseState();

			if (ms.isPressed() && ms.isInside(x, y, width, height))
			{
				if (!pressed)
				{
					pressed = true;
				}
			}
			else
			{
				if (pressed)
				{
					pressed = false;
					for (auto& callback : clickListeners)
					{
						callback(Event(*this, Event::Type::Click));
					}
				}				
			}
		}

		void Button::draw(const Font& font) const
		{
			Int32 px, py;

			drawFrame(x, y, width, height, pressed);
			px = x + (width >> 1) - (caption.length() << 2) + pressed;
			py = y - (height >> 1) - 7 - pressed;
			font.print(px, py, Color(0), caption);
		}

		/*
		================
		listbox_c
		================
		*/
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

		void Listbox::addItem(const std::string& item)
		{
			listPos.items++;
			items.push_back(item);
			if (listPos.items == 1)
				now = 0;
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

		/*
		================
		Combobox
		================
		*/
		Combobox::Combobox(Desktop& desk)
			: Control(desk)
		{
			now = -1;

			left = new Button(desk);
			left->setCaption(" ");
			right = new Button(desk);
			right->setCaption(" ");
		}

		Combobox::~Combobox()
		{
			clear();
		}

		void Combobox::clear()
		{
			items.clear();
			now = -1;
		}

		int Combobox::curItem()
		{
			return now;
		}

		void Combobox::setCur(int n)
		{
			now = n;
		}

		void Combobox::delItem(int n)
		{
			if (n < 0 || n >= (int)items.size())
				return;

			items.erase(items.begin() + n);
			if (now >= (int)items.size())
				now = int(items.size()) - 1;
		}

		void Combobox::addItem(const std::string& item)
		{
			items.push_back(item);
			if (items.size() == 1)
			{
				now = 0;
			}
		}

		void Combobox::setPosition(int X, int Y, int W, int H)
		{
			left->setPosition(X, Y - 1, 18, 18);
			right->setPosition(X + W - 18, Y - 1, 18, 18);
			width = W;
			x = X;
			y = Y;
		}

		void Combobox::check(const GuiContext& context)
		{
			static Int32 pWait = 0;
			bool change = false;

			if (pWait)
				pWait--;

			if (left->isPressed() && !pWait)
			{
				pWait = 120;  // TODO: Fix, hack to wait 2 seconds
				if (now > 0)
				{
					now--;
					change = true;
				}
			}

			if (right->isPressed() && !pWait)
			{
				pWait = 120;  // TODO: Fix, hack to wait 2 seconds
				if (now + 1 < (int)items.size())
				{
					now++;
					change = true;
				}
			}

			if (change)
			{
				// EventType::Change;
			}
		}

		void Combobox::draw(const Font& font) const
		{
			int     px, py;

			drawFrame(x + 20, y, width - 40, 18, true);
			glBegin(GL_QUADS);
			glColor3ub(255, 255, 255);
			glVertex2i(x + 22, y - 1);
			glVertex2i(x + width - 21, y - 1);
			glVertex2i(x + width - 21, y - 16);
			glVertex2i(x + 22, y - 16);
			glEnd();

			glBegin(GL_TRIANGLES);
			px = left->getX() + 7 + (left->isPressed() ? 1 : 0);
			py = left->getY() - 4 - (left->isPressed() ? 1 : 0);
			glColor3ub(0, 0, 0);
			glVertex2i(px + 2, py);
			glVertex2i(px + 2, py - 7);
			glVertex2i(px - 2, py - 4);

			px = right->getX() + 7 + (right->isPressed() ? 1 : 0);
			py = right->getY() - 4 - (right->isPressed() ? 1 : 0);
			glVertex2i(px - 1, py);
			glVertex2i(px - 1, py - 7);
			glVertex2i(px + 3, py - 4);
			glEnd();

			if (items.empty())
				return;

			font.print(x + 25, y - 15, Color(0), items[now]);
		}

		/*
		================
		label_c
		================
		*/
		Label::Label(Desktop& desk)
			: Control(desk)
		{
		}

		Label::~Label()
		{
		}

		void Label::setPosition(int X, int Y, int W, int H)
		{
			x = X;
			y = Y;
			width = W;
			height = H;
		}

		void Label::setCaption(const std::string& caption)
		{
			text = caption;
		}

		void Label::draw(const Font& font) const
		{
			glBegin(GL_QUADS);
			glColor3ub(170, 170, 170);
			glVertex2i(x, y);
			glVertex2i(x + width - 1, y);
			glVertex2i(x + width - 1, y - height + 1);
			glVertex2i(x, y - height + 1);
			glEnd();

			font.print(x, y - 15, Color(0), text);
		}

		void Label::check(const GuiContext& context)
		{}

		/*
		================
		slider_c
		================
		*/
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

		void Slider::connect(SliderPosition* to)
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
			glColor3ub(glibCBack.getRed(), glibCBack.getGreen(), glibCBack.getBlue());
			glVertex2i(x, y - s);
			glVertex2i(x + 15, y - s);
			glVertex2i(x + 15, y - s - h + 1);
			glVertex2i(x, y - s - h + 1);
			glEnd();

			drawFrame(x, y - s, 16, h, false);
		}

		/*
		================
		Textbox
		================
		*/
		Textbox::Textbox(Desktop& desk)
			: Control(desk)
		{
		}

		Textbox::~Textbox()
		{
		}

		void Textbox::setPosition(int X, int Y, int W, int M, const std::string& allowed)
		{
			x = X + 2;
			y = Y - 2;
			max = M;
			width = W;

			allowedCharacters = allowed;
			text.clear();
		}

		void Textbox::keyEvent(SDL_Keycode keyCode)
		{
			if (!text.empty() && keyCode == SDLK_BACKSPACE)
			{
				text.pop_back();
			}
		}

		void Textbox::textInputEvent(const char* newText)
		{
			while (*newText != 0)
			{
				if ((int)text.length() < max && allowedCharacters.find(*newText) != std::string::npos)
				{
					text.push_back(*newText);
				}

				++newText;
			}
		}

		const std::string& Textbox::getText() const
		{
			return text;
		}

		void Textbox::flush()
		{
			text.clear();
		}

		void Textbox::check(const GuiContext& context)
		{
			const MouseState& ms = context.getMouseState();

			if (ms.isPressed() && ms.isInside(x, y, (width + 1) << 3, 18))
			{
				// EventType::Active;
			}
		}

		void Textbox::draw(const Font& font) const
		{
			int     w = (width << 3) + 8;

			drawFrame(x - 2, y + 2, w + 4, 22, true);
			glBegin(GL_QUADS);
			glColor3ub(255, 255, 255);
			glVertex2i(x, y);
			glVertex2i(x + w, y);
			glVertex2i(x + w, y - 17);
			glVertex2i(x, y - 17);
			glEnd();

			font.print(x, y - 16, Color(0), text + "_");
		}

		/*
		================
		desk_c
		================
		*/
		Desktop::Desktop()
		{
		}

		Desktop::~Desktop()
		{
		}

		void Desktop::addControl(Control* control)
		{
			controls.push_back(std::unique_ptr<Control>(control));
		}

		void Desktop::screenSize(Int32 scrWidth, Int32 scrHeight, Int32 trX, Int32 trY)
		{
			screenWidth = scrWidth;
			screenHeight = scrHeight;
			this->trX = trX;
			this->trY = trY;
		}

		void Desktop::update()
		{
			Int32 mx, my, mb;
			mb = SDL_GetMouseState(&mx, &my) & SDL_BUTTON(SDL_BUTTON_LEFT);
			mx -= trX;
			my = screenHeight - my - trY;
			GuiContext context(MouseState(mx, my, mb != 0));

			for (auto& control : controls)
			{
				control->check(context);
			}
		}

		void Desktop::draw(const Font& font) const
		{
			glBegin(GL_QUADS);
			glColor3ub(glibCBack.getRed(), glibCBack.getGreen(), glibCBack.getBlue());
			glVertex2i(0, 0);
			glVertex2i(screenWidth, 0);
			glVertex2i(screenWidth, screenHeight);
			glVertex2i(0, screenHeight);
			glEnd();

			glPushMatrix();
			glTranslatef((GLfloat)trX, (GLfloat)trY, 0);

			for (auto& control : controls)
			{
				control->draw(font);
			}

			glPopMatrix();
		}

		void Desktop::keyEvent(SDL_Keycode keyCode)
		{
			for (auto& control : controls)
			{
				control->keyEvent(keyCode);
			}
		}

		void Desktop::textInputEvent(const char* text)
		{
			for (auto& control : controls)
			{
				control->textInputEvent(text);
			}
		}
	}
}