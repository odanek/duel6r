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
#include <stdarg.h>
#include "project.h"
#include "LevelList.h"
#include "glib.h"

#define D6_ALL_CHR  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 -=\\~!@#$%^&*()_+|[];',./<>?:{}"

namespace Duel6
{
	static LevelList    d6LevelList;
	static d6PHIST_s    *d6PHist;
	static desk_c       *myDesk;
	static button_c     *myButton[7];
	static listbox_c    *myListbox[7];
	static label_c      *myLabel[8];
	static switchbox_c  *mySwitch[D6_MAX_PLAYERS];
	static textbox_c    *myTextbox;
	static int          d6Backs, d6WillPlay[D6_MAX_PLAYERS], d6PHists;
	static GLuint       d6MenuTex;
	static bool         d6MenuKeyMask = false;
	static const char   *d6SndFl[D6_SOUNDS] =
	{
		"sound/smrt.wav",
		"sound/bnpick.wav",
		"sound/blesk.wav",
		"sound/bmblesk.wav",
		"sound/laser.wav",
		"sound/pistol.wav",
		"sound/plasma.wav",
		"sound/shotgun.wav",
		"sound/bazooka.wav",
		"sound/bmbazook.wav",
		"sound/voda.wav",
		"sound/triton.wav",
		"sound/kulomet.wav",
		"sound/hit.wav",
		"sound/luk.wav",
		"sound/sliz.wav",
		"sound/kiss2.wav",
		"sound/kiss.wav",
		"sound/spray.wav",
		"sound/prak.wav",
		"sound/spunt.wav",
		"sound/shit.wav",
		"sound/shit-hit.wav",
		"sound/letsrock.wav",
		"sound/gameover.wav"
	};

	void MENU_Free(void)
	{
		MY_Free(d6PHist);
		MY_UnregMem(myDesk);
		delete myDesk;
	}

	/*
	==================================================
	Vytvori seznam urovni a zjisti pocet pozadi
	==================================================
	*/
	static void MENU_GetBcgCount(void)
	{
		myKh3info_s         ki;

		MY_KH3Open(D6_FILE_BACK);
		MY_KH3GetInfo(&ki);
		d6Backs = ki.picts;
		MY_KH3Close();
	}

	static void MENU_LoadPH(void)
	{
		myFile_s    *f;
		int         i;

		if (MY_FSize(D6_FILE_PHIST) < 20)
		{
			d6PHists = 0;
			for (i = 0; i < D6_MAX_PLAYERS; i++)
				d6WillPlay[i] = -1;
			return;
		}

		f = MY_FOpen(D6_FILE_PHIST, 0, "rb", true);
		MY_FRead(&d6PHists, 4, 1, f);
		if (d6PHists)
		{
			d6PHist = D6_MALLOC(d6PHIST_s, d6PHists);
			MY_FRead(d6PHist, sizeof (d6PHIST_s), d6PHists, f);
		}
		else
			d6PHist = NULL;
		MY_FRead(d6WillPlay, 4, 8, f);
		MY_FClose(&f);
	}

	/*
	==================================================
	Umoznuje hot plug joypadu za behu hry. Vyhleda
	nove pripojena zarizeni.
	==================================================
	*/
	void MENU_JoyRescan(void)
	{
		char    f[30];
		int     i, j;

		CO_InpInit(APP_INP_INIT_JOY);

		for (i = 0; i < D6_MAX_PLAYERS; i++)
			mySwitch[i]->Clear();

		for (i = 0; i < 4; i++)
		{
			sprintf(f, "%s %d", MY_L("APP00045|Klavesy"), i + 1);
			for (j = 0; j < D6_MAX_PLAYERS; j++)
				mySwitch[j]->AddItem(f);
		}

		for (i = 0; i < g_inp.joy_num; i++)
		{
			sprintf(f, "Joypad %d", i + 1);
			for (j = 0; j < D6_MAX_PLAYERS; j++)
				mySwitch[j]->AddItem(f);
		}

		for (i = 0; i < D6_MAX_PLAYERS; i++)
			mySwitch[i]->SetCur(i % (4 + g_inp.joy_num));
	}

