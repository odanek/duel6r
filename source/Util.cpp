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
	namespace Util
	{
		GLuint loadKH3Texture(const std::string& path, Int32 num, bool clamp)
		{
			myKh3info_s info;
			std::vector<Uint16> pict;
			Uint16 cl;
			std::vector<Uint8> tgaData;

			MY_KH3Open(path.c_str());
			MY_KH3GetInfo(&info);
			Size imgSize = info.sizex * info.sizey;

			pict.resize(imgSize);
			tgaData.resize(imgSize * 4);
			MY_KH3Load(num, &pict[0]);
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

		void saveScreenTga()
		{
			Uint16 tga[9] = { 0, 2, 0, 0, 0, 0, Uint16(g_vid.cl_width), Uint16(g_vid.cl_height), 272 };
			Int32 x, y, r, g, b, num = 0;
			char name[50];
			myFile_s *f;

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

			std::vector<Float32> pixData(3 * g_vid.cl_height * g_vid.cl_width);			
			glReadPixels(0, 0, g_vid.cl_width, g_vid.cl_height, GL_RGB, GL_FLOAT, &pixData[0]);

			auto pixPtr = pixData.begin();
			for (y = 0; y < g_vid.cl_height; y++)
			{
				for (x = 0; x < g_vid.cl_width; x++)
				{
					r = (Int32)(pixPtr[0] * 31);
					g = (Int32)(pixPtr[1] * 31);
					b = (Int32)(pixPtr[2] * 31);
					r = (r << 10) | (g << 5) | b;
					MY_FWrite(&r, 2, 1, f);

					pixPtr += 3;
				}
			}

			MY_FClose(&f);
			g_app.con->printf(MY_L("APP00082|Screenshot ulozen do %s\n"), name);
		}
	}
}