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

#include "project.h"
#include "glib.h"

static desk_c       *glibMainDesk = NULL;
static textbox_c    *glibTextbox;
static int          glibMouseX, glibMouseY, glibMouseZ, glibMouseB,
                    glibScrWidth, glibScrHeight, glibSCN;
static GLubyte      glibCBack[3] = { 192, 192, 192 },
                    glibCFL[3]   = { 235, 235, 235 },
                    glibCFD[3]   = {   0,   0,   0 };

static bool GLib_MouseIn (int x, int y, int w, int h)
{
    y = glibScrHeight - 1 - y;
    if (glibMouseX >= x && glibMouseX < x + w &&
        glibMouseY >= y && glibMouseY < y + h)
        return true;

    return false;
}

static void GLib_DrawFrame (int x, int y, int w, int h, bool p)
{
    w--;
    h--;

    glBegin (GL_LINES);
        if (p)
            glColor3ubv (glibCFD);
        else
            glColor3ubv (glibCFL);

        glVertex2i (x, y); glVertex2i (x, y - h);
        glVertex2i (x + 1, y); glVertex2i (x + 1, y - h + 1);
        glVertex2i (x, y); glVertex2i (x + w, y);
        glVertex2i (x, y - 1); glVertex2i (x + w - 1, y - 1);

        if (p)
            glColor3ubv (glibCFL);
        else
            glColor3ubv (glibCFD);

        glVertex2i (x + w, y - h); glVertex2i (x + w, y);
        glVertex2i (x + w - 1, y - h); glVertex2i (x + w - 1, y - 1);
        glVertex2i (x + w, y - h); glVertex2i (x, y - h);
        glVertex2i (x + w, y - h + 1); glVertex2i (x + 1, y - h + 1);
    glEnd ();
}

/*
================
button_c
================
*/
button_c::button_c (void)
{
    caption = NULL;
    pressed = false;
    glibMainDesk->Register (GLIB_E_BUTTON, this);
}

button_c::~button_c (void)
{
    MY_Free (caption);
}

void button_c::SetCaption (const char *c)
{
    MY_Free (caption);
    caption = (char *) MY_Alloc (strlen (c) + 1);
    strcpy (caption, c);
}

void button_c::SetPosition (int X, int Y, int W, int H)
{
    x = X;
    y = glibScrHeight - 1 - Y;
    width = W;
    height = H;
}

void button_c::Check (int &event, int &from, int &n, int &g)
{
    if (glibMouseZ && GLib_MouseIn (x, y, width, height))
    {
        if (!pressed)
        {
            event = GLIB_E_PRESSED;
            from = GLIB_E_BUTTON;
            n = number;
            g = group;
            pressed = true;
        }
        return;
    }

    if (pressed)
    {
        event = GLIB_E_RELEASED;
        from = GLIB_E_BUTTON;
        n = number;
        g = group;
    }

    pressed = false;
}

void button_c::Draw (void)
{
    int     px, py;

    GLib_DrawFrame (x, y, width, height, pressed);
    px = x + (width >> 1) - (strlen (caption) << 2) + pressed;
    py = y - (height >> 1) - 7 - pressed;
    CO_FontColor (0, 0, 0);
    CO_FontPrint (px, py, caption);
}

/*
================
listbox_c
================
*/
listbox_c::listbox_c (bool sb)
{
    listPos.Items = 0;
    listPos.Start = 0;
    now = -1;
    list = NULL;
    glibMainDesk->Register (GLIB_E_LISTBOX, this);
    scrollBar = sb;
    if (sb)
    {
        slider = new slider_c;
        MY_RegMem (slider, sizeof (slider_c));
        slider->SetNG (glibSCN++, GLIB_SC_GROUP);
        slider->Connect (&listPos);
    }
}

listbox_c::~listbox_c (void)
{
    Clear ();
}

void listbox_c::Clear (void)
{
    int     i;

    if (list != NULL)
    {
        for (i = 0; i < listPos.Items; i++)
            MY_Free (list[i]);
        MY_Free (list);
    }
    listPos.Items = 0;
    listPos.Start = 0;
    now = -1;
}

int listbox_c::CurItem (void)
{
    return now;
}

void listbox_c::SetCur (int n)
{
    now = n;
    listPos.Start = now - listPos.ShowCount / 2;
}

