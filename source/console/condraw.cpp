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
#include "../Color.h"
#include "../Video.h" // TODO: Remove
#include "console.h"

namespace Duel6 {
    namespace {
        Color conCol[4] = {
                Color(0, 0, 0),          // Font
                Color(238, 221, 221),    // Background top
                Color(238, 221, 0),      // Background bottom
                Color(255, 0, 0)         // Separator
        };
    }

    void Console::renderLine(int y, Size pos, Int32 len, const Font &font) const {
        std::string line;
        for (Int32 i = 0; i < len; i++, pos++) {
            if (pos >= CON_TEXT_SIZE) {
                pos -= CON_TEXT_SIZE;
            }
            line += text[pos];
        }

        font.print(0, y, conCol[0], line);
    }

    void Console::renderHistory(Int32 csY, const Font &font) const {
        unsigned long pl_pos = bufpos, pl_max;
        int pl_disp, pl_y, len, ld_start, ld_num, i;

        pl_disp = 0;
        pl_y = csY - (show + 1) * font.getCharHeight();
        pl_max = buffull ? CON_TEXT_SIZE : bufpos;

        while (pl_max > 0) {
            if (!pl_pos)
                pl_pos = CON_TEXT_SIZE;
            pl_pos--;
            pl_max--;

            // Nalezeni zacatku radku a delky
            len = 0;
            while (pl_max > 0 && text[pl_pos] != '\n') {
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

            for (i = ld_num; i >= 0; i--, ld_start -= width) {
                if (pl_disp++ >= scroll) {
                    pl_y += font.getCharHeight();
                    if (i == ld_num)
                        renderLine(pl_y, ld_start, len - ld_num * width, font);
                    else
                        renderLine(pl_y, ld_start, width, font);
                    if (pl_y >= csY)
                        return;
                }
            }
        }
    }

    void Console::renderBackground(Int32 csX, Int32 csY, const Font &font) const {
        Int32 sizeY = (show + 2) * font.getCharHeight() + 2;
        Int32 bottom = csY - sizeY;

        // TODO: Gradient col[1] -> col[2]
        globRenderer->quadXY(Vector(0, bottom), Vector(csX, sizeY), conCol[2]);
        globRenderer->quadXY(Vector(0, bottom - 3), Vector(csX, 3), conCol[0]);
    }

    void Console::renderSeparator(Int32 csY, const Font &font) const {
        Int32 y = csY - (show + 1) * font.getCharHeight();
        std::string separatorLine(width, scroll ? '^' : '=');
        font.print(0, y, conCol[3], separatorLine);
    }

    void Console::renderInputLine(Int32 csY, const Font &font) const {
        Int32 y = csY - (show + 2) * font.getCharHeight();

        std::string line = (inputscroll > 0) ? "<" : "]";
        line += input.substr((size_t) inputscroll);
        font.print(0, y, conCol[0], line);

        int x = font.getCharWidth() * (curpos - inputscroll + 1);
        std::string cursor = insert ? std::string(1, char(219)) : "_";
        if ((clock() % CLOCKS_PER_SEC) > (CLOCKS_PER_SEC >> 1)) {
            font.print(x, y, conCol[0], cursor);
        }
    }

    /*
    ==================================================
    Render the console content to screen
    ==================================================
    */
    void Console::render(int csX, int csY, const Font &font) {
        if (!visible) {
            return;
        }

        // Reformat console if the width has changed
        if (csX / font.getCharWidth() != width) {
            width = csX / font.getCharWidth();
            if (width < 1) {
                width = 1;
            }
            setInputScroll();
            scroll = 0;
        }

        renderBackground(csX, csY, font);

        // Vypsani poslednich show radku
        if (show > 0) {
            renderHistory(csY, font);
        }

        renderSeparator(csY, font);
        renderInputLine(csY, font);
    }
}