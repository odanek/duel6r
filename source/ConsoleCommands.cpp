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


#include <stdlib.h>
#include <SDL2/SDL_mixer.h>
#include "Sound.h"
#include "Util.h"
#include "Math.h"
#include "Menu.h"
#include "Context.h"
#include "TextureManager.h"
#include "Render.h"
#include "Video.h"
#include "Game.h"
#include "Weapon.h"
#include "Globals.h"

namespace Duel6
{
	/*
	==================================================
	Set language - console command
	==================================================
	*/
	void CC_Language(Console& console, const Console::Arguments& args)
	{
		/*
		if (args.length() == 2)
		{
			if (args.get(1) == "lang/czech.lang")
			{
				MY_LangFree();
			}
			MY_LangLoad(args.get(1).c_str());
		}
		else
		{
			console.print(Format("{0}: {1}\n") << D6_L("Language") << D6_L("english"));
		}
		*/
	}

	/*
	==================================================
	Nastaveni poctu kol pres konzoli
	==================================================
	*/
	void CC_MaxRounds(Console& console, const Console::Arguments& args)
	{
		if (args.length() == 2)
		{
			d6Game.setMaxRounds(std::stoi(args.get(1)));
		}
		else
		{
			console.print(Format(D6_L("Played rounds: {0} | Max rounds: {1}\n")) << d6Game.getPlayedRounds() << d6Game.getMaxRounds());
		}
	}
	
	/*
	==================================================
	Nastaveni volume hudby pres konzolu
	==================================================
	*/
	void CC_Volume(Console& console, const Console::Arguments& args)
	{
		if (args.length() == 2)
		{
			Sound::volume(std::stoi(args.get(1)));
		}
	}

	void CC_ToggleRenderMode(Console& console, const Console::Arguments& args)
	{
		Renderer& renderer = d6Game.getRenderer();
		renderer.setWireframe(!renderer.getWireframe());

		if (renderer.getWireframe())
		{
			console.print(D6_L("Rendering mode switched to wireframe\n"));
		}
		else
		{
			console.print(D6_L("Rendering mode switched to solid\n"));
		}
	}

	void CC_ToggleShowFps(Console& console, const Console::Arguments& args)
	{
		Renderer& renderer = d6Game.getRenderer();
		renderer.setShowFps(!renderer.getShowFps());
		
		if (renderer.getShowFps())
		{
			console.print(D6_L("Fps counter shown\n"));
		}
		else
		{
			console.print(D6_L("Fps counter hidden\n"));
		}
	}

	/*
	==================================================
	Zapnuti/vypnuti hudby v menu
	==================================================
	*/
	void CC_MusicOnOff(Console& console, const Console::Arguments& args)
	{
		if (args.length() == 2)
		{
			if (args.get(1) == "on" || args.get(1) == "off")
			{
				d6Menu.enableMusic(args.get(1) == "on");
			}
		}
	}

	/*
	==================================================
	Vyhledani nove pripojenych joypadu
	==================================================
	*/
	void CC_JoyScan(Console& console, const Console::Arguments& args)
	{
		if (d6Menu.isCurrent())
		{
			d6Menu.joyRescan();
		}
	}

	/*
	==================================================
	Nastaveni skinu
	==================================================
	*/
	void CC_LoadSkin(Console& console, const Console::Arguments& args)
	{
		int     i, pl, pos, num, exp16, c;

		if (args.length() == 11)
		{
			pl = std::stoi(args.get(1));
			if (pl >= 0 && pl < (int)d6PlayerColors.size())
			{
				for (i = 0; i < 9; i++)
				{
					pos = args.get(i + 2).length() - 1;
					num = 0;
					exp16 = 1;
					while (pos >= 0)
					{
						c = args.get(i + 2)[pos];
						if (c >= '0' && c <= '9')
							num += (c - '0') * exp16;
						if (c >= 'a' && c <= 'f')
							num += (c - 'a' + 10) * exp16;
						pos--;
						exp16 *= 16;
					}

					Color color((num & 0xff0000) >> 16, (num & 0xff00) >> 8, num & 0xff);
					d6PlayerColors[pl].set((PlayerSkinColors::BodyPart)i, color);
				}

				console.print(Format(D6_L("Skin {0}: OK\n")) << pl);
			}
		}
		if (args.length() == 2)
		{
			pl = std::stoi(args.get(1));
			if (pl >= 0 && pl < (int)d6PlayerColors.size())
			{
				console.print(Format(D6_L("Skin {0}:\n")) << pl);
				for (i = 0; i < 9; i++)
				{
					const Color& color = d6PlayerColors[pl].get((PlayerSkinColors::BodyPart)i);
					console.print(Format("({0}, {1}, {2})\n") << color.getRed() << color.getGreen() << color.getBlue());
				}
			}
		}
	}