void listbox_c::DelItem (int n)
{
    int     i;
    char    **newL;

    if (!listPos.Items || n < 0 || n >= listPos.Items)
        return;

    if (--listPos.Items == 0)
    {
        MY_Free (list[0]);
        MY_Free (list);
        now = -1;
        return;
    }

    newL = (char **) MY_Alloc (sizeof (char *) * listPos.Items);
    for (i = 0; i < listPos.Items + 1; i++)
        if (i < n)
            newL[i] = list[i];
        else if (i == n)
            MY_Free (list[i]);
        else if (i > n)
            newL[i - 1] = list[i];

    MY_Free (list);
    list = newL;

    if (now >= listPos.Items)
        now = listPos.Items - 1;
}

void listbox_c::AddItem (const char *s)
{
    int     i;
    char    **newL;

    listPos.Items++;
    newL = (char **) MY_Alloc (sizeof (char *) * listPos.Items);
    for (i = 0; i < listPos.Items - 1; i++)
        newL[i] = list[i];
    newL[i] = (char *) MY_Alloc (strlen (s) + 1);
    strcpy (newL[i], s);
    MY_Free (list);
    list = newL;
    if (listPos.Items == 1)
        now = 0;
}

void listbox_c::SetPosition (int X, int Y, int W, int H, int fH)
{
    x = X + 2;
    y = glibScrHeight - 1 - (Y + 2);
    width = W << 3;
    field_height = fH;
    height = H * fH;
    listPos.ShowCount = H;
    if (scrollBar)
        slider->SetPosition (X + (W << 3) + 4, Y, H * fH + 4);
}

void listbox_c::Check (int &event, int &from, int &n, int &g)
{
    int     ny = glibScrHeight - 1 - y;

    if (glibMouseZ && GLib_MouseIn (x, y, width, height))
    {
        now = listPos.Start + ((glibMouseY - ny) / field_height);
        if (now >= listPos.Items)
            now = listPos.Items - 1;
        event = GLIB_E_CHANGE;
        from = GLIB_E_LISTBOX;
        n = number;
        g = group;
    }
}

void listbox_c::Draw (void)
{
    int     Y, i, shift;

    GLib_DrawFrame (x - 2, y + 2, width + 4, height + 4, true);
    glBegin (GL_QUADS);
        glColor3ub (255, 255, 255);
        glVertex2i (x, y);
        glVertex2i (x + width, y);
        glVertex2i (x + width, y - height + 1);
        glVertex2i (x, y - height + 1);
    glEnd ();

    if (list == NULL)
        return;

    Y = y;
    shift = 15 + (field_height - 16) / 2;

    for (i = listPos.Start; i < listPos.Start + listPos.ShowCount; i++, Y -= field_height)
    {
        if (i >= listPos.Items)
            break;
        if (i == now)
        {
            glBegin (GL_QUADS);
                glColor3ub (0, 0, 200);
                glVertex2i (x, Y);
                glVertex2i (x + width - 1, Y);
                glVertex2i (x + width - 1, Y - (field_height - 1));
                glVertex2i (x, Y - (field_height - 1));
            glEnd ();
            CO_FontColor (255, 255, 255);
        }
        else
            CO_FontColor (0, 0, 0);

        CO_FontPrint (x, Y - shift, list[i]);
    }
}

/*
================
switchbox_c
================
*/
switchbox_c::switchbox_c (void)
{
    now = -1;
    list = NULL;
    items = 0;

    glibMainDesk->Register (GLIB_E_SWITCHBOX, this);

    left = new button_c;
    left->SetCaption (" ");
    left->SetNG (glibSCN++, GLIB_SC_GROUP);
    right = new button_c;
    right->SetCaption (" ");
    right->SetNG (glibSCN++, GLIB_SC_GROUP);

    MY_RegMem (left, sizeof (button_c));
    MY_RegMem (right, sizeof (button_c));
}

switchbox_c::~switchbox_c (void)
{
    Clear ();
}

void switchbox_c::Clear (void)
{
    int     i;

    if (list != NULL)
    {
        for (i = 0; i < items; i++)
            MY_Free (list[i]);
        MY_Free (list);
    }
    items = 0;
    now = -1;
}

int switchbox_c::CurItem (void)
{
    return now;
}

void switchbox_c::SetCur (int n)
{
    now = n;
}

