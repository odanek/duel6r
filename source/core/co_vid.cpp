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
Popis: Obsluha grafickeho rezimu
*/

#include "co_core.h"

/*
==================================================
Swap buffers - prohodi buffery
==================================================
*/
void VID_SwapBuffers (void)
{
    if (!g_vid.inited)
        return;

    if (g_app.con != NULL && g_app.con->isactive ())
        g_app.con->blit (g_vid.cl_width, g_vid.cl_height);

    SDL_GL_SwapBuffers( );
}

/*
==================================================
Inicializace videa
==================================================
*/
void VID_Init (void)
{
    g_vid.inited = false;
}

/*
==================================================
Nastaveni konkretniho grafickeho modu
==================================================
*/
void VID_SetMode (int w, int h, int bpp, int aa, bool full)
{
    int flags = 0, val[7];

    g_vid.cl_width = w;
    g_vid.cl_height = h;
    g_vid.gl_aspect = (float) g_vid.cl_width / (float) g_vid.cl_height;

    g_app.con->printf (MY_L("COSTR0007|...Testuji zmenu grafickeho modu: %dx%dx%d\n"), w, h, bpp);

    flags = SDL_OPENGL;
    if (full)
        flags |= SDL_FULLSCREEN;

    //SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 1);
    //SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 1);
    //SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 1);
    //SDL_GL_SetAttribute (SDL_GL_ALPHA_SIZE, 0);
    //SDL_GL_SetAttribute (SDL_GL_STENCIL_SIZE, 0);
    //SDL_GL_SetAttribute (SDL_GL_ACCUM_RED_SIZE, 0);
    //SDL_GL_SetAttribute (SDL_GL_ACCUM_GREEN_SIZE, 0);
    //SDL_GL_SetAttribute (SDL_GL_ACCUM_BLUE_SIZE, 0);
    //SDL_GL_SetAttribute (SDL_GL_ACCUM_ALPHA_SIZE, 0);
    //SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute (SDL_GL_BUFFER_SIZE, bpp);
    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);

    if (aa > 1 && aa < 8 && aa % 2 == 0)
    {
        g_app.con->printf ("...Anti-aliasing: %dx\n", aa);
        g_vid.gl_aa = aa;
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, aa);
    }

    // Otestuj pri jakem bpp je mozne vyhovet parametrum
    bpp = SDL_VideoModeOK (w, h, bpp, flags);
    if (!bpp)
        MY_Err (MY_ErrDump ("%s", MY_L("COSTR0008|Nelze nastavit graficky rezim")));

    // Nastav novy graficky rezim
    g_vid.gl_bpp = bpp;
    g_app.screen_surf = SDL_SetVideoMode (w, h, bpp, flags);
    if (g_app.screen_surf == NULL)
        MY_Err (MY_ErrDump ("%s: %s\n", MY_L("COSTR0009|Nelze nastavit graficky rezim"), SDL_GetError()));

    g_app.con->printf (MY_L("COSTR0010|...Nastavuji novy graficky mod: %dx%dx%d\n"),
                       g_app.screen_surf->w,
                       g_app.screen_surf->h,
                       g_app.screen_surf->format->BitsPerPixel);

    // Kontrola vyslednych pouzitych parametru
    SDL_GL_GetAttribute (SDL_GL_RED_SIZE, &val[0]);
    SDL_GL_GetAttribute (SDL_GL_GREEN_SIZE, &val[1]);
    SDL_GL_GetAttribute (SDL_GL_BLUE_SIZE, &val[2]);
    SDL_GL_GetAttribute (SDL_GL_BUFFER_SIZE, &val[3]);
    SDL_GL_GetAttribute (SDL_GL_DEPTH_SIZE, &val[4]);
    SDL_GL_GetAttribute (SDL_GL_ALPHA_SIZE, &val[5]);
    SDL_GL_GetAttribute (SDL_GL_STENCIL_SIZE, &val[6]);

    g_app.con->printf (MY_L("COSTR0011|...RGB (%d, %d, %d)\n"), val[0], val[1], val[2]);
    g_app.con->printf (MY_L("COSTR0012|...Barva (%d), Z-buffer (%d), Alfa kanal (%d), Stencil (%d)\n"),
                       val[3], val[4], val[5], val[6]);

    SDL_ShowCursor (SDL_DISABLE);

    g_vid.inited = true;
}

/*
==================================================
Ukonceni grafickeho rezimu
==================================================
*/
void VID_Shutdown (void)
{
    g_vid.inited = false;
}