	void MENU_Init(void)
	{
		char        f[30];
		int         i;

		g_app.con->printf(MY_L("APP00029|\n===Menu inicializace===\n"));
		UTIL_LoadKH3Texture(&d6MenuTex, D6_FILE_LABEL, 0, false);
		MENU_LoadPH();
		g_app.con->printf(MY_L("APP00030|...Startuji knihovnu glib\n"));
		myDesk = desk_c::Create();
		myDesk->ScreenSize(g_vid.cl_width, g_vid.cl_height,
			(g_vid.cl_width - 800) / 2, (g_vid.cl_height - 600) / 2);

		myListbox[0] = new listbox_c(true);
		MY_RegMem(myListbox[0], sizeof (listbox_c));
		myListbox[0]->SetPosition(10, 400, 94, 12, 16);
		myListbox[0]->SetNG(12, 1);

		myListbox[1] = new listbox_c(true);
		MY_RegMem(myListbox[1], sizeof (listbox_c));
		myListbox[1]->SetPosition(10, 129, 20, 13, 18);
		myListbox[1]->SetNG(0, 1);

		myListbox[2] = new listbox_c(false);
		MY_RegMem(myListbox[2], sizeof (listbox_c));
		myListbox[2]->SetPosition(200, 129, 20, D6_MAX_PLAYERS, 18);
		myListbox[2]->SetNG(1, 1);

		myListbox[3] = new listbox_c(true);
		MY_RegMem(myListbox[3], sizeof (listbox_c));
		myListbox[3]->SetPosition(644, 189, 13, 6, 16);
		myListbox[3]->SetNG(2, 1);

		myListbox[4] = new listbox_c(true);
		MY_RegMem(myListbox[4], sizeof (listbox_c));
		myListbox[4]->SetPosition(500, 236, 13, 3, 16);
		myListbox[4]->SetNG(3, 1);

		myListbox[5] = new listbox_c(false);
		MY_RegMem(myListbox[5], sizeof (listbox_c));
		myListbox[5]->SetPosition(644, 129, 15, 2, 16);
		myListbox[5]->SetNG(4, 1);
		myListbox[5]->AddItem(MY_L("APP00031|Cela obrazovka"));
		myListbox[5]->AddItem(MY_L("APP00032|Split screen"));

		myListbox[6] = new listbox_c(true);
		MY_RegMem(myListbox[6], sizeof (listbox_c));
		myListbox[6]->SetPosition(500, 129, 13, 5, 16);
		myListbox[6]->SetNG(5, 1);

		myButton[0] = new button_c;
		MY_RegMem(myButton[0], sizeof (button_c));
		myButton[0]->SetPosition(200, 281, 80, 31);
		myButton[0]->SetCaption(">>");
		myButton[0]->SetNG(6, 1);

		myButton[1] = new button_c;
		MY_RegMem(myButton[1], sizeof (button_c));
		myButton[1]->SetPosition(200, 316, 80, 31);
		myButton[1]->SetCaption("<<");
		myButton[1]->SetNG(7, 1);

		myButton[2] = new button_c;
		MY_RegMem(myButton[2], sizeof (button_c));
		myButton[2]->SetPosition(284, 281, 80, 31);
		myButton[2]->SetCaption(MY_L("APP00033|Smaz"));
		myButton[2]->SetNG(8, 1);

		myButton[3] = new button_c;
		MY_RegMem(myButton[3], sizeof (button_c));
		myButton[3]->SetPosition(284, 316, 80, 31);
		myButton[3]->SetCaption(MY_L("APP00034|Pridej"));
		myButton[3]->SetNG(9, 1);

		myButton[6] = new button_c;
		MY_RegMem(myButton[6], sizeof (button_c));
		myButton[6]->SetPosition(370, 281, 120, 31);
		myButton[6]->SetCaption(MY_L("APP00092|Vynuluj (F3)"));
		myButton[6]->SetNG(30, 1);

		myButton[4] = new button_c;
		MY_RegMem(myButton[4], sizeof (button_c));
		myButton[4]->SetPosition(500, 300, 125, 73);
		myButton[4]->SetCaption(MY_L("APP00035|Hrat (F1)"));
		myButton[4]->SetNG(10, 1);

		myButton[5] = new button_c;
		MY_RegMem(myButton[5], sizeof (button_c));
		myButton[5]->SetPosition(644, 300, 125, 73);
		myButton[5]->SetCaption(MY_L("APP00036|Konec (ESC)"));
		myButton[5]->SetNG(11, 1);

		myLabel[0] = new label_c;
		MY_RegMem(myLabel[0], sizeof (label_c));
		myLabel[0]->SetPosition(10, 380, 772, 18);
		myLabel[0]->SetCaption(MY_L("APP00037|       Jmeno        |    Her    | Vitezstvi |   Strel   | Presnost  |   Zabiti   |   Bodu"));

		myLabel[1] = new label_c;
		MY_RegMem(myLabel[1], sizeof (label_c));
		myLabel[1]->SetPosition(500, 216, 125, 18);
		myLabel[1]->SetCaption(MY_L("APP00038|Pozadi"));

		myLabel[2] = new label_c;
		MY_RegMem(myLabel[2], sizeof (label_c));
		myLabel[2]->SetPosition(644, 170, 125, 18);
		myLabel[2]->SetCaption(MY_L("APP00039|Mapa"));

		myLabel[3] = new label_c;
		MY_RegMem(myLabel[3], sizeof (label_c));
		myLabel[3]->SetPosition(644, 110, 125, 18);
		myLabel[3]->SetCaption(MY_L("APP00040|Mod obrazovky"));

		myLabel[4] = new label_c;
		MY_RegMem(myLabel[4], sizeof (label_c));
		myLabel[4]->SetPosition(500, 110, 125, 18);
		myLabel[4]->SetCaption(MY_L("APP00041|Zoom"));

		myLabel[5] = new label_c;
		MY_RegMem(myLabel[5], sizeof (label_c));
		myLabel[5]->SetPosition(10, 110, 181, 18);
		myLabel[5]->SetCaption(MY_L("APP00042|Databaze hracu"));

		myLabel[6] = new label_c;
		MY_RegMem(myLabel[6], sizeof (label_c));
		myLabel[6]->SetPosition(200, 110, 165, 18);
		myLabel[6]->SetCaption(MY_L("APP00043|Hraci"));

		myLabel[7] = new label_c;
		MY_RegMem(myLabel[7], sizeof (label_c));
		myLabel[7]->SetPosition(370, 110, 120, 18);
		myLabel[7]->SetCaption(MY_L("APP00044|Ovladani"));

		myTextbox = new textbox_c;
		MY_RegMem(myTextbox, sizeof (textbox_c));
		myTextbox->SetPosition(200, 351, 19, 13, D6_ALL_CHR);
		myTextbox->SetNG(13, 1);

		// Switchbox - volba ovladani
		for (i = 0; i < D6_MAX_PLAYERS; i++)
		{
			mySwitch[i] = new switchbox_c;
			MY_RegMem(mySwitch[i], sizeof (switchbox_c));
			mySwitch[i]->SetPosition(370, 131 + i * 18, 120, 0);
			mySwitch[i]->SetNG(14 + i, 1);
		}

		MENU_JoyRescan();

		MENU_GetBcgCount();
		d6LevelList.Initialize(D6_FILE_LEVEL, D6_LEVEL_EXTENSION);

		myListbox[3]->AddItem(MY_L("APP00046|Nahodna"));
		for (i = 0; i < (int)d6LevelList.Length(); i++)
			myListbox[3]->AddItem(d6LevelList.FileName((Size)i).c_str());

		myListbox[4]->AddItem(MY_L("APP00047|Nahodne"));
		for (i = 0; i < d6Backs; i++)
		{
			sprintf(f, "%d", i + 1);
			myListbox[4]->AddItem(f);
		}

		for (i = 5; i < 21; i++)
		{
			sprintf(f, "%d", i);
			myListbox[6]->AddItem(f);
		}
		myListbox[6]->SetCur(8);

		for (i = 0; i < d6PHists; i++)
			myListbox[1]->AddItem(d6PHist[i].Name);

		d6Playing = 0;
		for (i = 0; i < D6_MAX_PLAYERS; i++)
			if (d6WillPlay[i] != -1)
			{
				myListbox[2]->AddItem(d6PHist[d6WillPlay[i]].Name);
				d6Playing++;
			}

		g_app.con->printf(MY_L("APP00086|\n===Nacteni hudebnich souboru===\n"));
		SOUND_LoadModule("sound/undead.xm");
		for (i = 0; i < D6_SOUNDS; i++)
			SOUND_LoadSample(d6SndFl[i]);
	}

