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

#include <SDL2/SDL_opengl.h>
#include "mylib/mylib.h"
#include "File.h"
#include "Video.h"
#include "Globals.h"
#include "Font.h"

namespace Duel6
{
	void Font::load(const std::string& fontFile)
	{
		size_t fileSize = File::getSize(fontFile);

		if (fileSize < 50)
		{
			MY_Err(MY_ErrDump(MY_L("COSTR0001|Nepodarilo se nahrat soubor s fontem %s"), fontFile.c_str()));
		}

		data.resize(fileSize - 50);
		File::load(fontFile, 50, &data[0]);
		fontSZX = (int)data[0];
		fontSZY = (int)data[1];
	}

	/*
	==================================================
	Nastaveni barvy jakou se bude psat
	==================================================
	*/
	void Font::setColor(const Color& color)
	{
		this->color = color;
	}

	/*
	==================================================
	Vykresleni jednoho znaku
	==================================================
	*/
	void Font::drawChar(Int32 x, Int32 y, Int32 c) const
	{
		const Uint8 *frm;
		Uint16 i, j, b;

		frm = &data[2 + c * fontSZY];

		for (i = 0; i < fontSZY; i++, frm++)
		{
			b = *frm;

			for (j = 0; j < 8; j++)
			{
				if (b & (1 << (7 - j)))
				{
					glVertex2i(x + j, y - i);
				}
			}
		}
	}

	/*
	==================================================
	Vypsani retezce
	==================================================
	*/
	void Font::print(Int32 x, Int32 y, const char* str) const
	{
		Int32 i, l = (Int32)strlen(str);

		if (xMul) x *= fontSZX;
		if (yMul) y *= fontSZY;
		if (yRev) y = d6Video.getScreen().getClientHeight() - y - fontSZY;

		y += fontSZY - 1;

		glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
		glBegin(GL_POINTS);

		for (i = 0; i < l; i++, x += 8)
		{
			if (str[i] > 0 && str[i] != ' ')
			{
				drawChar(x, y, str[i]);
			}
		}

		glEnd();
	}

	/*
	==================================================
	Vypsani retezce
	==================================================
	*/
	void Font::printf(Int32 x, Int32 y, const char *str, ...) const
	{
		static char fontStr[500];
		va_list argptr;

		va_start(argptr, str);
		vsprintf(fontStr, str, argptr);
		va_end(argptr);

		print(x, y, fontStr);
	}

	/*
	==================================================
	Vraci ukazatel na nahrany font
	==================================================
	*/
	const Uint8* Font::get()
	{
		return &data[0];
	}

	/*
	==================================================
	Nastaveni modu - zda se x/y pozice nasobi sirkou/vyskou
	a zda je osa y obracena
	==================================================
	*/
	void Font::setMode(bool xMul, bool yMul, bool yRev)
	{
		this->xMul = xMul;
		this->yMul = yMul;
		this->yRev = yRev;
	}
}