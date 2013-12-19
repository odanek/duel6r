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
Popis: Fps - vypocet a synchronizace
*/

#include <time.h>
#include "co_core.h"

/*
==================================================
CO_FpsSyncLoops

Nastavi promenou frame_interval ktera udava jak dlouho
trval predchozi update. Rychlosti tomu odpovidajici
jsou potom upraveny promene v procedure move.
Nasledne je vse vykresleno na obrazovku.
Zaroven provadi vypocet fps.
==================================================
*/
void CO_FpsSyncLoops (void (*move) (void), void (*draw) (void))
{
    static unsigned long    cur_time = 0, last_fps_time = 0;
    static int              fps_counter = 0;
    unsigned long           last_time;

    last_time = cur_time;
    cur_time = SDL_GetTicks ();

    // Spocitej fps
    if (cur_time - last_fps_time >= 1000)
    {
        g_app.fps = fps_counter / (cur_time - last_fps_time);
        last_fps_time = cur_time;
        fps_counter = 0;
    }

    // Synchronizuj
    if (cur_time - last_time < 70)
    {
        g_app.frame_interval = (APP_FPS_SPEED * (cur_time - last_time)) * 0.001f;
        move ();
    }

    fps_counter += 1000;
    draw ();
    VID_SwapBuffers ();
}
