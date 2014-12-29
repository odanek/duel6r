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

#ifndef DUEL6_GUI_H
#define DUEL6_GUI_H

#include <string>
#include <vector>
#include <memory>

namespace Duel6
{
	namespace Gui
	{
		class Control;

		class MouseState
		{
		private:
			Int32 x, y;
			bool buttonPressed;

		public:
			MouseState(Int32 x, Int32 y, bool buttonPressed)
				: x(x), y(y), buttonPressed(buttonPressed)
			{}

			Int32 getX() const
			{
				return x;
			}

			Int32 getY() const
			{
				return y;
			}

			bool isPressed() const
			{
				return buttonPressed;
			}

			bool isInside(Int32 x, Int32 y, Int32 w, Int32 h) const
			{
				return this->x >= x && this->x < x + w && this->y <= y && this->y > y - h;
			}
		};

		class GuiContext
		{
		private:
			MouseState mouseState;

		public:
			GuiContext(const MouseState& mouseState)
				: mouseState(mouseState)
			{}

			const MouseState& getMouseState() const
			{
				return mouseState;
			}
		};

		enum class ControlType
		{
			Button,
			Label,
			Textbox,
			Listbox,
			Switchbox,
			Slider
		};

		class Event
		{
		public:
			typedef std::function<void(const Event&)> Callback;

		public:
			enum class Type
			{
				None,
				Pressed,
				Change,
				Released,
				Active,
				Click
			};

		private:
			Control& sender;
			Type type;

		public:
			Event(Control& sender, Type type)
				: sender(sender), type(type)
			{}

			Control& getSender() const
			{
				return sender;
			}

			Type getType() const
			{
				return type;
			}
		};

		class Desktop
		{
		private:
			Int32 screenWidth;
			Int32 screenHeight;
			Int32 trX; // x translation
			Int32 trY; // y translation
			std::vector<std::unique_ptr<Control>> controls;

		public:
			Desktop();
			~Desktop();

			void screenSize(Int32 scrWidth, Int32 scrHeight, Int32 trX, Int32 trY);
			void update();
			void draw(const Font& font) const;

			void keyEvent(SDL_Keycode keyCode);
			void textInputEvent(const char* text);

			void addControl(Control* control);
		};

		// Abstract base class
		class Control
		{
		protected:
			int x, y;

		public:
			Control(Desktop& desk)
			{
				desk.addControl(this);
			}

			virtual ~Control()
			{
			}

			virtual void check(const GuiContext& context) = 0;
			virtual void draw(const Font& font) const = 0;
			virtual ControlType getType() const = 0;

			virtual void keyEvent(SDL_Keycode keyCode)
			{}

			virtual void textInputEvent(const char* text)
			{}

			int getX() const
			{
				return x;
			}

			int getY() const
			{
				return y;
			}

		protected:
			void drawFrame(Int32 x, Int32 y, Int32 w, Int32 h, bool p) const;
		};

		struct SliderPosition
		{
			int items;
			int start;
			int showCount;
		};

		/*
		================
		Button
		================
		*/
		class Button
			: public Control
		{
		private:
			std::vector<Event::Callback> clickListeners;
			Int32 width, height;
			bool pressed;
			std::string caption;

		public:
			Button(Desktop& desk);
			~Button();
			void setCaption(const std::string& caption);
			void setPosition(int X, int Y, int W, int H);
			void check(const GuiContext& context) override;
			void draw(const Font& font) const override;

			bool isPressed() const
			{
				return pressed;
			}

			ControlType getType() const override
			{
				return ControlType::Button;
			}

			void onClick(Event::Callback listener)
			{
				clickListeners.push_back(listener);
			}
		};

		/*
		================
		Listbox
		================
		*/
		class Slider;

		class Listbox
			: public Control
		{
		private:
			bool scrollBar;
			Slider *slider;
			int width, height, now, field_height;
			std::vector<std::string> items;
			SliderPosition listPos;

		public:
			Listbox(Desktop& desk, bool sb);
			~Listbox();
			void check(const GuiContext& context) override;
			void draw(const Font& font) const override;
			void setPosition(int X, int Y, int W, int H, int fH);
			void addItem(const std::string& item);
			void delItem(int n);
			int curItem();
			void setCur(int n);
			void clear();

			ControlType getType() const override
			{
				return ControlType::Listbox;
			}
		};

		/*
		================
		Combobox
		================
		*/
		class Combobox
			: public Control
		{
		private:
			Button *left, *right;
			Int32 width, now;
			std::vector<std::string> items;

		public:
			Combobox(Desktop& desk);
			~Combobox();
			void check(const GuiContext& context) override;
			void draw(const Font& font) const override;
			void setPosition(int X, int Y, int W, int H);
			void addItem(const std::string& item);
			void delItem(int n);
			void setCur(int n);
			int curItem();
			void clear();

			ControlType getType() const override
			{
				return ControlType::Switchbox;
			}
		};

		/*
		================
		Label
		================
		*/
		class Label
			: public Control
		{
		private:
			Int32 width, height;
			std::string text;

		public:
			Label(Desktop& desk);
			~Label();
			void setCaption(const std::string& caption);
			void setPosition(int X, int Y, int W, int H);
			void check(const GuiContext& context) override;
			void draw(const Font& font) const override;

			ControlType getType() const override
			{
				return ControlType::Label;
			}
		};

		/*
		================
		Slider
		================
		*/
		class Slider
			: public Control
		{
		private:
			SliderPosition *pos;
			Button *up, *down;
			Int32 height, dWait, pWait;

		public:
			Slider(Desktop& desk);
			void setPosition(int X, int Y, int H);
			void connect(SliderPosition *to);
			void draw(const Font& font) const override;
			void check(const GuiContext& context) override;

			ControlType getType() const override
			{
				return ControlType::Slider;
			}
		};

		/*
		================
		Textbox
		================
		*/
		class Textbox
			: public Control
		{
		private:
			Int32 max, width;
			std::string text;
			std::string allowedCharacters;

		public:
			Textbox(Desktop& desk);
			~Textbox();
			void setPosition(int X, int Y, int W, int M, const std::string& allowed);
			const std::string& getText() const;
			void flush();

			void draw(const Font& font) const override;
			void check(const GuiContext& context) override;
			void textInputEvent(const char* text) override;
			void keyEvent(SDL_Keycode keyCode) override;

			ControlType getType() const override
			{
				return ControlType::Textbox;
			}
		};
	}
}

#endif
