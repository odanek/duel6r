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
#include "PlayerSkin.h"

namespace Duel6
{
	PlayerSkin::PlayerSkin(const PlayerSkinColors& colors)
		: colors(colors)
	{
		load(D6_FILE_PLAYER);
	}

	PlayerSkin::~PlayerSkin()
	{
		if (!textures.empty())
		{
			glDeleteTextures(textures.size(), &textures[0]);
			textures.clear();
		}
	}

	void PlayerSkin::load(const std::string& fileName)
	{
		g_app.con->printf(MY_L("APP00049|Inicializace hrace - nahravam textury\n"));

		if (MY_KH3Open(fileName.c_str()) != MY_OK)
			MY_Err(MY_ErrDump(MY_L("APP00091|Nepodarilo se otevrit soubor %s s texturami postav"), fileName.c_str()));

		myKh3info_s ki;
		MY_KH3GetInfo(&ki);
		g_app.con->printf(MY_L("APP00050|...Soubor %s obsahuje %d textur\n"), fileName.c_str(), ki.picts);

		textures.resize(ki.picts);
		
		Size imgSize = ki.sizex * ki.sizey;
		Uint16* hcData = (Uint16 *)MY_Alloc(imgSize << 1);
		Uint8* tcData = (Uint8 *)MY_Alloc(imgSize << 2);

		glGenTextures(textures.size(), &textures[0]);

		for (Size i = 0; i < textures.size(); i++)
		{
			Color color;
			Size pos = 0;
			MY_KH3Load(i, hcData);

			for (Size j = 0; j < imgSize; j++)
			{
				// Get color from skin
				bool isBodyPart = true;
				PlayerSkinColors::BodyPart bodyPart;

				switch (hcData[j])
				{
				case 65504: bodyPart = PlayerSkinColors::HairTop; break;
				case 57024: bodyPart = PlayerSkinColors::HairBottom; break;
				case 21: bodyPart = PlayerSkinColors::BodyOuter; break;
				case 31: bodyPart = PlayerSkinColors::BodyInner; break;
				case 1440: bodyPart = PlayerSkinColors::HandOuter; break;
				case 2016: bodyPart = PlayerSkinColors::HandInner; break;
				case 34816: bodyPart = PlayerSkinColors::Trousers; break;
				case 46496: bodyPart = PlayerSkinColors::Shoes; break;
				case 64661: bodyPart = PlayerSkinColors::Face; break;
				default: isBodyPart = false;
				}

				if (!isBodyPart)
				{
					color.setRed((((hcData[j] >> 11) & 0x1F) * 255) / 31);
					color.setGreen((((hcData[j] >> 5) & 0x3F) * 255) / 63);
					color.setBlue(((hcData[j] & 0x1F) * 255) / 31);
				}
				else
				{
					color = colors.get(bodyPart);
				}

				tcData[pos++] = color.getRed();
				tcData[pos++] = color.getGreen();
				tcData[pos++] = color.getBlue();
				tcData[pos++] = (!hcData[j]) ? 0 : 0xff; // Alpha mask, black pixels are transparent
			}

			glBindTexture(GL_TEXTURE_2D, textures[i]);
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
}