	static void MENU_End(void)
	{
		d6InMenu = false;
		SDL_ShowCursor(SDL_DISABLE);
		SOUND_StopMusic();
	}

	void MENU_SavePH(void)
	{
		myFile_s    *f;

		f = MY_FOpen(D6_FILE_PHIST, 0, "wb", true);
		MY_FWrite(&d6PHists, 4, 1, f);
		if (d6PHists)
			MY_FWrite(d6PHist, sizeof (d6PHIST_s), d6PHists, f);
		MY_FWrite(d6WillPlay, 4, 8, f);
		MY_FClose(&f);
	}

	static void MENU_Pit(char *r, const char *f, ...)
	{
		char    r2[50];
		int i = 0;
		va_list argptr;

		va_start(argptr, f);
		vsprintf(r2, f, argptr);
		va_end(argptr);

		while (r2[i])
			*(r++) = r2[i++];
	}

	static void MENU_RebuildTable(void)
	{
		char    ret[100];
		int     s, i, *pi, *pb, k;

		myListbox[0]->Clear();
		if (!d6PHists)
			return;

		pi = D6_MALLOC(int, d6PHists);
		pb = D6_MALLOC(int, d6PHists);

		for (i = 0; i < d6PHists; i++)
		{
			pi[i] = i;
			pb[i] = d6PHist[i].Kills + d6PHist[i].Wins;
		}

		for (k = 0; k < d6PHists - 1; k++)
			for (i = 0; i < d6PHists - 1 - k; i++)
				if (pb[pi[i + 1]] > pb[pi[i]])
				{
					s = pi[i];
					pi[i] = pi[i + 1];
					pi[i + 1] = s;
				}

		for (i = 0; i < d6PHists; i++)
		{
			k = pi[i];
			memset(ret, 0, 100);
			memset(ret, ' ', 94);
			MENU_Pit(ret, "%s", d6PHist[k].Name);
			MENU_Pit(&ret[20], "| %d", d6PHist[k].Games);
			MENU_Pit(&ret[32], "| %d", d6PHist[k].Wins);
			MENU_Pit(&ret[44], "| %d", d6PHist[k].Shots);
			if (d6PHist[k].Shots)
				MENU_Pit(&ret[56], "| %d%%", d6PHist[k].Hits * 100 / d6PHist[k].Shots);
			else
				MENU_Pit(&ret[56], "| 0%%");
			MENU_Pit(&ret[68], "| %d", d6PHist[k].Kills);
			MENU_Pit(&ret[81], "| %d", pb[k]);
			myListbox[0]->AddItem(ret);
		}

		MY_Free(pi);
		MY_Free(pb);
	}

