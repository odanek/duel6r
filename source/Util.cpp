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

#include <vector>
#include "mylib/mylib.h"
#include "core/co_core.h"
#include "Util.h"

namespace Duel6
{
	namespace Util
	{
		GLuint createTexture(const Image& image, GLint filtering)
		{
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, 4, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, &image.at(0));

			//  gluBuild2DMipmaps(GL_TEXTURE_2D, 4, info.SizeX, info.SizeY, GL_RGBA, GL_UNSIGNED_BYTE, tgaData);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);

			// Clamp texture coordinates
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			return texture;
		}

		void loadKH3Image(const std::string& path, Int32 num, Image& image)
		{
			myKh3info_s info;
			std::vector<Uint16> pict;
			Uint16 cl;

			MY_KH3Open(path.c_str());
			MY_KH3GetInfo(&info);
			Size imgSize = info.sizex * info.sizey;

			pict.resize(imgSize);
			image.resize(info.sizex, info.sizey);
			MY_KH3Load(num, &pict[0]);
			MY_KH3Close();
			
			for (Size i = 0; i < imgSize; i++)
			{
				cl = pict[i];
				Color& color = image.at(i);
				color.setRed((((cl >> 11) & 0x1F) * 255) / 31);
				color.setGreen((((cl >> 5) & 0x3F) * 255) / 63);
				color.setBlue(((cl & 0x1F) * 255) / 31);
				color.setAlpha(cl == 0 ? 0 : 255);
			}
		}

		static void readTgaColor(FILE *f, Color& color)
		{
			Uint8 colBytes[3];
			fread(colBytes, 1, 3, f);
			color.set(colBytes[2], colBytes[1], colBytes[0], (colBytes[0] == 0 && colBytes[1] == 0 && colBytes[2] == 0) ? 0 : 255);
		}

		static void writeTgaColor(FILE *f, const Color& color)
		{
			Uint8 colBytes[3] = { color.getBlue(), color.getGreen(), color.getRed() };
			fwrite(colBytes, 1, 3, f);
		}

		void loadTargaImage(const std::string& path, Image& image)
		{
			FILE *f;
			f = fopen(path.c_str() , "rb");
			if (f == nullptr)
			{
				MY_Err(MY_ErrDump(MY_L("APPxxxxx|loadTarga: nelze otevrit soubor %s\n"), path.c_str()));
			}

			// Header
			Uint16 header[9];
			fread(header, 2, 9, f);

			// Data
			image.resize(header[6], header[7]);
			Color color, *output = &image.at(0);
			Size remainingData = header[6] * header[7];
			Uint8 chunkLength, packet;

			while (remainingData > 0)
			{
				fread(&packet, 1, 1, f);

				if (packet >= 0x80) 
				{
					// RLE chunk
					chunkLength = packet - 0x7f;
										
					readTgaColor(f, color);
					for (Size i = 0; i < chunkLength; ++i)
					{
						*output++ = color;
					}
				}
				else
				{
					// Raw chunk
					chunkLength = packet + 1;

					for (Size i = 0; i < chunkLength; ++i)
					{
						readTgaColor(f, color);
						*output++ = color;
					}					
				}

				remainingData -= chunkLength;
			}

			fclose(f);
		}

		void saveTarga(const std::string& path, const Image& image)
		{
			FILE *f;
			f = fopen(path.c_str() , "wb");
			if (f == nullptr)
			{
				g_app.con->printf(MY_L("APP00081|saveTarga: nelze otevrit soubor %s\n"), path.c_str());
				return;
			}

			// Header
			Uint16 header[9] = { 0, 10, 0, 0, 0, 0, image.getWidth(), image.getHeight(), 0x18 };  // 0x2018
			fwrite(header, 2, 9, f);

			// Data
			Size remainingData = image.getWidth() * image.getHeight();
			const Color* data = &image.at(0);
			Uint8 chunkLength;
			while (remainingData > 0)
			{
				if (remainingData > 1 && data[0] == data[1]) 
				{
					// RLE chunk
					chunkLength = 2;
					while (chunkLength < 128 && chunkLength < remainingData && data[0] == data[chunkLength])
					{
						++chunkLength;
					}

					Uint8 packet = 0x80 + (chunkLength - 1);
					fwrite(&packet, 1, 1, f);
					writeTgaColor(f, data[0]);
				}
				else
				{
					// Raw chunk
					chunkLength = 1;
					while (chunkLength < 128 && chunkLength < remainingData && data[chunkLength - 1] != data[chunkLength])
					{
						++chunkLength;
					}

					Uint8 packet = chunkLength - 1;
					fwrite(&packet, 1, 1, f);
					for (Size i = 0; i < chunkLength; ++i)
					{
						writeTgaColor(f, data[i]);
					}
				}

				data += chunkLength;
				remainingData -= chunkLength;
			}

			fclose(f);
		}

		void saveScreenTga()
		{
			Int32 num = 0;
			char name[50];			

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

			Image image(g_vid.cl_width, g_vid.cl_height);			
			glReadPixels(0, 0, g_vid.cl_width, g_vid.cl_height, GL_RGBA, GL_UNSIGNED_BYTE, &image.at(0));
			saveTarga(name, image);

			g_app.con->printf(MY_L("APP00082|Screenshot ulozen do %s\n"), name);
		}
	}
}