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
Projekt: Konzole
Popis: Vykresleni konzoly
*/

#include <time.h>
#include <string.h>
#include <SDL2/SDL_opengl.h>
#include "console.h"

static GLubyte  conCol[3][3] =
{
    { 0x00, 0x00, 0x00 },   // Font
    { 0xee, 0xdd, 0xdd },   // Background Up
    { 0xee, 0xdd, 0x00 }    // Background Down
};

/*
==================================================
Vykresleni jednoho znaku
==================================================
*/
void Console::drawChar(int x, int y, int c) const
{
    const conBYTE   *frm;
    conWORD         i, j, b;

    if (c == ' ' || !c)
        return;

    frm = &font[2 + c * CON_FSY];

    for (i = 0; i < CON_FSY; i++, frm++)
    {
        b = *frm;

        for (j = 0; j < CON_FSX; j++)
            if (b & (1 << (7 - j)))
                glVertex2i (x + j, y - i);
    }
}

/*
==================================================
Vypsani poslednich show radek - bere do uvahy scrolling
a rotace bufferu. Varovani: HARDCORE ALGORITMUS!!! :-)
==================================================
*/
void Console::dprintLine(int y, unsigned long pos, int len) const
{
    int     i, x = 0;

    for (i = 0; i < len; i++, x+= CON_FSX)
    {
        if (pos >= CON_TEXT_SIZE)
            pos -= CON_TEXT_SIZE;
        drawChar(x, y, (int)text[pos++]);
    }
}

void Console::dshowHist(int res_y) const
{
    unsigned long   pl_pos = bufpos, pl_max;
    int             pl_disp, pl_y, len, ld_start, ld_num, i;

    pl_disp = 0;
    pl_y = res_y - show * CON_FSY;
    pl_max = buffull ? CON_TEXT_SIZE : bufpos;

    while (pl_max > 0)
    {
        if (!pl_pos)
            pl_pos = CON_TEXT_SIZE;
        pl_pos--;
        pl_max--;

        // Nalezeni zacatku radku a delky
        len = 0;
        while (pl_max > 0 && text[pl_pos] != '\n')
        {
            if (!pl_pos)
                pl_pos = CON_TEXT_SIZE;
            pl_pos--;
            pl_max--;
            len++;
        }

        // Ignorovani pripadneho posledniho znaku '\n'
        if (!len && !pl_disp)
            continue;

        // Vypsani radku po fragmentech sirky width od konce
        ld_num = (len - 1) / width;
        ld_start = pl_pos + 1 + ld_num * width;

        for (i = ld_num; i >= 0; i--, ld_start -= width)
            if (pl_disp++ >= scroll)
            {
                pl_y += CON_FSY;
                if (i == ld_num)
                    dprintLine (pl_y, ld_start, len - ld_num * width);
                else
                    dprintLine (pl_y, ld_start, width);
                if (pl_y >= res_y)
                    return;
            }
    }
}

/*
==================================================
Vykresleni konzole na obrazovku
==================================================
*/
void Console::blit(int res_x, int res_y)
{
    long    sizeY;
    char    cursor;
    int     i, d, x, y;

    if (font == NULL || !visible)
        return;

    // Preformatuje konzoli pokud se zmenila sirka
    if (res_x / CON_FSX != width)
    {
        width = res_x / CON_FSX;
        if (width < 1)
            width = 1;
        setInputScroll ();
        scroll = 0;
    }

    // Vycisteni transformacni matice
    glLoadIdentity ();

    // Vykresleni pozadi
    sizeY = (show + 2) * CON_FSY + 2;

    glBegin (GL_QUADS);
    glColor4ub (conCol[1][0], conCol[1][1], conCol[1][2], 125);
    glVertex2i (0, res_y);
    glVertex2i (res_x, res_y);
    glColor4ub (conCol[2][0], conCol[2][1], conCol[2][2], 125);
    glVertex2i (res_x, res_y - sizeY);
    glVertex2i (0, res_y - sizeY);
    glColor4ub (conCol[0][0], conCol[0][1], conCol[0][2], 125);
    glVertex2i (0, res_y - sizeY);
    glVertex2i (res_x, res_y - sizeY);
    glVertex2i (res_x, res_y - sizeY - 3);
    glVertex2i (0, res_y - sizeY - 3);
    glEnd ();

    // Vykresleni obsahu
    glColor3ub (conCol[0][0], conCol[0][1], conCol[0][2]);
    glBegin (GL_POINTS);

    // Vypsani poslednich show radku
	if (show > 0)
	{
		dshowHist(res_y);
	}

    // Vypsani oddelovace
    x = 0;
    y = res_y - show * CON_FSY;

    if (scroll)
        d = '^';
    else
        d = '=';

    glColor3ub (255, 0, 0);
	for (i = 0; i < width; i++, x += CON_FSX)
	{
		drawChar(x, y, d);
	}
    glColor3ub (conCol[0][0], conCol[0][1], conCol[0][2]);

    // Vypsani vstupniho radku
    x = CON_FSX;
    y -= CON_FSY;
    d = ((int)input.length()) - inputscroll;

	if (inputscroll)
	{
		drawChar(0, y, '<');
	}
	else
	{
		drawChar(0, y, ']');
	}

	for (i = 0; i < d; i++, x += CON_FSX)
	{
		drawChar(x, y, (conBYTE)input[inputscroll + i]);
	}

    x = CON_FSX * (curpos - inputscroll + 1);
    cursor = insert ? 219 : '_';
	if ((clock() % CLOCKS_PER_SEC) > (CLOCKS_PER_SEC >> 1))
	{
		drawChar(x, y, cursor);
	}

    glEnd ();
}