	static void MENU_PridejHrace(void)
	{
		int     i, c;

		c = myListbox[1]->CurItem();
		if (c != -1 && d6Playing < D6_MAX_PLAYERS)
		{
			for (i = 0; i < D6_MAX_PLAYERS; i++)
				if (d6WillPlay[i] == c)
					return;
			d6WillPlay[d6Playing++] = c;
			myListbox[2]->AddItem(d6PHist[c].Name);
		}
	}

	static void MENU_AddPH(void)
	{
		d6PHIST_s   *np;
		char        *t;

		t = myTextbox->Text();
		if (strlen(t) > 0)
		{
			d6PHists++;
			np = D6_MALLOC(d6PHIST_s, d6PHists);
			if (d6PHists > 1)
				memcpy(np, d6PHist, (d6PHists - 1) * sizeof (d6PHIST_s));
			memset(&np[d6PHists - 1], 0, sizeof (d6PHIST_s));
			strcpy(np[d6PHists - 1].Name, t);
			MY_Free(d6PHist);
			d6PHist = np;
			myListbox[1]->AddItem(t);
			MENU_RebuildTable();
			myTextbox->Flush();
		}
	}

	// Vykresli otazku, zda opravdu smazat
	static bool MENU_DelQuestion(void)
	{
		int x = g_vid.cl_width / 2 - 150, y = g_vid.cl_height / 2 - 10;
		d6MenuKeyMask = true;

		glColor3f(1.0f, 0.8f, 0.8f);
		glBegin(GL_QUADS);
		glVertex2i(x, y);
		glVertex2i(x, y + 20);
		glVertex2i(x + 300, y + 20);
		glVertex2i(x + 300, y);
		glEnd();
		glColor3f(0, 0, 0);
		glLineWidth(2);
		glBegin(GL_LINE_LOOP);
		glVertex2i(x, y);
		glVertex2i(x, y + 20);
		glVertex2i(x + 300, y + 20);
		glVertex2i(x + 300, y);
		glEnd();
		glLineWidth(1);
		CO_FontColor(255, 0, 0);
		CO_FontPrintf(x + 70, y + 2, MY_L("APP00090|Opravdu smazat? (A/N)"));
		VID_SwapBuffers();

		while (!g_inp.key[SDLK_a] && !g_inp.key[SDLK_y] && !g_inp.key[SDLK_n])
			CO_ProcessEvents();

		while (CO_InpGetKey(false))
			CO_ProcessEvents();

		d6MenuKeyMask = false;

		return !g_inp.key[SDLK_n];
	}