void switchbox_c::DelItem (int n)
{
    int     i;
    char    **newL;

    if (items < 0 || n < 0 || n >= items)
        return;

    if (--items == 0)
    {
        MY_Free (list[0]);
        MY_Free (list);
        now = -1;
        return;
    }

    newL = (char **) MY_Alloc (sizeof (char *) * items);
    for (i = 0; i < items + 1; i++)
        if (i < n)
            newL[i] = list[i];
        else if (i == n)
            MY_Free (list[i]);
        else if (i > n)
            newL[i - 1] = list[i];

    MY_Free (list);
    list = newL;

    if (now >= items)
        now = items - 1;
}

void switchbox_c::AddItem (const char *s)
{
    int     i;
    char    **newL;

    items++;
    newL = (char **) MY_Alloc (sizeof (char *) * items);
    for (i = 0; i < items - 1; i++)
        newL[i] = list[i];
    newL[i] = (char *) MY_Alloc (strlen (s) + 1);
    strcpy (newL[i], s);
    MY_Free (list);
    list = newL;
    if (items == 1)
        now = 0;
}

void switchbox_c::SetPosition (int X, int Y, int W, int H)
{
    left->SetPosition (X, Y - 1, 18, 18);
    right->SetPosition (X + W - 18, Y - 1, 18, 18);
    width = W;
    x = X;
    y = glibScrHeight - Y;
}

void switchbox_c::Check (int &event, int &from, int &n, int &g)
{
    static int  pWait = 0;
    bool        change = false;

    if (pWait)
        pWait--;

    if (left->pressed && !pWait)
    {
        pWait = APP_FPS_SPEED * 2;
        if (now > 0)
        {
            now--;
            change = true;
        }
    }

    if (right->pressed && !pWait)
    {
        pWait = APP_FPS_SPEED * 2;
        if (now < items - 1)
        {
            now++;
            change = true;
        }
    }

    if (change)
    {
        event = GLIB_E_CHANGE;
        from = GLIB_E_SLIDER;
        n = number;
        g = group;
    }
}

void switchbox_c::Draw (void)
{
    int     px, py;

    GLib_DrawFrame (x + 20, y, width - 40, 18, true);
    glBegin (GL_QUADS);
        glColor3ub (255, 255, 255);
        glVertex2i (x + 22, y - 1);
        glVertex2i (x + width - 21, y - 1);
        glVertex2i (x + width - 21, y - 16);
        glVertex2i (x + 22, y - 16);
    glEnd ();

    glBegin (GL_TRIANGLES);
        px = left->x + 7 + left->pressed;
        py = left->y - 4 - left->pressed;
        glColor3ub (0, 0, 0);
        glVertex2i (px + 2, py);
        glVertex2i (px + 2, py - 7);
        glVertex2i (px - 2, py - 4);

        px = right->x + 7 + right->pressed;
        py = right->y - 4 - right->pressed;
        glVertex2i (px - 1, py);
        glVertex2i (px - 1, py - 7);
        glVertex2i (px + 3, py - 4);
    glEnd ();

    if (list == NULL)
        return;

    CO_FontPrint (x + 25, y - 15, list[now]);
}

/*
================
label_c
================
*/
label_c::label_c (void)
{
    text = NULL;
    glibMainDesk->Register (GLIB_E_LABEL, this);
}

label_c::~label_c (void)
{
    MY_Free (text);
}

void label_c::SetPosition (int X, int Y, int W, int H)
{
    x = X;
    y = glibScrHeight - Y - 1;
    width = W;
    height = H;
}

void label_c::SetCaption (const char *s)
{
    MY_Free (text);
    text = (char *) MY_Alloc (strlen (s) + 1);
    strcpy (text, s);
}

void label_c::Draw (void)
{
    glBegin (GL_QUADS);
        glColor3ub (170, 170, 170);
        glVertex2i (x, y);
        glVertex2i (x + width - 1, y);
        glVertex2i (x + width - 1, y - height + 1);
        glVertex2i (x, y - height + 1);
    glEnd ();

    CO_FontColor (0, 0, 0);
    CO_FontPrint (x, y - 15, text);
}

void label_c::Check (int &event, int &from, int &n, int &g)
{

}