	void CC_EnableWeapon(Console& console, const Console::Arguments& args)
	{
		if (args.length() == 3)
		{
			Int32 gn = std::stoi(args.get(1));
			if (gn >= 0 && gn < D6_WEAPONS)
			{
				d6WpnDef[gn].enabled = (args.get(2) == "true");
				std::string enabled = d6WpnDef[gn].enabled ? D6_L("enabled") : D6_L("disabled");
				console.print(Format("\t{0}. {1} {2}\n") << gn << D6_L(d6WpnDef[gn].name) << enabled);
			}
		}
		else
		{
			for (Int32 gn = 0; gn < D6_WEAPONS; gn++)
			{
				std::string enabled = d6WpnDef[gn].enabled ? D6_L("enabled") : D6_L("disabled");
				console.print(Format("\t{0,2}. {1,-13} {2}\n") << gn << D6_L(d6WpnDef[gn].name) << enabled);
			}
		}
	}

	void CC_AmmoRange(Console& console, const Console::Arguments& args)
	{
		if (args.length() == 3)
		{
			int min = std::stoi(args.get(1)), max = std::stoi(args.get(2));

			if (min <= max && min >= 0)
			{
				d6Game.setAmmoRange(std::make_pair(min, max));
			}
		}
		else if (args.length() == 1)
		{
			const std::pair<Int32, Int32>& range = d6Game.getAmmoRange();
			console.print(Format(D6_L("\tmin = {0}\n\tmax = {1}\n")) << range.first << range.second);
		}
		else
		{
			console.print(Format(D6_L("{0}: {0} [min max]\n")) << args.get(0));
		}
	}

	void CC_OpenGLInfo(Console& console, const Console::Arguments& args)
	{
		console.print(D6_L("\n===OpenGL info===\n"));
		console.print(Format(D6_L("Vendor     : {0}\n")) << (const char *)glGetString(GL_VENDOR));
		console.print(Format(D6_L("Renderer   : {0}\n")) << (const char *)glGetString(GL_RENDERER));
		console.print(Format(D6_L("Version    : {0}\n")) << (const char *)glGetString(GL_VERSION));
		console.print(D6_L("Extensions :\n"));

		const char* estr = (const char *)glGetString(GL_EXTENSIONS);

		if (estr == nullptr || strlen(estr) < 2)
		{
			console.print(D6_L("...No supported extensions\n"));
		}
		else
		{
			while (*estr)
			{
				while (*estr == ' ')
				{
					estr++;
				}

				if (*estr == 0)
				{
					break;
				}

				std::string extensionName;
				while (*estr != ' ' && *estr != 0)
				{
					extensionName += *estr;
					++estr;
				}

				console.print(Format("...{0}\n") << extensionName);
			}
		}

		console.print("\n");
	}

	void CC_Register(Console& console)
	{
		SDL_version sdlVersion;
		std::string verStr = D6_L("version");

		// Print application info
		console.print(D6_L("\n===Application information===\n"));
		console.print(Format("{0} {1}: {2}\n") << APP_NAME << verStr << APP_VERSION);
		SDL_GetVersion(&sdlVersion);
		console.print(Format("SDL {0}: {1}.{2}.{3}\n") << verStr << sdlVersion.major << sdlVersion.minor << sdlVersion.patch);
		const SDL_version* mixVersion = Mix_Linked_Version();
		console.print(Format("SDL_mixer {0}: {1}.{2}.{3}\n") << verStr << mixVersion->major << mixVersion->minor << mixVersion->patch);
		console.print(D6_L("Language: english\n"));

		// Set some console functions
		console.setLast(15);
		console.registerCommand("switch_render_mode", &CC_ToggleRenderMode);
		console.registerCommand("show_fps", &CC_ToggleShowFps);
		console.registerCommand("gl_info", &CC_OpenGLInfo);
		console.registerCommand("lang", &CC_Language);
		console.registerCommand("volume", &CC_Volume);
		console.registerCommand("rounds", &CC_MaxRounds);
		console.registerCommand("music", &CC_MusicOnOff);
		console.registerCommand("joy_scan", &CC_JoyScan);
		console.registerCommand("skin", &CC_LoadSkin);
		console.registerCommand("gun", &CC_EnableWeapon);
		console.registerCommand("start_ammo_range", &CC_AmmoRange);		
	}
}