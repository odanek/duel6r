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

#define GLIB_SC_GROUP       0

/*
================
desk_c
================
*/
class DeskControl;

class Desk
{
public:
	enum class ControlType
	{
		Button,
		Label,
		Textbox,
		Listbox,
		Switchbox,
		Slider
	};

	enum class EventType
	{
		None,
		Pressed,
		Change,
		Released,
		Active
	};

private:
    int tr_x; // x translation
    int tr_y; // y translation
    std::vector<DeskControl*> controls;

    Desk();

public:
	~Desk();
    static Desk* Create();
    void ScreenSize(int SizeX, int SizeY, int tr_x, int tr_y);
    void Check(EventType &event, DeskControl*& from);
    void Draw();

	void keyEvent(SDL_Keycode keyCode);
    void textInputEvent(const char* text);

    void Register(DeskControl* control);
};

// Abstract class
class DeskControl
{
protected:
    int x, y, number, group;

public:
    virtual ~DeskControl() 
	{}

    virtual void Check(Desk::EventType &event, DeskControl*& from) = 0;
    virtual void Draw() const = 0;
	virtual Desk::ControlType getType() const = 0;

	virtual void keyEvent(SDL_Keycode keyCode)
	{}

	virtual void textInputEvent(const char* text)
	{}

    void SetNG(int n, int g)
	{ 
		number = n; 
		group = g; 
	}

	int getNumber() const
	{
		return number;
	}

	int getGroup() const
	{
		return group;
	}

	int getX() const
	{
		return x;
	}

	int getY() const
	{
		return y;
	}
};

struct slider_s
{
    int     Items;
    int     Start;
    int     ShowCount;
};

/*
================
button_c
================
*/
class button_c 
	: public DeskControl
{
private:
    int     width, height;
    bool    pressed;
    std::string caption;

public:
            button_c    ();
            ~button_c   ();
    void    SetCaption  (const std::string& caption);
    void    SetPosition (int X, int Y, int W, int H);
    void    Check       (Desk::EventType &event, DeskControl*& from) override;
    void    Draw        () const override;

	bool isPressed() const
	{
		return pressed;
	}

	Desk::ControlType getType() const override
	{
		return Desk::ControlType::Button;
	}
};

/*
================
listbox_c
================
*/
class slider_c;

class listbox_c 
	: public DeskControl
{
private:
    bool        scrollBar;
    slider_c    *slider;
    int         width, height, now, field_height;
    std::vector<std::string> items;
    slider_s    listPos;

public:
            listbox_c   (bool sb);
            ~listbox_c  ();
    void    Check       (Desk::EventType &event, DeskControl*& from) override;
    void    Draw        () const override;
    void    SetPosition (int X, int Y, int W, int H, int fH);
    void    AddItem     (const std::string& item);
    void    DelItem     (int n);
    int     CurItem     ();
    void    SetCur      (int n);
    void    Clear       ();

	Desk::ControlType getType() const override
	{
		return Desk::ControlType::Listbox;
	}
};

/*
================
switchbox_c
================
*/
class switchbox_c 
	: public DeskControl
{
private:
    button_c    *left, *right;
    int         width, now;
    std::vector<std::string> items;

public:
            switchbox_c   ();
            ~switchbox_c  ();
    void    Check       (Desk::EventType &event, DeskControl*& from);
    void    Draw        () const override;
    void    SetPosition (int X, int Y, int W, int H);
    void    AddItem     (const std::string& item);
    void    DelItem     (int n);
    void    SetCur      (int n);
    int     CurItem     ();
    void    Clear       ();

	Desk::ControlType getType() const override
	{
		return Desk::ControlType::Switchbox;
	}
};

/*
================
label_c
================
*/
class label_c 
	: public DeskControl
{
private:
    int     width, height;
    std::string text;

public:
            label_c     ();
            ~label_c    ();
    void    SetCaption  (const std::string& caption);
    void    SetPosition (int X, int Y, int W, int H);
    void    Check       (Desk::EventType &event, DeskControl*& from);
    void    Draw        () const override;

	Desk::ControlType getType() const override
	{
		return Desk::ControlType::Label;
	}
};

/*
================
slider_c
================
*/
class slider_c 
	: public DeskControl
{
private:
    slider_s    *pos;
    button_c    *up, *down;
    int         height, dWait, pWait;

public:
            slider_c    ();
    void    SetPosition (int X, int Y, int H);
    void    Connect     (slider_s *to);
    void    Draw        () const override;
    void    Check       (Desk::EventType &event, DeskControl*& from);

	Desk::ControlType getType() const override
	{
		return Desk::ControlType::Slider;
	}
};

/*
================
textbox_c
================
*/
class textbox_c 
	: public DeskControl
{
private:
    int     max, width;
	std::string text;
	std::string allowedCharacters;

public:
            textbox_c   ();
            ~textbox_c  ();
    void    SetPosition (int X, int Y, int W, int M, const std::string& allowed);
    const std::string& Text() const;
    void    Flush       ();

	void Draw() const override;
    void Check(Desk::EventType &event, DeskControl*& from) override;
    void textInputEvent(const char* text) override;
	void keyEvent(SDL_Keycode keyCode) override;


	Desk::ControlType getType() const override
	{
		return Desk::ControlType::Textbox;
	}
};

#endif
