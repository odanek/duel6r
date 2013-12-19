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

#ifndef __GLIB_H
#define __GLIB_H

#define GLIB_SC_GROUP       0

enum
{
    GLIB_E_NONE,
    GLIB_E_SLIDER,
    GLIB_E_LISTBOX,
    GLIB_E_LABEL,
    GLIB_E_BUTTON,
    GLIB_E_TEXTBOX,
    GLIB_E_SWITCHBOX,
    GLIB_E_PRESSED,
    GLIB_E_CHANGE,
    GLIB_E_RELEASED,
    GLIB_E_ACTIVE
};

class slider_c;
class switchbox_c;
class desk_c;

// Abstract class
class glibObject_c
{
protected:
    int     x, y, number, group;

    virtual void    Check   (int &event, int &from, int &n, int &g) = 0;
    virtual void    Draw    (void) = 0;

public:
    virtual ~glibObject_c () {};
    void    SetNG   (int n, int g) { number = n; group = g; }

    friend class desk_c;
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
class button_c : public glibObject_c
{
private:
    int     width, height;
    bool    pressed;
    char    *caption;

public:
            button_c    (void);
            ~button_c   (void);
    void    SetCaption  (const char *c);
    void    SetPosition (int X, int Y, int W, int H);
    void    Check       (int &event, int &from, int &n, int &g);
    void    Draw        (void);

    friend class slider_c;
    friend class switchbox_c;
};

/*
================
listbox_c
================
*/
class listbox_c : public glibObject_c
{
private:
    bool        scrollBar;
    slider_c    *slider;
    int         width, height, now, field_height;
    char        **list;
    slider_s    listPos;

public:
            listbox_c   (bool sb);
            ~listbox_c  (void);
    void    Check       (int &event, int &from, int &n, int &g);
    void    Draw        (void);
    void    SetPosition (int X, int Y, int W, int H, int fH);
    void    AddItem     (const char *s);
    void    DelItem     (int n);
    int     CurItem     (void);
    void    SetCur      (int n);
    void    Clear       (void);
};

/*
================
switchbox_c
================
*/
class switchbox_c : public glibObject_c
{
private:
    button_c    *left, *right;
    int         width, now, items;
    char        **list;

public:
            switchbox_c   (void);
            ~switchbox_c  (void);
    void    Check       (int &event, int &from, int &n, int &g);
    void    Draw        (void);
    void    SetPosition (int X, int Y, int W, int H);
    void    AddItem     (const char *s);
    void    DelItem     (int n);
    void    SetCur      (int n);
    int     CurItem     (void);
    void    Clear       (void);
};

/*
================
label_c
================
*/
class label_c : public glibObject_c
{
private:
    int     width, height;
    char    *text;

public:
            label_c     (void);
            ~label_c    (void);
    void    SetCaption  (const char *s);
    void    SetPosition (int X, int Y, int W, int H);
    void    Check       (int &event, int &from, int &n, int &g);
    void    Draw        (void);
};

/*
================
slider_c
================
*/
class slider_c : public glibObject_c
{
private:
    slider_s    *pos;
    button_c    *up, *down;
    int         height, dWait, pWait;

public:
            slider_c    (void);
    void    SetPosition (int X, int Y, int H);
    void    Connect     (slider_s *to);
    void    Draw        (void);
    void    Check       (int &event, int &from, int &n, int &g);
};

/*
================
textbox_c
================
*/
class textbox_c : public glibObject_c
{
private:
    int     max, width;
    char    *text, *allow;

public:
            textbox_c   (void);
            ~textbox_c  (void);
    void    SetPosition (int X, int Y, int W, int M, const char *A);
    void    KeyEvent    (int e);
    char    *Text       (void);
    void    Flush       (void);
    void    Draw        (void);
    void    Check       (int &event, int &from, int &n, int &g);
};

/*
================
desk_c
================
*/
struct desk_s
{
    glibObject_c    *Ptr;
    int             Type;
    struct desk_s   *Next;
};

class desk_c
{
private:
    int             _tr_x;       // x translation
    int             _tr_y;       // y translation
    desk_s          *element;

    void            Register    (int what, glibObject_c *p);
                    desk_c      (void);
public:
                    ~desk_c     (void);
    static desk_c   *Create     (void);
    void            ScreenSize  (int SizeX, int SizeY, int tr_x, int tr_y);
    void            Check       (int &event, int &from, int &n, int &g);
    void            Draw        (void);
    void            KeyEvent    (int e);

    friend class button_c;
    friend class listbox_c;
    friend class label_c;
    friend class slider_c;
    friend class textbox_c;
    friend class switchbox_c;
};

#endif