	static void MENU_CleanPH(void)
	{
		int     i;

		if (!MENU_DelQuestion())
			return;

		for (i = 0; i < d6PHists; i++)
		{
			d6PHist[i].Games = 0;
			d6PHist[i].Hits = 0;
			d6PHist[i].Kills = 0;
			d6PHist[i].Shots = 0;
			d6PHist[i].Wins = 0;
		}
		MENU_RebuildTable();
		MENU_SavePH();
	}

	void MENU_Restart(bool same_level)
	{
		static int  last = -1;

		if (!same_level || last == -1)
		{
			if (!myListbox[3]->CurItem())
				last = rand() % d6LevelList.Length();
			else
				last = myListbox[3]->CurItem() - 1;
		}

		D6_StartGame(d6LevelList.Path(last));

		if (!myListbox[4]->CurItem())
			SET_LoadBackground(rand() % d6Backs);
		else
			SET_LoadBackground(myListbox[4]->CurItem() - 1);

		g_inp.key[SDLK_ESCAPE] = 0;
		d6KeyWait = APP_FPS_SPEED;

		SOUND_PlaySample(D6_SND_LETS_ROCK);
	}

	static void MENU_Play(void)
	{
		MENU_End();

		if (d6Playing > 4)
			d6ScreenMode = ScreenMode::FullScreen;
		else
			d6ScreenMode = (myListbox[5]->CurItem() == 0) ? ScreenMode::FullScreen : ScreenMode::SplitScreen;

		d6ZoomBlc = myListbox[6]->CurItem() + 5;

		for (Size i = 0; i < d6Playing; i++)
		{
			d6Player[i]->State.PH = &d6PHist[d6WillPlay[i]];
		}

		for (Size i = 0; i < d6Playing; i++)
		{
			d6Player[i]->SetControls(mySwitch[i]->CurItem());
		}

		MENU_Restart(false);
	}

	static void MENU_UberHrace(int c)
	{
		int     i;

		if (c != -1)
		{
			myListbox[2]->DelItem(c);
			for (i = c; i < D6_MAX_PLAYERS - 1; i++)
				d6WillPlay[i] = d6WillPlay[i + 1];
			d6WillPlay[i] = -1;
			d6Playing--;
		}
	}

