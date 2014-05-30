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
void con_c::drawchar(int x, int y, int c) const
{
    const conBYTE   *frm;
    conWORD         i, j, b;

    if (c == ' ' || !c)
        return;

    frm = &m_font[2 + c * CON_FSY];

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
Vypsani poslednich m_show radek - bere do uvahy scrolling
a rotace bufferu. Varovani: HARDCORE ALGORITMUS!!! :-)
==================================================
*/
void con_c::dprint_line(int y, unsigned long pos, int len) const
{
    int     i, x = 0;

    for (i = 0; i < len; i++, x+= CON_FSX)
    {
        if (pos >= CON_TEXT_SIZE)
            pos -= CON_TEXT_SIZE;
        drawchar(x, y, (int)m_text[pos++]);
    }
}

void con_c::dshow_hist(int res_y) const
{
    unsigned long   pl_pos = m_bufpos, pl_max;
    int             pl_disp, pl_y, len, ld_start, ld_num, i;

    pl_disp = 0;
    pl_y = res_y - m_show * CON_FSY;
    pl_max = m_buffull ? CON_TEXT_SIZE : m_bufpos;

    while (pl_max > 0)
    {
        if (!pl_pos)
            pl_pos = CON_TEXT_SIZE;
        pl_pos--;
        pl_max--;

        // Nalezeni zacatku radku a delky
        len = 0;
        while (pl_max > 0 && m_text[pl_pos] != '\n')
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

        // Vypsani radku po fragmentech sirky m_width od konce
        ld_num = (len - 1) / m_width;
        ld_start = pl_pos + 1 + ld_num * m_width;

        for (i = ld_num; i >= 0; i--, ld_start -= m_width)
            if (pl_disp++ >= m_scroll)
            {
                pl_y += CON_FSY;
                if (i == ld_num)
                    dprint_line (pl_y, ld_start, len - ld_num * m_width);
                else
                    dprint_line (pl_y, ld_start, m_width);
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
void con_c::blit(int res_x, int res_y)
{
    long    sizeY;
    char    cursor;
    int     i, d, x, y;

    if (m_font == NULL || !m_visible)
        return;

    // Preformatuje konzoli pokud se zmenila sirka
    if (res_x / CON_FSX != m_width)
    {
        m_width = res_x / CON_FSX;
        if (m_width < 1)
            m_width = 1;
        setinputscroll ();
        m_scroll = 0;
    }

    // Vycisteni transformacni matice
    glLoadIdentity ();

    // Vykresleni pozadi
    sizeY = (m_show + 2) * CON_FSY + 2;

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

    // Vypsani poslednich m_show radku
	if (m_show > 0)
	{
		dshow_hist(res_y);
	}

    // Vypsani oddelovace
    x = 0;
    y = res_y - m_show * CON_FSY;

    if (m_scroll)
        d = '^';
    else
        d = '=';

    glColor3ub (255, 0, 0);
	for (i = 0; i < m_width; i++, x += CON_FSX)
	{
		drawchar(x, y, d);
	}
    glColor3ub (conCol[0][0], conCol[0][1], conCol[0][2]);

    // Vypsani vstupniho radku
    x = CON_FSX;
    y -= CON_FSY;
    d = ((int)m_input.length()) - m_inputscroll;

	if (m_inputscroll)
	{
		drawchar(0, y, '<');
	}
	else
	{
		drawchar(0, y, ']');
	}

	for (i = 0; i < d; i++, x += CON_FSX)
	{
		drawchar(x, y, (conBYTE)m_input[m_inputscroll + i]);
	}

    x = CON_FSX * (m_curpos - m_inputscroll + 1);
    cursor = m_insert ? 219 : '_';
	if ((clock() % CLOCKS_PER_SEC) > (CLOCKS_PER_SEC >> 1))
	{
		drawchar(x, y, cursor);
	}

    glEnd ();
}
