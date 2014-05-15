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
Popis: Obsluha vstupnih zarizeni - klavesnice, mys, joypad
*/

#include "co_core.h"

/*
==================================================
Inicializace prekladu klaves
==================================================
*/
static void CO_InpInitKeyTranslation (void)
{
    int     i;

    for (i = 0; i < APP_KEY_LAST; i++)
    {
        g_inp.keytrans[i][0] = g_inp.keytrans[i][1] = i;
        g_inp.key[i] = false;
    }
    for (i = SDLK_a; i <= SDLK_z; i++)
        g_inp.keytrans[i][1] = 'A' + i - SDLK_a;

    g_inp.keytrans[SDLK_0][1] = ')';
    g_inp.keytrans[SDLK_1][1] = '!';
    g_inp.keytrans[SDLK_2][1] = '@';
    g_inp.keytrans[SDLK_3][1] = '#';
    g_inp.keytrans[SDLK_4][1] = '$';
    g_inp.keytrans[SDLK_5][1] = '%';
    g_inp.keytrans[SDLK_6][1] = '^';
    g_inp.keytrans[SDLK_7][1] = '&';
    g_inp.keytrans[SDLK_8][1] = '*';
    g_inp.keytrans[SDLK_9][1] = '(';

    g_inp.keytrans[SDLK_MINUS][1] = '_';
    g_inp.keytrans[SDLK_EQUALS][1] = '+';
    g_inp.keytrans[SDLK_LEFTBRACKET][1] = '{';
    g_inp.keytrans[SDLK_RIGHTBRACKET][1] = '}';
    g_inp.keytrans[SDLK_SEMICOLON][1] = ':';
    g_inp.keytrans[SDLK_QUOTE][1] = '"';
    g_inp.keytrans[SDLK_BACKSLASH][1] = '|';
    g_inp.keytrans[SDLK_COMMA][1] = '<';
    g_inp.keytrans[SDLK_PERIOD][1] = '>';
    g_inp.keytrans[SDLK_SLASH][1] = '?';

    g_inp.keytrans[SDL_SCANCODE_KP_0][0] = g_inp.keytrans[SDL_SCANCODE_KP_0][1] = '0';
    g_inp.keytrans[SDL_SCANCODE_KP_1][0] = g_inp.keytrans[SDL_SCANCODE_KP_1][1] = '1';
    g_inp.keytrans[SDL_SCANCODE_KP_2][0] = g_inp.keytrans[SDL_SCANCODE_KP_2][1] = '2';
    g_inp.keytrans[SDL_SCANCODE_KP_3][0] = g_inp.keytrans[SDL_SCANCODE_KP_3][1] = '3';
    g_inp.keytrans[SDL_SCANCODE_KP_4][0] = g_inp.keytrans[SDL_SCANCODE_KP_4][1] = '4';
    g_inp.keytrans[SDL_SCANCODE_KP_5][0] = g_inp.keytrans[SDL_SCANCODE_KP_5][1] = '5';
    g_inp.keytrans[SDL_SCANCODE_KP_6][0] = g_inp.keytrans[SDL_SCANCODE_KP_6][1] = '6';
    g_inp.keytrans[SDL_SCANCODE_KP_7][0] = g_inp.keytrans[SDL_SCANCODE_KP_7][1] = '7';
    g_inp.keytrans[SDL_SCANCODE_KP_8][0] = g_inp.keytrans[SDL_SCANCODE_KP_8][1] = '8';
    g_inp.keytrans[SDL_SCANCODE_KP_9][0] = g_inp.keytrans[SDL_SCANCODE_KP_9][1] = '9';

    g_inp.keytrans[SDL_SCANCODE_KP_PERIOD][0] = g_inp.keytrans[SDL_SCANCODE_KP_PERIOD][1] = ',';
    g_inp.keytrans[SDL_SCANCODE_KP_DIVIDE][0] = g_inp.keytrans[SDL_SCANCODE_KP_DIVIDE][1] = '/';
    g_inp.keytrans[SDL_SCANCODE_KP_MULTIPLY][0] = g_inp.keytrans[SDL_SCANCODE_KP_MULTIPLY][1] = '*';
    g_inp.keytrans[SDL_SCANCODE_KP_MINUS][0] = g_inp.keytrans[SDL_SCANCODE_KP_MINUS][1] = '-';
    g_inp.keytrans[SDL_SCANCODE_KP_PLUS][0] = g_inp.keytrans[SDL_SCANCODE_KP_PLUS][1] = '+';
    g_inp.keytrans[SDL_SCANCODE_KP_EQUALS][0] = g_inp.keytrans[SDL_SCANCODE_KP_EQUALS][1] = '=';

    g_inp.keytrans[SDL_SCANCODE_KP_ENTER][0] = g_inp.keytrans[SDL_SCANCODE_KP_ENTER][1] = SDLK_RETURN;
}

