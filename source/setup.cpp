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
#include <SDL/SDL_mixer.h>
#include "project.h"

namespace Duel6
{
	extern  int     d6Winner;
	static  bool    d6BcgLoaded = false;

	struct d6CONVAR_s
	{
		int     bpp;
		int     aa;
		int     vid_width;
		int     vid_height;
	};

	d6CONVAR_s  d6ConVar = { 
		D6_CL_BPP, 
		D6_CL_AA, 
		D6_CL_WIDTH, 
		D6_CL_HEIGHT 
	};

	/*
	==================================================
	Inicializace zbrani - enabled jsou pouze ty stare
	==================================================
	*/
	void SET_InitWeapons(void)
	{
		for (int i = 0; i < D6_WEAPONS; i++)
			d6WpnEnabled[i] = i < 12;
	}

	/*
	==================================================
	Video init
	==================================================
	*/
	void SET_InitVideo(void)
	{
		g_app.con->printf(MY_L("APP00055|\n===Nastavuji OpenGL okno===\n"));

		// Get current video mode
		const SDL_VideoInfo* cur_vid = SDL_GetVideoInfo();
		if (!d6ConVar.vid_width)
		{
			d6ConVar.vid_width = cur_vid->current_w;
		}
		if (!d6ConVar.vid_height)
		{
			d6ConVar.vid_height = cur_vid->current_h;
		}
		if (!d6ConVar.bpp)
		{
			d6ConVar.bpp = cur_vid->vfmt->BitsPerPixel;
		}

		// Set graphics mode
		//VID_SetMode(d6ConVar.vid_width, d6ConVar.vid_height, d6ConVar.bpp, d6ConVar.aa, true);
		VID_SetMode (800, 600, d6ConVar.bpp, d6ConVar.aa, false);

		g_vid.gl_fov = 45.0f;
		g_vid.gl_nearclip = 0.1f;
		g_vid.gl_farclip = 100.0f;

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glCullFace(GL_FRONT);
		glAlphaFunc(GL_GEQUAL, 1);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		D6_SetGLMode(D6_GL_ORTHO);
	}

	static void SET_InitWorldStruct(void)
	{
		d6World.Vertex = NULL;
		d6World.Face = NULL;
		d6World.Level.Data = NULL;
	}

	static void SET_LoadWorldTextures(void)
	{
		myFile_s        *f;
		d6ANM           *anm = &d6World.Anm;
		myKh3info_s     ki;
		myUINT          i;

		g_app.con->printf(MY_L("APP00056|Nahravam textury urovne\n"));
		MY_KH3Open(D6_FILE_ART);
		MY_KH3GetInfo(&ki);
		g_app.con->printf(MY_L("APP00057|...Soubor %s obsahuje %lu textur\n"), D6_FILE_ART, ki.picts);

		anm->Textures = ki.picts;
		anm->Anim = D6_MALLOC(int, ki.picts);
		anm->Znak = D6_MALLOC(int, ki.picts);
		anm->TexGlNum = D6_MALLOC(myUINT, ki.picts);

		g_app.con->printf(MY_L("APP00058|Nahravam animacni data (%s)\n"), D6_FILE_ANM);
		f = MY_FOpen(D6_FILE_ANM, 0, "rb", true);
		for (i = 0; i < ki.picts; i++)
		{
			MY_FRead(&anm->Anim[i], 4, 1, f);
			MY_FRead(&anm->Znak[i], 4, 1, f);
		}
		MY_FClose(&f);

		for (i = 0; i < ki.picts; i++)
			UTIL_LoadKH3Texture(&anm->TexGlNum[i], D6_FILE_ART, i, false);
		MY_KH3Close();
	}

	void SET_Players(void)
	{
		int     i;

		for (i = 0; i < D6_MAX_PLAYERS; i++)
		{
			d6Player[i] = new Player(i);
			MY_RegMem(d6Player[i], sizeof (Player));
			d6Player[i]->SetSkin(d6PlayerSkin[i]);
		}
	}

	void SET_LoadBackground(int n)
	{
		if (d6BcgLoaded)
			glDeleteTextures(1, &d6BackTex);

		g_app.con->printf(MY_L("APP00059|...Nahravam pozadi (%s, %d)\n"), D6_FILE_BACK, n);
		UTIL_LoadKH3Texture(&d6BackTex, D6_FILE_BACK, n, false);
		d6BcgLoaded = true;
	}