	static void MENU_DelPH(void)
	{
		d6PHIST_s   *np;
		int         i, j, c;

		if (!MENU_DelQuestion())
			return;

		c = myListbox[1]->CurItem();
		if (c != -1)
		{
			myListbox[1]->DelItem(c);
			for (i = 0; i < D6_MAX_PLAYERS; i++)
				if (d6WillPlay[i] == c)
					MENU_UberHrace(i);
			if (--d6PHists == 0)
			{
				MY_Free(d6PHist);
				return;
			};
			np = D6_MALLOC(d6PHIST_s, d6PHists);
			for (i = 0; i <= d6PHists; i++)
				if (i < c)
					memcpy(&np[i], &d6PHist[i], sizeof (d6PHIST_s));
				else if (i > c)
				{
					for (j = 0; j < D6_MAX_PLAYERS; j++)
						if (d6WillPlay[j] == i)
							d6WillPlay[j]--;
					memcpy(&np[i - 1], &d6PHist[i], sizeof (d6PHIST_s));
				}
			MY_Free(d6PHist);
			d6PHist = np;
		}
	}

	void MENU_Start(void)
	{
		d6InMenu = true;
		SDL_ShowCursor(SDL_ENABLE);
		MENU_RebuildTable();
		if (d6PlayMusic)
			SOUND_StartMusic(0, false);
	}

	void MENU_KeyEvent(int e)
	{
		if (!d6MenuKeyMask)
		{
			myDesk->KeyEvent(e);
			if (e == SDLK_RETURN)
				MENU_AddPH();
			if (e == SDLK_F1)
				if (d6Playing > 1)
					MENU_Play();
			if (e == SDLK_F3)
				MENU_CleanPH();
			if (e == SDLK_ESCAPE)
			{
				g_app.flags |= APP_FLAG_QUIT;
				MENU_SavePH();
			}
		}
	}

	static void MENU_Update(void)
	{
		static  float sync = 0;
		int     event, from, n, g;

		sync += g_app.frame_interval;

		while (sync > 1)
		{
			sync--;

			myDesk->Check(event, from, n, g);
			if (g == 1 && event == GLIB_E_RELEASED)
			{
				switch (n)
				{
				case 6:
					MENU_PridejHrace();
					break;
				case 7:
					MENU_UberHrace(myListbox[2]->CurItem());
					break;
				case 8:
					MENU_DelPH();
					MENU_RebuildTable();
					break;
				case 9:
					MENU_AddPH();
					break;
				case 10:
					if (d6Playing > 1)
						MENU_Play();
					break;
				case 11:
					MENU_End();
					g_app.flags |= APP_FLAG_QUIT;
					MENU_SavePH();
					return;
				case 30:
					MENU_CleanPH();
					break;
				}
			}
		}
	}

	static void MENU_Draw(void)
	{
		int     tr_x = (g_vid.cl_width - 800) / 2, tr_y = (g_vid.cl_height - 600) / 2;

		myDesk->Draw();

		glPushMatrix();
		glTranslatef((GLfloat)tr_x, (GLfloat)-tr_y, 0);

		CO_FontColor(255, 255, 255);
		CO_FontPrintf(687, g_vid.cl_height - 20, "%s %s", MY_L("APP00048|verze"), APP_VERSION);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, d6MenuTex);
		glColor3ub(255, 255, 255);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2i(300, g_vid.cl_height - 5);
		glTexCoord2f(1.0f, 0.0f); glVertex2i(500, g_vid.cl_height - 5);
		glTexCoord2f(1.0f, 1.0f); glVertex2i(500, g_vid.cl_height - 100);
		glTexCoord2f(0.0f, 1.0f); glVertex2i(300, g_vid.cl_height - 100);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
	}

	void MENU_Loop(void)
	{
		CO_FpsSyncLoops(&MENU_Update, &MENU_Draw);
	}
}