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
void    P_Init          (void);
void    P_DeInit        (void);
void    P_Main          (void);
void    P_KeyEvent      (int key);
void    P_ActiveEvent   (bool active);

// Deklarace globalnich promenych
app_s       g_app;
appVid_s    g_vid;
appInp_s    g_inp;

/*
==================================================
Zpracovani eventu
==================================================
*/
void CO_ProcessEvents (void)
{
    SDL_Event   event;
    SDL_keysym  key;

    while (SDL_PollEvent (&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            key = event.key.keysym;
            g_inp.lastkey = key.sym;
            g_inp.lastkeychar = g_inp.keytrans[key.sym][(key.mod & KMOD_SHIFT) != 0];
            g_inp.key[key.sym] = true;
            g_app.con->keyevent (g_inp.lastkeychar);
            if (!g_app.con->isactive ())
                P_KeyEvent (g_inp.lastkeychar);
            break;
        case SDL_KEYUP:
            g_inp.key[event.key.keysym.sym] = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
                g_inp.mouse_but[0] = event.button.state == SDL_PRESSED;
            if (event.button.button == SDL_BUTTON_RIGHT)
                g_inp.mouse_but[1] = event.button.state == SDL_PRESSED;
            if (event.button.button == SDL_BUTTON_MIDDLE)
                g_inp.mouse_but[2] = event.button.state == SDL_PRESSED;
            break;
        case SDL_MOUSEMOTION:
            g_inp.mouse_pos[0] = event.motion.x;
            g_inp.mouse_pos[1] = event.motion.y;
            break;
        case SDL_QUIT:
            g_app.flags |= APP_FLAG_QUIT;
            break;
        case SDL_ACTIVEEVENT:
            g_app.active = event.active.gain == 1;
            P_ActiveEvent (event.active.gain == 1);
            break;
        }
    }
}

/*
==================================================
Deinicializace jadra
==================================================
*/
static void CO_DeInit (void)
{
    MY_UnregMem (g_app.con);
    delete g_app.con;
    CO_FontFree ();
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
    if (g_app.con != NULL)
    {
        g_app.con->printf (str);
        g_app.con->exec ("dump chyba.con");
    }

    P_DeInit ();
    VID_Shutdown ();
    CO_DeInit ();
}

/*
==================================================
Inicializace jadra
==================================================
*/
static void CO_Init (void)
{
    // Inicializace promenych jadra
    g_app.flags = APP_FLAG_NONE;
    g_app.con = NULL;

    // Inicializace me knihovny
    MY_Init ();
    MY_AddPck (APP_FILE_CORE_PCK);
    MY_ErrCallback (CO_ErrorHandler);

    // Inicializace SDL
    if (SDL_Init (SDL_INIT_VIDEO) != 0)
        MY_Err (MY_ErrDump ("%S: %s\n", MY_L("COSTR0006|Chyba pri inicializaci grafickeho modu"), SDL_GetError()));

    SDL_WM_SetCaption (APP_NAME, APP_NAME);
    SDL_WM_SetIcon (SDL_LoadBMP (APP_FILE_ICON), NULL);
    SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    // Inicializace jadra
    CO_FontLoad (APP_FILE_FONT);

    // Inicializace konzoly
#ifdef _DEBUG
    g_app.con = new con_c (CON_F_EXPAND | CON_F_REG_INFO);
#else
    g_app.con = new con_c (CON_F_EXPAND);
#endif
    MY_RegMem (g_app.con, sizeof (con_c));

    g_app.con->setfont (CO_FontGet ());
    CON_RegisterBasicCmd (g_app.con);

    // Inicializace vstupnich zarizeni
    g_inp.flags = APP_INP_INIT_NONE;
    CO_InpInit (APP_INP_INIT_KEY);
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
    VID_Init ();
    P_Init ();

    // Provedeni prikazu predanych z prikazove radky
    for (int i = 1; i < argc; i++)
        g_app.con->exec (argv[i]);

    // Predani kontroly aplikaci
    P_Main ();

    // Deinicializace
    P_DeInit ();
    VID_Shutdown ();   
    CO_DeInit ();

    return 0;
}
