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
Inicializace vstupnich systemu
==================================================
*/
void CO_JoystickScan()
{
    g_app.con->printf (MY_L("COSTR0004|\n===Inicializace vstupnich zarizeni===\n"));
	g_inp.joysticks.clear();

	if (SDL_WasInit(SDL_INIT_JOYSTICK))
	{
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	}
		
    if (SDL_InitSubSystem (SDL_INIT_JOYSTICK))
    {
        g_app.con->printf (MY_L("COSTR0002|...Nepodarilo se inicializovat subsystem pro joypady"));            
    }
    else
    {
        size_t joysticks = SDL_NumJoysticks();
        g_app.con->printf (MY_L("COSTR0003|...Nalezeno %d joypadu\n"), joysticks);

        for (size_t i = 0; i < joysticks; i++)
        {
            g_inp.joysticks.push_back(SDL_JoystickOpen(i));
            g_app.con->printf ("... * %s\n", SDL_JoystickName(g_inp.joysticks[i]));
        }
    }
}

/*
==================================================
Update vstupnich zarizeni (hlavne joypadu)
==================================================
*/
void CO_InpUpdate (void)
{
	if (!g_inp.joysticks.empty())
	{
		SDL_JoystickUpdate();
	}
}