	void SET_LoadWorld(const char *fl)
	{
		bool    mirror = rand() % 2 ? true : false;
		int     av, bonus[D6_BONUS_MAX][3];

		g_app.con->printf(MY_L("APP00060|\n===Nahravam uroven %s===\n"), fl);
		LOADER_LoadWorld(fl, &d6World, ANM_MAX, mirror, bonus);
		g_app.con->printf(MY_L("APP00061|...Sirka   : %d\n"), d6World.Level.SizeX);
		g_app.con->printf(MY_L("APP00062|...Vyska   : %d\n"), d6World.Level.SizeY);
		g_app.con->printf(MY_L("APP00063|...Bloku   : %d\n"), d6World.Blocks);
		g_app.con->printf(MY_L("APP00064|...Spritu  : %d\n"), d6World.Sprites);
		g_app.con->printf(MY_L("APP00065|...Voda    : %d\n"), d6World.Waters);

		glVertexPointer(3, GL_FLOAT, sizeof (d6VERTEX), (void *)&d6World.Vertex[0].X);
		glTexCoordPointer(2, GL_FLOAT, sizeof (d6VERTEX), (void *)&d6World.Vertex[0].U);

		d6World.Anm.Wait = 0;
		WATER_Build();
		av = d6World.Faces << 2;
		ANM_Init(&d6World.Vertex[av], av);
		g_app.con->printf(MY_L("APP00066|...Pripravuji hrace\n"));
		PLAYER_PrepareAll();
		g_app.con->printf(MY_L("APP00067|...Inicializace urovne\n"));
		WPN_LevelInit();
		KONTR_Init();
		EXPL_Init();
		INFO_Init();
		BONUS_Init(bonus);
		ELEV_Load(fl, mirror);
		FIRE_Find();
		d6Winner = -1;

		glDrawBuffer(GL_FRONT_AND_BACK);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawBuffer(GL_BACK);
	}

	/*
	==================================================
	Pokud neexistuji soubory config.txt a duel6.dat tak
	je vytvori - pro snadnejsi update dodavany bez techto
	souboru
	==================================================
	*/
	void SET_InitUserFiles(void)
	{
		char        def_file[50];
		const char *user_file[3] = { D6_FILE_CONFIG, D6_FILE_PHIST, D6_FILE_SKIN };
		int         i;
		mySIZE      len;
		myBYTE      *ptr;
		myFile_s    *f;

		for (i = 0; i < 3; i++)
		{
			if (MY_FSize(user_file[i]) < 1)
			{
				sprintf(def_file, "%s.def", user_file[i]);
				len = MY_FSize(def_file);
				ptr = (myBYTE *)MY_Alloc(len);
				MY_FLoad(def_file, ptr);
				f = MY_FOpen(user_file[i], 0, "wb", true);
				MY_FWrite(ptr, 1, len, f);
				MY_FClose(&f);
				MY_Free(ptr);
			}
		}
	}

	void P_DeInit(void)
	{
		int     i;

		SOUND_DeInit();

		glDeleteTextures(d6World.Anm.Textures, d6World.Anm.TexGlNum);
		WPN_FreeTextures();
		MY_Free(d6World.Anm.Znak);
		MY_Free(d6World.Anm.Anim);
		MY_Free(d6World.Anm.TexGlNum);
		MY_Free(d6World.Vertex);
		MY_Free(d6World.Face);
		MY_Free(d6World.Level.Data);
		MENU_Free();
		WATER_Free();
		FIRE_Free();
		ELEV_Free();
		EXPL_Free();

		for (i = 0; i < D6_MAX_PLAYERS; i++)
		{
			MY_UnregMem(d6Player[i]);
			delete d6Player[i];
		}

		WPN_DeInit();
	}

	/*
	==================================================
	Memory check - console command
	==================================================
	*/
	void SET_Check(con_c *con)
	{
#if defined (_MSC_VER) || defined (MY_LEAK_CHECK)
		mySIZE      mu = MY_MemUsed();

		con->printf("%s: ", MY_L("APP00068|Alokovana pamet"));
		if (mu < (2 << 10))
			con->printf("%d B\n", mu);
		else if (mu < (2 << 20))
			con->printf("%.2f KiB\n", mu / 1024.0f);
		else if (mu < (mySIZE)1073741824)
			con->printf("%.2f MiB\n", mu / 1048576.0f);
		else
			con->printf("%.2f GiB\n", mu / 1073741824.0f);
#else
		con->printf ("%s: %s\n", con->argv (0), MY_L("APP00069|Kontrola alokovane pameti nebyla prikompilovana"));
#endif
	}

