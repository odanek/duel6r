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

#include "project.h"

d6WORLD     d6World;
float       d6Sin[450];
myUINT      d6BackTex;
int         d6Wireframe = 0;
int         d6ZoomBlc = 13, d6ZoomMode = D6_ZM_FULL;
int         d6Playing = 3;
float       d6KeyWait = 0;
bool        d6ShowFps = false;
d6PLAYER_c  *d6Player[D6_MAX_PLAYERS];
int         d6Winner;
bool        d6InMenu = false, d6PlayMusic = false, d6FreqProblem = false;
int         d6FreqUsed = 0;
int         d6PlayerSkin[D6_MAX_PLAYERS][8];
int			d6AmmoRangeMin = 15, d6AmmoRangeMax = 15;

int D6_BlockZ (int x, int y)
{
    d6LEVEL *l = &d6World.Level;

    if (x < 0 || x >= l->SizeX || y < 0 || y >= l->SizeY)
        return D6_ANM_F_BLOCK;

    return d6World.Anm.Znak[l->Data[y * l->SizeX + x]];
}

int D6_BlockN (int x, int y)
{
    d6LEVEL *l = &d6World.Level;

    if (x < 0 || x >= l->SizeX || y < 0 || y >= l->SizeY)
        return 0;

    return l->Data[y * l->SizeX + x];
}

float D6_Sin (int a)
{
    return d6Sin[abs (a + 90) % 360];
}

float D6_Cos (int a)
{
    return d6Sin[abs (a) % 360];
}

void D6_ConSwitchW (con_c *con)
{
    d6Wireframe = 1 - d6Wireframe;
    if (d6Wireframe)
        con->printf (MY_L("APP00020|Vykreslovaci mod prepnut na dratovy\n"));
    else
        con->printf (MY_L("APP00021|Vykreslovaci mod prepnut na solidni\n"));
}

void D6_ConShowFps (con_c *con)
{
    d6ShowFps = !d6ShowFps;
    if (d6ShowFps)
        con->printf (MY_L("APP00022|Ukazatel fps zapnut\n"));
    else
        con->printf (MY_L("APP00023|Ukazatel fps vypnut\n"));
}

void D6_SetView (d6VIEW_s *w)
{
    glViewport (w->X, w->Y, w->Width, w->Height);
}

void D6_SetView (int x, int y, int width, int height)
{
    glViewport (x, y, width, height);
}

void D6_CheckWinner (void)
{
    int     pa = 0, la = -1, i;

    for (i = 0; i < d6Playing; i++)
        if (d6Player[i]->State.Life > 0)
        {
            pa++;
            la = i;
        }

    if (pa < 2)
    {
        d6Winner = 1;
        if (la != -1)
        {
            INFO_Add (la, MY_L("APP00024|Jsi vitez - stiskni ESC pro konec nebo F1 pro novou hru"));
            d6Player[la]->State.PH->Wins++;
        }
        else
            for (i = 0; i < d6Playing; i++)
                INFO_Add (i, MY_L("APP00025|Konec hry - bez viteze"));
    }
}

void D6_SetGLMode (int mode)
{
    if (mode == D6_GL_PERSPECTIVE)
    {
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity ();
        
        //gluPerspective (g_vid.gl_fov, g_vid.gl_aspect, g_vid.gl_nearclip, g_vid.gl_farclip);        
        float fovy = MM_D2R(g_vid.gl_fov) / 2;
        float f = cos(fovy) / sin(fovy);

        mat4_c<mval_t> p(0.0f);
        p(0,0) = f / g_vid.gl_aspect;
        p(1,1) = f;
        p(2,2) = (g_vid.gl_nearclip + g_vid.gl_farclip) / (g_vid.gl_nearclip - g_vid.gl_farclip);
        p(3,2) = (2 * g_vid.gl_nearclip * g_vid.gl_farclip) / (g_vid.gl_nearclip - g_vid.gl_farclip);
        p(2,3) = -1;        
        glMultMatrixf(&p(0,0));
        
        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity ();

        glEnable (GL_TEXTURE_2D);
        glEnable (GL_DEPTH_TEST);
        glEnable (GL_CULL_FACE);
    }
    else
    {
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity ();
        glOrtho (0, g_vid.cl_width, 0, g_vid.cl_height, -1, 1);
        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity ();

        glDisable (GL_CULL_FACE);
        glDisable (GL_TEXTURE_2D);
        glDisable (GL_DEPTH_TEST);
    }
}

void D6_GameLoop (void)
{
    CO_FpsSyncLoops (&RENDER_MoveScene, &RENDER_DrawScene);

    if (d6Winner == -1)
        D6_CheckWinner ();

    if (g_inp.key[SDLK_ESCAPE])
    {
        MENU_SavePH ();
        MENU_Start ();
    }

    if (!d6KeyWait)
    {
        // Restart hry pres F1
        if (g_inp.key[SDLK_F1])
        {
            MENU_SavePH ();
            MENU_Restart (g_inp.key[SDLK_LSHIFT]);
            return;
        }

        // Zmena split screen/full screen pres F2
        if (g_inp.key[SDLK_F2] && d6Playing < 5)
        {
            d6ZoomMode = (d6ZoomMode == D6_ZM_FULL) ? D6_ZM_SCROLL : D6_ZM_FULL;
            PLAYER_PrepareViews ();
            d6KeyWait = APP_FPS_SPEED;
        }

        // Vypnuti/zapnuti zobrazovani strucnych statistik ve hre
        if (g_inp.key[SDLK_F4])
        {
            INFO_ShowRankingSwap ();
            d6KeyWait = APP_FPS_SPEED;
        }

        // Ulozeni screenshotu
        if (g_inp.key[SDLK_F10])
        {
            UTIL_SaveScreenTga (1);
            d6KeyWait = APP_FPS_SPEED;
        }
    }

}
