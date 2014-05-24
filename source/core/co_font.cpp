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

/*
Projekt: Sablona aplikace
Popis: Prace s fonty, psani na obrazovku
*/

#include <vector>
#include "co_core.h"

static  std::vector<myBYTE> fontData;
static  int     fontSZX, fontSZY;
static  GLubyte fontCol[3];
static  char    fontStr[500];
static  bool    font_x_mul = false;         // Nasobit x sirkou fontu?
static  bool    font_y_mul = false;         // Nasobit y sirkou fontu?
static  bool    font_y_rev = false;         // Otocit osu y?

/*
==================================================
Nahrani fontu ze souboru
==================================================
*/
void CO_FontLoad (const std::string& fontFile)
{
	size_t fileSize = MY_FSize(fontFile.c_str());

	if (fileSize < 50)
	{
		MY_Err(MY_ErrDump(MY_L("COSTR0001|Nepodarilo se nahrat soubor s fontem %s"), fontFile));
	}

    fontData.resize(fileSize - 50);
    MY_FLoadBlock (fontFile.c_str(), 50, -1, &fontData[0]);
    fontSZX = (int) fontData[0];
    fontSZY = (int) fontData[1];
}

/*
==================================================
Nastaveni barvy jakou se bude psat
==================================================
*/
void CO_FontColor (GLubyte red, GLubyte green, GLubyte blue)
{
    fontCol[0] = red;
    fontCol[1] = green;
    fontCol[2] = blue;
}

/*
==================================================
Vykresleni jednoho znaku
==================================================
*/
static void CO_FontDrawChar (int x, int y, int c)
{
    myBYTE  *frm;
    myWORD  i, j, b;

    frm = &fontData[2 + c * fontSZY];

    for (i = 0; i < fontSZY; i++, frm++)
    {
        b = *frm;

        for (j = 0; j < 8; j++)
            if (b & (1 << (7 - j)))
                glVertex2i (x + j, y - i);
    }
}

/*
==================================================
Vypsani retezce
==================================================
*/
void CO_FontPrint (int x, int y, const char *str)
{
    int     i, l = (int) strlen (str);

    if (font_x_mul) x *= fontSZX;
    if (font_y_mul) y *= fontSZY;
    if (font_y_rev) y = g_vid.cl_height - y - fontSZY;

    y += fontSZY - 1;

    glColor3ubv (fontCol);
    glBegin (GL_POINTS);

    for (i = 0; i < l; i++, x += 8)
        if (str[i] > 0 && str[i] != ' ')
            CO_FontDrawChar (x, y, str[i]);

    glEnd ();
}

/*
==================================================
Vypsani retezce
==================================================
*/
void CO_FontPrintf (int x, int y, const char *str, ...)
{
    va_list argptr;

    va_start (argptr, str);
    vsprintf (fontStr, str, argptr);
    va_end (argptr);

    CO_FontPrint (x, y, fontStr);
}

/*
==================================================
Vraci ukazatel na nahrany font
==================================================
*/
const myBYTE *CO_FontGet ()
{
    return &fontData[0];
}

/*
==================================================
Nastaveni modu - zda se x/y pozice nasobi sirkou/vyskou
a zda je osa y obracena
==================================================
*/
void CO_FontSetMode (bool x_mul, bool y_mul, bool y_rev)
{
    font_x_mul = x_mul;
    font_y_mul = y_mul;
    font_y_rev = y_rev;
}