	/*
	==================================================
	Set language - console command
	==================================================
	*/
	void SET_Language(con_c *con)
	{
		if (con->argc() == 2)
		{
			if (!strcmp(con->argv(1), "lang/czech.lang"))
				MY_LangFree();
			MY_LangLoad(con->argv(1));
		}
		else
			con->printf("%s: %s\n", MY_L("APP00070|Jazyk"), MY_L("APP00071|cestina"));
	}

	/*
	==================================================
	Nastaveni volume hudby pres konzolu
	==================================================
	*/
	void SET_Volume(con_c *con)
	{
		if (con->argc() == 2)
			SOUND_Volume(atoi(con->argv(1)));
	}

	/*
	==================================================
	Zapnuti/vypnuti hudby v menu
	==================================================
	*/
	void SET_MusicOnOff(con_c *con)
	{
		if (con->argc() == 2)
		{
			if (!strcmp(con->argv(1), "on"))
			{
				d6PlayMusic = true;
				if (d6InMenu)
					SOUND_StartMusic(0, false);
			}
			if (!strcmp(con->argv(1), "off"))
			{
				d6PlayMusic = false;
				if (d6InMenu)
					SOUND_StopMusic();
			}
		}
	}

	/*
	==================================================
	Vyhledani nove pripojenych joypadu
	==================================================
	*/
	void SET_JoyScan(con_c *con)
	{
		if (d6InMenu)
			MENU_JoyRescan();
	}

	/*
	==================================================
	Nastaveni skinu
	==================================================
	*/
	void SET_LoadSkin(con_c *con)
	{
		int     i, pl, pos, num, exp16, c;

		if (con->argc() == 11)
		{
			pl = atoi(con->argv(1));
			if (pl >= 0 && pl < D6_MAX_PLAYERS)
			{
				for (i = 0; i < 9; i++)
				{
					pos = strlen(con->argv(i + 2)) - 1;
					num = 0;
					exp16 = 1;
					while (pos >= 0)
					{
						c = con->argv(i + 2)[pos];
						if (c >= '0' && c <= '9')
							num += (c - '0') * exp16;
						if (c >= 'a' && c <= 'f')
							num += (c - 'a' + 10) * exp16;
						pos--;
						exp16 *= 16;
					}

					Color color((num & 0xff0000) >> 16, (num & 0xff00) >> 8, num & 0xff);
					d6PlayerSkin[pl].Set((PlayerSkinColors::BodyPart)i, color);
				}
				con->printf("Skin %d: OK\n", pl);
			}
		}
		if (con->argc() == 2)
		{
			pl = atoi(con->argv(1));
			if (pl >= 0 && pl < D6_MAX_PLAYERS)
			{
				con->printf("Skin %d: ", pl);
				for (i = 0; i < 9; i++)
				{
					const Color& color = d6PlayerSkin[pl].Get((PlayerSkinColors::BodyPart)i);
					con->printf("%02x%02x%02x ", color.Red(), color.Green(), color.Blue());
				}
				con->printf("\n");
			}
		}

		if (con->argc() == 1)
		{
			for (i = 0; i < D6_MAX_PLAYERS; i++)
			{
				if (d6Player[i] != nullptr)
				{
					d6Player[i]->SetSkin(d6PlayerSkin[i]);
				}
			}
		}
	}

	/*
	==================================================
	Povoleni/zakazani zbrane
	==================================================
	*/
	void SET_EnableWeapon(con_c *con)
	{
		int gn;

		if (con->argc() == 3)
		{
			gn = atoi(con->argv(1));
			if (gn >= 0 && gn < D6_WEAPONS)
			{
				d6WpnEnabled[gn] = !strcmp(con->argv(2), "true");
				if (d6WpnEnabled[gn])
					con->printf("\t%02d. %-13s %s\n", gn, MY_L(d6WpnDef[gn].Name), MY_L("APP00113|povoleno"));
				else
					con->printf("\t%02d. %-13s %s\n", gn, MY_L(d6WpnDef[gn].Name), MY_L("APP00114|zakazano"));
			}
		}
		else
			for (gn = 0; gn < D6_WEAPONS; gn++)
				if (d6WpnEnabled[gn])
					con->printf("\t%02d. %-13s %s\n", gn, MY_L(d6WpnDef[gn].Name), MY_L("APP00113|povoleno"));
				else
					con->printf("\t%02d. %-13s %s\n", gn, MY_L(d6WpnDef[gn].Name), MY_L("APP00114|zakazano"));
	}

