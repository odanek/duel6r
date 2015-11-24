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
#include "DataException.h"
#include "File.h"
#include "Video.h"
#include "Font.h"

namespace Duel6
{
	void Font::load(const std::string& fontFile)
	{
		size_t fileSize = File::getSize(fontFile);

		if (fileSize < 50)
		{
			D6_THROW(DataException, "Font file has unexpected size: " + fontFile);
		}

		data.resize(fileSize - 50);
		File::load(fontFile, 50, &data[0]);
		charWidth = (Int32)data[0];
		charHeight = (Int32)data[1];
	}

	void Font::drawChar(Int32 x, Int32 y, Int32 c) const
	{
		const Uint8* frm = &data[2 + c * charHeight];

		for (Int32 i = 0; i < charHeight; i++, frm++)
		{
			Uint8 b = *frm;

			for (Int32 j = 0; j < 8; j++)
			{
				if (b & (1 << (7 - j)))
				{
					glVertex2i(x + j, y - i);
				}
			}
		}
	}

	void Font::print(Int32 x, Int32 y, const Color& color, const std::string& str) const
	{
		y += charHeight - 1;

		glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
		glBegin(GL_POINTS);

		for (Size i = 0; i < str.length(); i++, x += 8)
		{
			if (str[i] > 0 && str[i] != ' ')
			{
				drawChar(x, y, str[i]);
			}
		}

		glEnd();
	}
}