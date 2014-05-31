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

#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL_mixer.h>
#include "Sound.h"
#include "Weapon.h"
#include "Util.h"
#include "ElevatorList.h"
#include "Math.h"
#include "Menu.h"
#include "Context.h"
#include "TextureManager.h"
#include "Render.h"
#include "Fire.h"
#include "Video.h"
#include "Globals.h"

namespace Duel6
{
	/*
	==================================================
	Set language - console command
	==================================================
	*/
	void SET_Language(Console& console, const Console::Arguments& args)
	{
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
			console.printf("%s: %s\n", MY_L("APP00070|Jazyk"), MY_L("APP00071|cestina"));
		}
	}

	/*
	==================================================
	Nastaveni poctu kol pres konzoli
	==================================================
	*/
	void SET_MaxRounds(Console& console, const Console::Arguments& args)
	{
		if (args.length() == 2)
		{
			d6Game.setMaxRounds(atoi(args.get(1).c_str()));
		}
		else
		{
			console.printf("Odehranych kol: %d | Max pocet kol: %d\n", d6Game.getPlayedRounds(), d6Game.getMaxRounds());
		}
	}
	
	/*
	==================================================
	Nastaveni volume hudby pres konzolu
	==================================================
	*/
	void SET_Volume(Console& console, const Console::Arguments& args)
	{
		if (args.length() == 2)
		{
			Sound::volume(atoi(args.get(1).c_str()));
		}
	}

	void SET_ToggleRenderMode(Console& console, const Console::Arguments& args)
	{
		d6Wireframe = !d6Wireframe;
		if (d6Wireframe)
		{
			console.printf(MY_L("APP00020|Vykreslovaci mod prepnut na dratovy\n"));
		}
		else
		{
			console.printf(MY_L("APP00021|Vykreslovaci mod prepnut na solidni\n"));
		}
	}

	void SET_ToggleShowFps(Console& console, const Console::Arguments& args)
	{
		d6ShowFps = !d6ShowFps;
		if (d6ShowFps)
		{
			console.printf(MY_L("APP00022|Ukazatel fps zapnut\n"));
		}
		else
		{
			console.printf(MY_L("APP00023|Ukazatel fps vypnut\n"));
		}
	}

	/*
	==================================================
	Zapnuti/vypnuti hudby v menu
	==================================================
	*/
	void SET_MusicOnOff(Console& console, const Console::Arguments& args)
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
	void SET_JoyScan(Console& console, const Console::Arguments& args)
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
	void SET_LoadSkin(Console& console, const Console::Arguments& args)
	{
		int     i, pl, pos, num, exp16, c;

		if (args.length() == 11)
		{
			pl = atoi(args.get(1).c_str());
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

				console.printf("Skin %d: OK\n", pl);
			}
		}
		if (args.length() == 2)
		{
			pl = atoi(args.get(1).c_str());
			if (pl >= 0 && pl < (int)d6PlayerColors.size())
			{
				console.printf("Skin %d: ", pl);
				for (i = 0; i < 9; i++)
				{
					const Color& color = d6PlayerColors[pl].get((PlayerSkinColors::BodyPart)i);
					console.printf("%02x%02x%02x ", color.getRed(), color.getGreen(), color.getBlue());
				}
				console.printf("\n");
			}
		}
	}

	/*
	==================================================
	Povoleni/zakazani zbrane
	==================================================
	*/
	void SET_EnableWeapon(Console& console, const Console::Arguments& args)
	{
		int gn;

		if (args.length() == 3)
		{
			gn = atoi(args.get(1).c_str());
			if (gn >= 0 && gn < D6_WEAPONS)
			{
				d6WpnDef[gn].enabled = (args.get(2) == "true");
				if (d6WpnDef[gn].enabled)
					console.printf("\t%02d. %-13s %s\n", gn, MY_L(d6WpnDef[gn].name), MY_L("APP00113|povoleno"));
				else
					console.printf("\t%02d. %-13s %s\n", gn, MY_L(d6WpnDef[gn].name), MY_L("APP00114|zakazano"));
			}
		}
		else
		{
			for (gn = 0; gn < D6_WEAPONS; gn++)
			{
				if (d6WpnDef[gn].enabled)
					console.printf("\t%02d. %-13s %s\n", gn, MY_L(d6WpnDef[gn].name), MY_L("APP00113|povoleno"));
				else
					console.printf("\t%02d. %-13s %s\n", gn, MY_L(d6WpnDef[gn].name), MY_L("APP00114|zakazano"));
			}
		}
	}

	/*
	==================================================
	Nastaveni rozsahu poctu naboju po startu hry
	==================================================
	*/
	void SET_AmmoRange(Console& console, const Console::Arguments& args)
	{
		if (args.length() == 3)
		{
			int min = atoi(args.get(1).c_str()), max = atoi(args.get(2).c_str());

			if (min <= max && min >= 0)
			{
				d6Game.setAmmoRange(std::make_pair(min, max));
			}
		}
		else if (args.length() == 1)
		{
			const std::pair<Int32, Int32>& range = d6Game.getAmmoRange();
			console.printf("\tmin = %d\n\tmax = %d\n", range.first, range.second);
		}
		else
		{
			console.printf("%s: %s [min max]\n", args.get(0).c_str(), args.get(0).c_str());
		}
	}

	/** OpenGL info */
	void SET_OpenGLInfo(Console& console, const Console::Arguments& args)
	{
		const char *e;
		char *txp = nullptr, *tx, tx2[200];
		int len, i;

		console.printf(MY_L("APP00075|\n===OpenGL info===\n"));
		console.printf(MY_L("APP00076|Vyrobce    : %s\n"), glGetString(GL_VENDOR));
		console.printf(MY_L("APP00077|Renderer   : %s\n"), glGetString(GL_RENDERER));
		console.printf(MY_L("APP00078|Verze      : %s\n"), glGetString(GL_VERSION));
		console.printf(MY_L("APP00079|Extenze    :\n"));

		e = (const char *)glGetString(GL_EXTENSIONS);

		if (e == nullptr || strlen(e) < 2)
		{
			console.printf(MY_L("APP00080|...Zadne podporovane extenze\n"));
		}
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

				console.printf("...%s\n", tx2);
			}

			MY_Free(txp);
		}

		console.printf("\n");
	}

	void SET_RegisterCommands()
	{
		SDL_version sdlVersion;
		const char *ver_str = MY_L("APP00072|verze");

		// Print application info
		d6Console.printf(MY_L("APP00073|\n===Informace o aplikaci===\n"));
		d6Console.printf("%s %s: %s\n", APP_NAME, ver_str, APP_VERSION);
		d6Console.printf("Mylib %s: %s\n", ver_str, MYLIB_VERSION);
		SDL_GetVersion(&sdlVersion);
		d6Console.printf("SDL %s: %d.%d.%d\n", ver_str, sdlVersion.major, sdlVersion.minor, sdlVersion.patch);
		const SDL_version* mixVersion = Mix_Linked_Version();
		d6Console.printf("SDL_mixer %s: %d.%d.%d\n", ver_str, mixVersion->major, mixVersion->minor, mixVersion->patch);
		d6Console.printf(MY_L("APP00074|Jazyk: cestina\n"));

		// Set some console functions
		d6Console.setLast(15);
		d6Console.registerCommand(&SET_ToggleRenderMode, "switch_render_mode");
		d6Console.registerCommand(&SET_ToggleShowFps, "show_fps");
		d6Console.registerCommand(&SET_OpenGLInfo, "gl_info");
		d6Console.registerCommand(&SET_Language, "lang");
		d6Console.registerCommand(&SET_Volume, "volume");
		d6Console.registerCommand(&SET_MaxRounds, "rounds");
		d6Console.registerCommand(&SET_MusicOnOff, "music");
		d6Console.registerCommand(&SET_JoyScan, "joy_scan");
		d6Console.registerCommand(&SET_LoadSkin, "skin");
		d6Console.registerCommand(&SET_EnableWeapon, "gun");
		d6Console.registerCommand(&SET_AmmoRange, "start_ammo_range");		
		
		/* TODO:
		d6Console.regvar(&g_app.fps, "g_fps", CON_F_RONLY, CON_VAR_FLOAT);
		d6Console.regvar(&d6VideoMode.aa, "g_aa", CON_F_NONE, CON_VAR_INT);
		d6Console.regvar(&d6VideoMode.bpp, "g_bpp", CON_F_NONE, CON_VAR_INT);
		d6Console.regvar(&d6VideoMode.width, "g_cl_width", CON_F_NONE, CON_VAR_INT);
		d6Console.regvar(&d6VideoMode.height, "g_cl_height", CON_F_NONE, CON_VAR_INT);
		*/
	}

	/*
	==================================================
	Main init
	==================================================
	*/
	void SET_Init()
	{
		SET_RegisterCommands();

		srand((unsigned)time(nullptr));

		File::load(D6_FILE_COS, 0, d6Cos);
		Sound::init(20);

		// Read config file
		d6Console.printf("\n===Config===\n");
		d6Console.exec("exec data/config.txt");
		// Init player skins
		d6Console.printf("\n====Skin====\n");
		d6Console.exec("exec data/skin.txt");

		d6Video.initialize(APP_NAME, APP_FILE_ICON, d6Console);

		d6TextureManager.load(D6_TEXTURE_BCG_KEY, D6_TEXTURE_BCG_PATH, GL_LINEAR);
		d6TextureManager.load(D6_TEXTURE_EXPL_KEY, D6_TEXTURE_EXPL_PATH, GL_NEAREST);
		d6TextureManager.load(D6_TEXTURE_MENU_KEY, D6_TEXTURE_MENU_PATH, GL_LINEAR);
		d6TextureManager.load(D6_TEXTURE_WPN_KEY, D6_TEXTURE_WPN_PATH, GL_LINEAR);
		d6TextureManager.load(D6_TEXTURE_BLOCK_KEY, D6_TEXTURE_BLOCK_PATH, GL_LINEAR);

		WPN_Init();
		ELEV_Init();
		FIRE_Init();
		CONTROLS_Init();

		d6Menu.init();
		d6Menu.startContext();
	}
}