	/*
	==================================================
	Nastaveni rozsahu poctu naboju po startu hry
	==================================================
	*/
	void SET_AmmoRange(con_c *con)
	{
		if (con->argc() == 3)
		{
			int min = atoi(con->argv(1)), max = atoi(con->argv(2));

			if (min <= max && min >= 0)
			{
				d6AmmoRangeMin = min;
				d6AmmoRangeMax = max;
			}
		}
		else if (con->argc() == 1)
		{
			con->printf("\tmin = %d\n\tmax = %d\n", d6AmmoRangeMin, d6AmmoRangeMax);
		}
		else
		{
			con->printf("%s: %s [min max]\n", con->argv(0), con->argv(0));
		}
	}

	/*
	==================================================
	Main init
	==================================================
	*/
	void P_Init(void)
	{
		const SDL_version   *sdl_ver;
		const char          *ver_str = MY_L("APP00072|verze");

		// Print application info
		g_app.con->printf(MY_L("APP00073|\n===Informace o aplikaci===\n"));
		g_app.con->printf("%s %s: %s\n", APP_NAME, ver_str, APP_VERSION);
		g_app.con->printf("Mylib %s: %s\n", ver_str, MYLIB_VERSION);
		sdl_ver = SDL_Linked_Version();
		g_app.con->printf("SDL %s: %d.%d.%d\n", ver_str, sdl_ver->major, sdl_ver->minor, sdl_ver->patch);
		sdl_ver = Mix_Linked_Version();
		g_app.con->printf("SDL_mixer %s: %d.%d.%d\n", ver_str, sdl_ver->major, sdl_ver->minor, sdl_ver->patch);
		g_app.con->printf(MY_L("APP00074|Jazyk: cestina\n"));

		// Set some console functions
		g_app.con->setlast(15);
		g_app.con->regcmd(&D6_ConSwitchW, "switch_render_mode");
		g_app.con->regcmd(&D6_ConShowFps, "show_fps");
		g_app.con->regcmd(&SET_Check, "memory_used");
		g_app.con->regcmd(&UTIL_OpenGLInfo, "gl_info");
		g_app.con->regcmd(&SET_Language, "lang");
		g_app.con->regcmd(&SET_Volume, "volume");
		g_app.con->regcmd(&SET_MusicOnOff, "music");
		g_app.con->regcmd(&SET_JoyScan, "joy_scan");
		g_app.con->regcmd(&SET_LoadSkin, "skin");
		g_app.con->regcmd(&SET_EnableWeapon, "gun");
		g_app.con->regcmd(&SET_AmmoRange, "start_ammo_range");
		g_app.con->regvar(&g_app.fps, "g_fps", CON_F_RONLY, CON_VAR_INT);
		g_app.con->regvar(&d6ConVar.aa, "g_aa", CON_F_NONE, CON_VAR_INT);
		g_app.con->regvar(&d6ConVar.bpp, "g_bpp", CON_F_NONE, CON_VAR_INT);
		g_app.con->regvar(&d6ConVar.vid_width, "g_cl_width", CON_F_NONE, CON_VAR_INT);
		g_app.con->regvar(&d6ConVar.vid_height, "g_cl_height", CON_F_NONE, CON_VAR_INT);

		srand((unsigned)time(NULL));

		MY_FLoadBlock(D6_FILE_SIN, 0, -1, (void *)d6Sin);

		SOUND_Init(20, 30, 1);

		SET_InitUserFiles();
		SET_InitWeapons();

		// Read config file
		g_app.con->printf("\n===Config===\n");
		g_app.con->exec("exec data/config.txt");
		g_app.con->printf("\n====Skin====\n");
		g_app.con->exec("exec data/skin.txt");

		SET_InitVideo();
		SET_InitWorldStruct();
		SET_LoadWorldTextures();
		WPN_LoadTextures();
		EXPL_Load();
		ELEV_Init();
		FIRE_Init();
		SET_Players();

		WPN_Init();

		MENU_Init();
		MENU_Start();
	}
}