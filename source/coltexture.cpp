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
#include "coltexture.h"

namespace Duel6
{
	/*
	==================================================
	Inicializace
	Vytvori textury hrace tvorene jen jednou barvou
	==================================================
	*/
	void ColorTexture::Init(myBYTE red, myBYTE green, myBYTE blue)
	{
		myKh3info_s     ki;
		myWORD          *hcData;
		myBYTE          *tcData;
		int             i, j, pos, img_size;

		g_app.con->printf(MY_L("APP00111|...Inicializace barvenych textur (%d, %d, %d)\n"), red, green, blue);

		if (MY_KH3Open(D6_FILE_PLAYER) != MY_OK)
			MY_Err(MY_ErrDump(MY_L("APP00091|Nepodarilo se otevrit soubor %s s texturami postav"), D6_FILE_PLAYER));

		MY_KH3GetInfo(&ki);
		g_app.con->printf(MY_L("APP00050|...Soubor %s obsahuje %d textur\n"), D6_FILE_PLAYER, ki.picts);

		m_textures = ki.picts;
		m_glTexture = D6_MALLOC(myUINT, m_textures);
		img_size = ki.sizex * ki.sizey;
		hcData = (myWORD *)MY_Alloc(img_size << 1);
		tcData = (myBYTE *)MY_Alloc(img_size << 2);

		glGenTextures(m_textures, m_glTexture);

		for (i = 0; i < m_textures; i++)
		{
			MY_KH3Load(i, hcData);
			pos = 0;

			for (j = 0; j < img_size; j++)
			{
				tcData[pos++] = hcData[j] ? red : 0;
				tcData[pos++] = hcData[j] ? green : 0;
				tcData[pos++] = hcData[j] ? blue : 0;
				tcData[pos++] = (!hcData[j]) ? 0 : 0xff;
			}

			glBindTexture(GL_TEXTURE_2D, m_glTexture[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, 4, ki.sizex, ki.sizey, 0, GL_RGBA, GL_UNSIGNED_BYTE, tcData);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		}

		MY_Free(hcData);
		MY_Free(tcData);
		MY_KH3Close();
	}

	/*
	==================================================
	DeInit
	Uvolni naalokovane textury
	==================================================
	*/
	void ColorTexture::DeInit(void)
	{
		if (IsInited())
		{
			glDeleteTextures(m_textures, m_glTexture);
			MY_Free(m_glTexture);
		}
	}
}