/*
==================================================
Vraci posledni stisknutou klavesu
==================================================
*/
int CO_InpGetKey (bool trans)
{
    int key = trans ? g_inp.lastkeychar : g_inp.lastkey;
    g_inp.lastkey = 0;
    g_inp.lastkeychar = 0;
    return key;
}

/*
==================================================
Inicializace vstupnich systemu
==================================================
*/
void CO_InpInit (int flags)
{
    int     i;

    // Pokud jeste nebyly inicializovany klavesy je treba to provest
    if (g_inp.flags == APP_INP_INIT_NONE)
        flags |= APP_INP_INIT_KEY;

    g_app.con->printf (MY_L("COSTR0004|\n===Inicializace vstupnich zarizeni===\n"));

    // -----Klavesy-----
    if (flags & APP_INP_INIT_KEY)
    {
        // Key translation
        g_app.con->printf (MY_L("COSTR0005|...Spusteni prekladu klaves\n"));
        CO_InpInitKeyTranslation ();
    }

    // -----Joypady-----
    if (flags & APP_INP_INIT_JOY)
    {
        if (SDL_WasInit (SDL_INIT_JOYSTICK))
            SDL_QuitSubSystem (SDL_INIT_JOYSTICK);

        if (SDL_InitSubSystem (SDL_INIT_JOYSTICK))
        {
            g_app.con->printf (MY_L("COSTR0002|...Nepodarilo se inicializovat subsystem pro joypady"));
            g_inp.joy_num = 0;
        }
        else
        {
            g_inp.joy_num = SDL_NumJoysticks ();

            // Momentalne podporovano maximalne 8 joyapdu
            if (g_inp.joy_num > 8)
                g_inp.joy_num = 8;

            g_app.con->printf (MY_L("COSTR0003|...Nalezeno %d joypadu\n"), g_inp.joy_num);

            for (i = 0; i < g_inp.joy_num; i++)
            {
                g_inp.joy_dev[i] = SDL_JoystickOpen (i);
                g_app.con->printf ("... * %s\n", SDL_JoystickName(g_inp.joy_dev[i]));
            }
        }
    }

    // Aktualizuj seznam inicializovanych zarizeni
    g_inp.flags |= flags;

    // Pokud nejsou inicializovane joypady nastav pocet na 0
    if (!(g_inp.flags & APP_INP_INIT_JOY))
        g_inp.joy_num = 0;
}

/*
==================================================
Update vstupnich zarizeni (hlavne joypadu)
==================================================
*/
void CO_InpUpdate (void)
{
    if ((g_inp.flags & APP_INP_INIT_JOY) != 0 && g_inp.joy_num > 0)
        SDL_JoystickUpdate ();
}

/*
==================================================
Zjisti zda je dana klavesa na danem vstupnim
zarizeni stisknuta
==================================================
*/
bool CO_InpIsPressed (int code)
{
    int     pos, dev = code & 0x0ff000, key = code & 0x0fff;

    switch (dev)
    {
    case APP_INP_KEY:
        if (key >= APP_KEY_LAST)
            return false;
        else
            return g_inp.key[key];
    case APP_INP_MOUSE:
        if (key > 2)
            return false;
        else
            return g_inp.mouse_but[key];
    }

    if (dev >= APP_INP_JOY1 && dev <= APP_INP_JOY8)
    {
        dev = (dev >> 12) - (APP_INP_JOY1 >> 12);

        if (dev >= g_inp.joy_num)
            return false;

        if (key == APP_INP_JOY_LEFT || key == APP_INP_JOY_RIGHT)
        {
            pos = SDL_JoystickGetAxis (g_inp.joy_dev[dev], 0);
            if (pos < -1000 && key == APP_INP_JOY_LEFT)
                return true;
            if (pos > 1000 && key == APP_INP_JOY_RIGHT)
                return true;
            return false;
        }
        if (key == APP_INP_JOY_UP || key == APP_INP_JOY_DOWN)
        {
            pos = SDL_JoystickGetAxis (g_inp.joy_dev[dev], 1);
            if (pos > 1000 && key == APP_INP_JOY_DOWN)
                return true;
            if (pos < -1000 && key == APP_INP_JOY_UP)
                return true;
            return false;
        }
        if (key >= APP_INP_JOY_BUT1 || key <= APP_INP_JOY_BUT8)
            return (SDL_JoystickGetButton (g_inp.joy_dev[dev], key - APP_INP_JOY_BUT1) == 1);
    }

    return false;
}