/*
================
slider_c
================
*/
slider_c::slider_c (void)
{
    glibMainDesk->Register (GLIB_E_SLIDER, this);
    up = new button_c;
    up->SetCaption (" ");
    up->SetNG (glibSCN++, GLIB_SC_GROUP);
    down = new button_c;
    down->SetCaption (" ");
    down->SetNG (glibSCN++, GLIB_SC_GROUP);
    pos = NULL;

    MY_RegMem (up, sizeof (button_c));
    MY_RegMem (down, sizeof (button_c));
}

void slider_c::SetPosition (int X, int Y, int H)
{
    up->SetPosition (X, Y, 16, 16);
    down->SetPosition (X, Y + H - 16, 16, 16);
    x = X;
    y = glibScrHeight - Y - 17;
    height = H - 32;
}

void slider_c::Connect (slider_s *to)
{
    pos = to;
}

void slider_c::Check (int &event, int &from, int &n, int &g)
{
    int     h = height, o = pos->Start;

    if (!up->pressed && !down->pressed)
        pWait = 0;
    else
        if (pWait)
            pWait--;

    if (up->pressed && !pWait)
    {
        pWait = 6;
        pos->Start--;
    }

    if (down->pressed && !pWait)
    {
        pWait = 6;
        pos->Start++;
    }

    if (glibMouseZ && GLib_MouseIn (x, y, 16, height))
    {
        if (pos->Items)
        {
            h = pos->ShowCount * height / pos->Items;
            if (h > height)
                h = height;
            if (h < 5)
                h = 5;
        }
        h = glibMouseY - (h >> 1) - (glibScrHeight - y - 1);
        pos->Start = h * pos->Items / height;
    }

    if (pos->Start > pos->Items - pos->ShowCount)
        pos->Start = pos->Items - pos->ShowCount;
    if (pos->Start < 0)
        pos->Start = 0;

    if (pos->Start != o)
    {
        event = GLIB_E_CHANGE;
        from = GLIB_E_SLIDER;
        n = number;
        g = group;
    }
}

void slider_c::Draw (void)
{
    int px, py, h = height, s = 0;

    px = up->x + 7 + up->pressed;
    py = up->y - 4 - up->pressed;
    glBegin (GL_TRIANGLES);
        glColor3ub (0, 0, 0);
        glVertex2i (px, py);
        glVertex2i (px + 3, py - 6);
        glVertex2i (px - 3, py - 6);
    glEnd ();

    px = down->x + 7 + down->pressed;
    py = down->y - 4 - down->pressed;
    glBegin (GL_TRIANGLES);
        glColor3ub (0, 0, 0);
        glVertex2i (px - 3, py);
        glVertex2i (px + 3, py);
        glVertex2i (px, py - 6);
    glEnd ();

    glBegin (GL_QUADS);
        glColor3ub (230, 230, 230);
        glVertex2i (x, y);
        glVertex2i (x + 15, y);
        glVertex2i (x + 15, y - height + 1);
        glVertex2i (x, y - height + 1);
    glEnd ();

    if (pos->Items)
    {
        h = pos->ShowCount * height / pos->Items;
        if (h > height)
            h = height;
        if (h < 5)
            h = 5;

        s = pos->Start * height / pos->Items;
        if (s > height - h)
            s = height - h;
    }

    glBegin (GL_QUADS);
        glColor3ubv (glibCBack);
        glVertex2i (x, y - s);
        glVertex2i (x + 15, y - s);
        glVertex2i (x + 15, y - s - h + 1);
        glVertex2i (x, y - s - h + 1);
    glEnd ();

    GLib_DrawFrame (x, y - s, 16, h, false);
}

/*
================
textbox_c
================
*/
textbox_c::textbox_c (void)
{
    text = NULL;
    allow = NULL;
    glibMainDesk->Register (GLIB_E_TEXTBOX, this);
    if (glibTextbox == NULL)
        glibTextbox = this;
}

textbox_c::~textbox_c (void)
{
    MY_Free (text);
    MY_Free (allow);
}

void textbox_c::SetPosition (int X, int Y, int W, int M, const char *A)
{
    x = X + 2;
    y = glibScrHeight - 1 - (Y + 2);
    max = M;
    width = W;

    MY_Free (allow);
    allow = MY_StrDup (A);

    MY_Free (text);
    text = (char *) MY_CAlloc (M + 1);
}

void textbox_c::KeyEvent (int e)
{
    char    *a = allow;
    int     l = strlen (text);

    if (e == 8 && l)
    {
        text[l - 1] = 0;
        l--;
    }
    else if (l < max)
        while (*a)
        {
            if ((int) *a == e)
            {
                text[l++] = e;
                text[l] = 0;
            }
            a++;
        }
}

