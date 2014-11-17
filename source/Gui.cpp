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
	static Desk    *glibMainDesk = nullptr;
	static int     glibMouseX, glibMouseY, glibMouseZ, glibMouseB, glibScrWidth, glibScrHeight, glibSCN;
	static GLubyte glibCBack[3] = { 192, 192, 192 },
				   glibCFL[3] = { 235, 235, 235 },
				   glibCFD[3] = { 0, 0, 0 };

	static bool GLib_MouseIn(int x, int y, int w, int h)
	{
		y = glibScrHeight - 1 - y;
		if (glibMouseX >= x && glibMouseX < x + w &&
			glibMouseY >= y && glibMouseY < y + h)
			return true;

		return false;
	}

	static void GLib_DrawFrame(int x, int y, int w, int h, bool p)
	{
		w--;
		h--;

		glBegin(GL_LINES);
		if (p)
			glColor3ubv(glibCFD);
		else
			glColor3ubv(glibCFL);

		glVertex2i(x, y); glVertex2i(x, y - h);
		glVertex2i(x + 1, y); glVertex2i(x + 1, y - h + 1);
		glVertex2i(x, y); glVertex2i(x + w, y);
		glVertex2i(x, y - 1); glVertex2i(x + w - 1, y - 1);

		if (p)
			glColor3ubv(glibCFL);
		else
			glColor3ubv(glibCFD);

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
	button_c::button_c()
	{
		pressed = false;
		glibMainDesk->Register(this);
	}

	button_c::~button_c()
	{
	}

	void button_c::setCaption(const std::string& caption)
	{
		this->caption = caption;
	}

	void button_c::setPosition(int X, int Y, int W, int H)
	{
		x = X;
		y = glibScrHeight - 1 - Y;
		width = W;
		height = H;
	}

	void button_c::check(Desk::EventType &event, DeskControl*& from)
	{
		if (glibMouseZ && GLib_MouseIn(x, y, width, height))
		{
			if (!pressed)
			{
				event = Desk::EventType::Pressed;
				from = this;
				pressed = true;
			}
		}
		else
		{
			if (pressed)
			{
				event = Desk::EventType::Released;
				from = this;
			}

			pressed = false;
		}
	}

	void button_c::draw(const Font& font) const
	{
		int     px, py;

		GLib_DrawFrame(x, y, width, height, pressed);
		px = x + (width >> 1) - (caption.length() << 2) + pressed;
		py = y - (height >> 1) - 7 - pressed;
		font.print(px, py, Color(0), caption);
	}

	/*
	================
	listbox_c
	================
	*/
	listbox_c::listbox_c(bool sb)
	{
		listPos.items = 0;
		listPos.start = 0;
		now = -1;
		glibMainDesk->Register(this);
		scrollBar = sb;
		if (sb)
		{
			slider = new slider_c;
			slider->setNG(glibSCN++, GLIB_SC_GROUP);
			slider->connect(&listPos);
		}
	}

	listbox_c::~listbox_c()
	{
		clear();
	}

	void listbox_c::clear()
	{
		items.clear();
		listPos.items = 0;
		listPos.start = 0;
		now = -1;
	}

	int listbox_c::curItem()
	{
		return now;
	}

	void listbox_c::setCur(int n)
	{
		now = n;
		listPos.start = now - listPos.showCount / 2;
	}

	void listbox_c::delItem(int n)
	{
		if (!listPos.items || n < 0 || n >= listPos.items)
			return;

		items.erase(items.begin() + n);
		listPos.items--;
		if (now >= listPos.items)
			now = listPos.items - 1;
	}

	void listbox_c::addItem(const std::string& item)
	{
		listPos.items++;
		items.push_back(item);
		if (listPos.items == 1)
			now = 0;
	}

	void listbox_c::setPosition(int X, int Y, int W, int H, int fH)
	{
		x = X + 2;
		y = glibScrHeight - 1 - (Y + 2);
		width = W << 3;
		field_height = fH;
		height = H * fH;
		listPos.showCount = H;
		if (scrollBar)
			slider->setPosition(X + (W << 3) + 4, Y, H * fH + 4);
	}

	void listbox_c::check(Desk::EventType &event, DeskControl*& from)
	{
		int     ny = glibScrHeight - 1 - y;

		if (glibMouseZ && GLib_MouseIn(x, y, width, height))
		{
			now = listPos.start + ((glibMouseY - ny) / field_height);
			if (now >= listPos.items)
				now = listPos.items - 1;
			event = Desk::EventType::Change;
			from = this;
		}
	}

	void listbox_c::draw(const Font& font) const
	{
		int     Y, i, shift;

		GLib_DrawFrame(x - 2, y + 2, width + 4, height + 4, true);
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
	switchbox_c
	================
	*/
	switchbox_c::switchbox_c()
	{
		now = -1;

		glibMainDesk->Register(this);

		left = new button_c;
		left->setCaption(" ");
		left->setNG(glibSCN++, GLIB_SC_GROUP);
		right = new button_c;
		right->setCaption(" ");
		right->setNG(glibSCN++, GLIB_SC_GROUP);
	}

	switchbox_c::~switchbox_c()
	{
		clear();
	}

	void switchbox_c::clear()
	{
		items.clear();
		now = -1;
	}

	int switchbox_c::curItem()
	{
		return now;
	}

	void switchbox_c::setCur(int n)
	{
		now = n;
	}

	void switchbox_c::delItem(int n)
	{
		if (n < 0 || n >= (int)items.size())
			return;

		items.erase(items.begin() + n);
		if (now >= (int)items.size())
			now = int(items.size()) - 1;
	}

	void switchbox_c::addItem(const std::string& item)
	{
		items.push_back(item);
		if (items.size() == 1)
		{
			now = 0;
		}
	}

	void switchbox_c::setPosition(int X, int Y, int W, int H)
	{
		left->setPosition(X, Y - 1, 18, 18);
		right->setPosition(X + W - 18, Y - 1, 18, 18);
		width = W;
		x = X;
		y = glibScrHeight - Y;
	}

	void switchbox_c::check(Desk::EventType &event, DeskControl*& from)
	{
		static int  pWait = 0;
		bool        change = false;

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
			event = Desk::EventType::Change;
			from = this;
		}
	}

	void switchbox_c::draw(const Font& font) const
	{
		int     px, py;

		GLib_DrawFrame(x + 20, y, width - 40, 18, true);
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
	label_c::label_c()
	{
		glibMainDesk->Register(this);
	}

	label_c::~label_c()
	{
	}

	void label_c::setPosition(int X, int Y, int W, int H)
	{
		x = X;
		y = glibScrHeight - Y - 1;
		width = W;
		height = H;
	}

	void label_c::setCaption(const std::string& caption)
	{
		text = caption;
	}

	void label_c::draw(const Font& font) const
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

	void label_c::check(Desk::EventType &event, DeskControl*& from)
	{
	}

	/*
	================
	slider_c
	================
	*/
	slider_c::slider_c()
	{
		glibMainDesk->Register(this);
		up = new button_c;
		up->setCaption(" ");
		up->setNG(glibSCN++, GLIB_SC_GROUP);
		down = new button_c;
		down->setCaption(" ");
		down->setNG(glibSCN++, GLIB_SC_GROUP);
		pos = nullptr;
	}

	void slider_c::setPosition(int X, int Y, int H)
	{
		up->setPosition(X, Y, 16, 16);
		down->setPosition(X, Y + H - 16, 16, 16);
		x = X;
		y = glibScrHeight - Y - 17;
		height = H - 32;
	}

	void slider_c::connect(slider_s *to)
	{
		pos = to;
	}

	void slider_c::check(Desk::EventType &event, DeskControl*& from)
	{
		int     h = height, o = pos->start;

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

		if (glibMouseZ && GLib_MouseIn(x, y, 16, height))
		{
			if (pos->items)
			{
				h = pos->showCount * height / pos->items;
				if (h > height)
					h = height;
				if (h < 5)
					h = 5;
			}
			h = glibMouseY - (h >> 1) - (glibScrHeight - y - 1);
			pos->start = h * pos->items / height;
		}

		if (pos->start > pos->items - pos->showCount)
			pos->start = pos->items - pos->showCount;
		if (pos->start < 0)
			pos->start = 0;

		if (pos->start != o)
		{
			event = Desk::EventType::Change;
			from = this;
		}
	}

	void slider_c::draw(const Font& font) const
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
		py = down->getY() - 4 - (down->isPressed() ? 1 : 0);
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
		glColor3ubv(glibCBack);
		glVertex2i(x, y - s);
		glVertex2i(x + 15, y - s);
		glVertex2i(x + 15, y - s - h + 1);
		glVertex2i(x, y - s - h + 1);
		glEnd();

		GLib_DrawFrame(x, y - s, 16, h, false);
	}

	/*
	================
	textbox_c
	================
	*/
	textbox_c::textbox_c()
	{
		glibMainDesk->Register(this);
	}

	textbox_c::~textbox_c()
	{
	}

	void textbox_c::setPosition(int X, int Y, int W, int M, const std::string& allowed)
	{
		x = X + 2;
		y = glibScrHeight - 1 - (Y + 2);
		max = M;
		width = W;

		allowedCharacters = allowed;
		text.clear();
	}

	void textbox_c::keyEvent(SDL_Keycode keyCode)
	{
		if (!text.empty() && keyCode == SDLK_BACKSPACE)
		{
			text.pop_back();
		}
	}

	void textbox_c::textInputEvent(const char* newText)
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

	const std::string& textbox_c::getText() const
	{
		return text;
	}

	void textbox_c::flush()
	{
		text.clear();
	}

	void textbox_c::check(Desk::EventType &event, DeskControl*& from)
	{
		if (glibMouseZ && GLib_MouseIn(x, y, (width + 1) << 3, 18))
		{
			event = Desk::EventType::Active;
			from = this;
		}
	}

	void textbox_c::draw(const Font& font) const
	{
		int     w = (width << 3) + 8;

		GLib_DrawFrame(x - 2, y + 2, w + 4, 22, true);
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
	Desk *Desk::create()
	{
		if (glibMainDesk == nullptr)
			glibMainDesk = new Desk;

		// Left mouse button (used by SDL_BUTTON(x))
		glibMouseB = 1;

		return glibMainDesk;
	}

	Desk::Desk()
	{
		glibSCN = 0;
	}

	Desk::~Desk()
	{
		glibMainDesk = nullptr;

		for (DeskControl* control : controls)
		{
			delete control;
		}
	}

	void Desk::Register(DeskControl *control)
	{
		controls.push_back(control);
	}

	void Desk::screenSize(int SizeX, int SizeY, int tr_x, int tr_y)
	{
		glibScrWidth = SizeX;
		glibScrHeight = SizeY;
		this->tr_x = tr_x;
		this->tr_y = tr_y;
	}

	void Desk::check(EventType &event, DeskControl*& from)
	{
		glibMouseZ = SDL_GetMouseState(&glibMouseX, &glibMouseY) & SDL_BUTTON(glibMouseB);
		glibMouseX -= tr_x;
		glibMouseY -= tr_y;

		event = EventType::None;
		for (DeskControl* control : controls)
		{
			control->check(event, from);
		}
	}

	void Desk::draw(const Font& font)
	{
		glBegin(GL_QUADS);
		glColor3ubv(glibCBack);
		glVertex2i(0, 0);
		glVertex2i(glibScrWidth, 0);
		glVertex2i(glibScrWidth, glibScrHeight);
		glVertex2i(0, glibScrHeight);
		glEnd();

		glPushMatrix();
		glTranslatef((GLfloat)tr_x, (GLfloat)-tr_y, 0);

		for (DeskControl* control : controls)
		{
			control->draw(font);
		}

		glPopMatrix();
	}

	void Desk::keyEvent(SDL_Keycode keyCode)
	{
		for (DeskControl* control : controls)
		{
			control->keyEvent(keyCode);
		}
	}

	void Desk::textInputEvent(const char* text)
	{
		for (DeskControl* control : controls)
		{
			control->textInputEvent(text);
		}
	}
}