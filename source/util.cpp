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

namespace Duel6
{
	GLuint UTIL_LoadKH3Texture(const char *fl, int n, bool clamp)
	{
		myKh3info_s info;
		std::vector<Uint16> pict;
		Uint16 cl;
		std::vector<Uint8> tgaData;

		MY_KH3Open(fl);
		MY_KH3GetInfo(&info);
		Size imgSize = info.sizex * info.sizey;

		pict.resize(imgSize);
		tgaData.resize(imgSize * 4);
		MY_KH3Load(n, &pict[0]);
		MY_KH3Close();

		Size pos = 0;
		for (Size i = 0; i < imgSize; i++)
		{
			cl = pict[i];
			tgaData[pos++] = (((cl >> 11) & 0x1F) * 255) / 31;
			tgaData[pos++] = (((cl >> 5) & 0x3F) * 255) / 63;
			tgaData[pos++] = ((cl & 0x1F) * 255) / 31;
			if (!cl)
				tgaData[pos++] = 0;
			else
				tgaData[pos++] = 255;
		}

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, info.sizex, info.sizey, 0, GL_RGBA, GL_UNSIGNED_BYTE, &tgaData[0]);
		//  gluBuild2DMipmaps(GL_TEXTURE_2D, 4, info.SizeX, info.SizeY, GL_RGBA, GL_UNSIGNED_BYTE, tgaData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Clamp texture coordinates?
		if (clamp)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		}

		return texture;
	}

	void UTIL_OpenGLInfo(con_c *con)
	{
		const char  *e;
		char        *txp = NULL, *tx, tx2[200];
		int         len, i;

		con->printf(MY_L("APP00075|\n===OpenGL info===\n"));
		con->printf(MY_L("APP00076|Vyrobce    : %s\n"), glGetString(GL_VENDOR));
		con->printf(MY_L("APP00077|Renderer   : %s\n"), glGetString(GL_RENDERER));
		con->printf(MY_L("APP00078|Verze      : %s\n"), glGetString(GL_VERSION));
		con->printf(MY_L("APP00079|Extenze    :\n"));

		e = (const char *)glGetString(GL_EXTENSIONS);

		if (strlen(e) < 2)
			con->printf(MY_L("APP00080|...Zadne podporovane extenze\n"));
		else
		{
			txp = (char *)MY_Alloc(strlen(e) + 1);
			strcpy(txp, e);
			tx = txp;

			while (*tx)
			{
				while (*tx == ' ')
					tx++;

				if (*tx == 0)
					break;

				len = 0;

				while (tx[len] != ' ' && tx[len] != 0)
					len++;

				for (i = 0; i < len; i++, tx++)
					tx2[i] = *tx;

				tx2[len] = 0;

				con->printf("...%s\n", tx2);
			}

			MY_Free(txp);
		}

		con->printf("\n");
	}

	/*
	==================================================
	Vytvori a ulozi screenshot
	==================================================
	*/
	void UTIL_SaveScreenTga(int sd)
	{
		Uint16       tga[9] = { 0, 2, 0, 0, 0, 0, Uint16(g_vid.cl_width / sd), Uint16(g_vid.cl_height / sd), 272 };
		int         x, y, r, g, b, num = 0;
		char        name[50];
		float       *pix_data, *pix_ptr;
		myFile_s    *f;

		// Vyhledani cisla pod ktere ukladat
		while (num < 1000)
		{
			num++;
			sprintf(name, "screenshot_%d.tga", num);
			if (MY_FSize(name) <= 0)
				break;
		}

		// Maximalne 1000 screenshotu
		if (num >= 1000)
			return;

		f = MY_FOpen(name, 0, "wb", false);
		if (f == NULL)
		{
			g_app.con->printf(MY_L("APP00081|SaveScreenTga: nelze otevrit soubor %s\n"), name);
			return;
		}

		MY_FWrite(tga, 2, 9, f);

		pix_data = MY_MALLOC(float, 3 * g_vid.cl_height * g_vid.cl_width);
		pix_ptr = pix_data;
		glReadPixels(0, 0, g_vid.cl_width, g_vid.cl_height, GL_RGB, GL_FLOAT, pix_data);

		for (y = 0; y < g_vid.cl_height; y += sd)
			for (x = 0; x < g_vid.cl_width; x += sd)
			{
				r = (int)(*(pix_ptr++) * 31);
				g = (int)(*(pix_ptr++) * 31);
				b = (int)(*(pix_ptr++) * 31);
				r = (r << 10) | (g << 5) | b;
				MY_FWrite(&r, 2, 1, f);
			}

		MY_Free(pix_data);
		MY_FClose(&f);
		g_app.con->printf(MY_L("APP00082|Screenshot ulozen do %s\n"), name);
	}
}