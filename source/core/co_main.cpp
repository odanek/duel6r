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
Popis: Jadro - zakladni funkce, vstupni bod
*/

#include "co_core.h"

// Deklarace procedur ktere musi byt nekde implementovany
namespace Duel6
{
	void    P_Init();
	void    P_DeInit();
	void    P_Main();
}

// Deklarace globalnich promenych
app_s       g_app;
appVid_s    g_vid;
appInp_s    g_inp;

/*
==================================================
Deinicializace jadra
==================================================
*/
static void CO_DeInit ()
{
    MY_DeInit ();
    SDL_Quit ();
}

/*
==================================================
Ulozeni obsahu konzoly na disk a uklid po chybe
==================================================
*/
static void CO_ErrorHandler (const char *str)
{
    if (g_app.con.get() == nullptr)
    {
        g_app.con->printf(str);
        g_app.con->exec("dump chyba.con");
    }

    Duel6::P_DeInit ();
    CO_DeInit ();
}

/*
==================================================
Inicializace jadra
==================================================
*/
static void CO_Init ()
{
    // Inicializace promenych jadra
    g_app.flags = APP_FLAG_NONE;
	g_app.con.reset(new con_c(CON_F_EXPAND));

    // Inicializace me knihovny
    MY_Init ();
    MY_AddPck (APP_FILE_CORE_PCK);
    MY_ErrCallback (CO_ErrorHandler);

    // Inicializace SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		MY_Err(MY_ErrDump("%S: %s\n", MY_L("COSTR0006|Chyba pri inicializaci grafickeho modu"), SDL_GetError()));
	}

    SDL_SetWindowTitle(g_app.window, APP_NAME);
    SDL_SetWindowIcon(g_app.window, SDL_LoadBMP (APP_FILE_ICON));

    // Inicializace jadra
    CO_FontLoad(APP_FILE_FONT);
    g_app.con->setfont(CO_FontGet());

    CON_RegisterBasicCmd(g_app.con.get());
}

/*
==================================================
Vstupni bod aplikace
==================================================
*/
int main (int argc, char *argv[])
{
    // Inicializace
    CO_Init ();
    Duel6::P_Init ();

    // Provedeni prikazu predanych z prikazove radky
	for (int i = 1; i < argc; i++)
	{
		g_app.con->exec(argv[i]);
	}

    // Predani kontroly aplikaci
    Duel6::P_Main();

    // Deinicializace
    Duel6::P_DeInit ();
    CO_DeInit ();

    return 0;
}