char *textbox_c::Text (void)
{
    return text;
}

void textbox_c::Flush (void)
{
    text[0] = 0;
}

void textbox_c::Check (int &event, int &from, int &n, int &g)
{
    if (glibMouseZ && GLib_MouseIn (x, y, (width + 1) << 3, 18))
    {
        glibTextbox = this;
        event = GLIB_E_ACTIVE;
        from = GLIB_E_TEXTBOX;
        n = number;
        g = group;
    }
}

void textbox_c::Draw (void)
{
    int     w = (width << 3) + 8;

    GLib_DrawFrame (x - 2, y + 2, w + 4, 22, true);
    glBegin (GL_QUADS);
        glColor3ub (255, 255, 255);
        glVertex2i (x, y);
        glVertex2i (x + w, y);
        glVertex2i (x + w, y - 17);
        glVertex2i (x, y - 17);
    glEnd ();

    CO_FontColor (0, 0, 0);
    if (glibTextbox == this)
        CO_FontPrintf (x, y - 16, "%s_", text);
    else
        CO_FontPrintf (x, y - 16, "%s ", text);
}

/*
================
desk_c
================
*/
desk_c *desk_c::Create (void)
{
    if (glibMainDesk == NULL)
        glibMainDesk = new desk_c;

    MY_RegMem (glibMainDesk, sizeof (desk_c));

    // Left mouse button (used by SDL_BUTTON(x))
    glibMouseB = 1;

    return glibMainDesk;
}

desk_c::desk_c (void)
{
    glibTextbox = NULL;
    glibSCN = 0;
    element = NULL;
}

desk_c::~desk_c (void)
{
    desk_s  *s = element, *o;

    glibMainDesk = NULL;
    glibTextbox = NULL;
    while (s != NULL)
    {
        o = s->Next;
        MY_UnregMem (s->Ptr);
        switch (s->Type)
        {
        case GLIB_E_BUTTON:
            delete (button_c *) s->Ptr; break;
        case GLIB_E_LISTBOX:
            delete (listbox_c *) s->Ptr; break;
        case GLIB_E_LABEL:
            delete (label_c *) s->Ptr; break;
        case GLIB_E_SLIDER:
            delete (slider_c *) s->Ptr; break;
        case GLIB_E_TEXTBOX:
            delete (textbox_c *) s->Ptr; break;
        case GLIB_E_SWITCHBOX:
            delete (switchbox_c *) s->Ptr; break;
        }
        MY_Free (s);
        s = o;
    }
}

void desk_c::Register (int what, glibObject_c *p)
{
    desk_s  *d;

    d = (desk_s *) MY_Alloc (sizeof (desk_s));
    d->Type = what;
    d->Ptr = p;
    d->Next = element;
    element = d;
}

void desk_c::ScreenSize (int SizeX, int SizeY, int tr_x, int tr_y)
{
    glibScrWidth = SizeX;
    glibScrHeight = SizeY;
    _tr_x = tr_x;
    _tr_y = tr_y;
}

void desk_c::Check (int &event, int &from, int &n, int &g)
{
    desk_s  *d = element;

    event = GLIB_E_NONE;

    glibMouseZ = SDL_GetMouseState (&glibMouseX, &glibMouseY) & SDL_BUTTON(glibMouseB);

    glibMouseX -= _tr_x;
    glibMouseY -= _tr_y;

    while (d != NULL)
    {
        d->Ptr->Check (event, from, n, g);
        d = d->Next;
    }
}

void desk_c::Draw (void)
{
    desk_s  *d = element;

    glBegin (GL_QUADS);
        glColor3ubv (glibCBack);
        glVertex2i (0, 0);
        glVertex2i (glibScrWidth, 0);
        glVertex2i (glibScrWidth, glibScrHeight);
        glVertex2i (0, glibScrHeight);
    glEnd ();

    glPushMatrix ();
    glTranslatef ((GLfloat)_tr_x, (GLfloat)-_tr_y, 0);

    while (d != NULL)
    {
        d->Ptr->Draw ();
        d = d->Next;
    }

    glPopMatrix ();
}

void desk_c::KeyEvent (int e)
{
    if (glibTextbox != NULL)
        glibTextbox->KeyEvent (e);
}
