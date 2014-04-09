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
#include "project.h"

static short    noAnim[4] = { 0, 50, 0, -1 };
extern short    wtAnim[2][24];
extern short    d6WpnAnm[D6_WEAPONS][16];
extern myUINT   *d6WpnTexture;
d6COLTEXTURE_c  d6ShtTexture;

void PLAYER_CheckWater (d6PLAYER_c *p, d6LEVEL *l, int *z)
{
    bool    w = false;
    int     X, Y, w_kind;

    X = (int) (p->State.X + 0.5f);
    Y = l->SizeY - (int) (p->State.Y - 0.2) - 1;
    p->State.Flags &= ~D6_FLAG_INWATER;

    if (X >= 0 && Y >= 0 && X < l->SizeX && Y < l->SizeY)
        if (D6_BlockZ (X, Y) == D6_ANM_F_WATER)
        {
            p->State.Flags |= D6_FLAG_INWATER;
            if ((p->State.Air -= g_app.frame_interval) < 0)
            {
                p->State.Air = 0;
                p->Hit (g_app.frame_interval, NULL, false);
            }
            return;
        }

    if (p->State.Air < D6_MAX_AIR)
        p->State.Air++;

    Y = l->SizeY - (int) (p->State.Y - 0.9) - 1;

    if (X >= 0 && Y >= 0 && X < l->SizeX && Y < l->SizeY)
        if (D6_BlockZ (X, Y) == D6_ANM_F_WATER)
        {
            w = true;
            w_kind = D6_BlockN (X, Y) == 4 ? 0 : 1;
        }

    if (w && !p->State.InWater)
    {
        ANM_Add (p->State.X, p->State.Y, 0.5f, 1, ANM_LOOP_ONEKILL, 0, wtAnim[w_kind], d6WpnTexture, false);
        SOUND_PlaySample (D6_SND_WATER);
    }

    p->State.InWater = w;
}

void PLAYER_UpdateAll (void)
{
    d6PLAYER_c  *p;
    d6LEVEL     *l;
    int         *z;
    int         i;

    l = &d6World.Level;
    z = d6World.Anm.Znak;

    for (i = 0; i < d6Playing; i++)
    {
        p = d6Player[i];

        PLAYER_CheckWater (p, l, z);
        if (!p->IsDead())
            BONUS_Check (&p->State);

        p->CheckKeys ();
        p->MakeMove ();
        p->SetAnm ();
        p->UpdateCam ();

        // Move intervals
        if (p->State.SI > 0)
            if ((p->State.SI -= g_app.frame_interval) <= 0)
                p->State.SI = 0;

        if (p->State.BD > 0)
            if ((p->State.BD -= g_app.frame_interval) <= 0)
            {
                if (p->State.Bonus == D6_BONUS_INVIS)
                {
                    ANM_SetAlpha (p->State.A, 1);
                    ANM_SetAlpha (p->State.GA, 1);
                }
                p->State.Bonus = 0;
                p->State.BD = 0;
            }

        if (p->State.SD > 0 )
            if ((p->State.SD -= g_app.frame_interval) <= 0)
            {
                ANM_SetTexture (p->State.A, p->Texture);
                p->State.SD = 0;
            }
    }
}

static void PLAYER_FindStart (float *x, float *y)
{
    d6LEVEL *l = &d6World.Level;
    int     X, Y, y2, s = 1;

    while (s)
    {
        X = rand () % l->SizeX;
        Y = rand () % l->SizeY;
        s = 1;

        if (D6_BlockZ (X, Y) != D6_ANM_F_BLOCK)
        {
            y2 = Y - 1;

            while (++y2 < l->SizeY)
            {
                if (D6_BlockZ (X, y2) == D6_ANM_F_WATER)
                    break;
                if (D6_BlockZ (X, y2) == D6_ANM_F_BLOCK)
                {
                    *x = (float) X;
                    *y = (float) (l->SizeY - Y) + 0.0001f;
                    return;
                }
            }
        }
    }
}

void PLAYER_View (int i, int x, int y)
{
    int w = g_vid.cl_width / 2 - 4, h = g_vid.cl_height / 2 - 4;

    d6Player[i]->SetView (x, y, w, h);
    d6Player[i]->State.IBP[0] = x + w / 2 - 76;
    d6Player[i]->State.IBP[1] = y + 30;
}

void PLAYER_PrepareViews (void)
{
    int     i, xShift = (g_vid.cl_width / 4) / 2 - 70;

    for (i = 0; i < d6Playing; i++)
    {
        d6Player[i]->PrepareCam ();
        d6Player[i]->UpdateCam ();
    }

    if (d6ZoomMode == D6_ZM_FULL)
    {
        for (i = 0; i < d6Playing; i++)
        {
            d6Player[i]->SetView (0, 0, g_vid.cl_width, g_vid.cl_height);

            if (i < 4)
            {
                d6Player[i]->State.IBP[0] = (g_vid.cl_width / 4) * i + xShift;
                d6Player[i]->State.IBP[1] = 30;
            }
            else
            {
                d6Player[i]->State.IBP[0] = (g_vid.cl_width / 4) * (i - 4) + xShift;
                d6Player[i]->State.IBP[1] = g_vid.cl_height - 7;
            }
        }

        return;
    }

    if (d6Playing == 2)
    {
        PLAYER_View (0, g_vid.cl_width / 4 + 2, 2);
        PLAYER_View (1, g_vid.cl_width / 4 + 2, g_vid.cl_height / 2 + 2);
    }

    if (d6Playing == 3)
    {
        PLAYER_View (0, 2, 2);
        PLAYER_View (1, g_vid.cl_width / 2 + 2, 2);
        PLAYER_View (2, g_vid.cl_width / 4 + 2, g_vid.cl_height / 2 + 2);
    }

    if (d6Playing == 4)
    {
        PLAYER_View (0, 2, 2);
        PLAYER_View (1, g_vid.cl_width / 2 + 2, 2);
        PLAYER_View (2, 2, g_vid.cl_height / 2 + 2);
        PLAYER_View (3, g_vid.cl_width / 2 + 2, g_vid.cl_height / 2 + 2);
    }
}

void PLAYER_PrepareAll (void)
{
    d6PLSTATE_s *s;
    int         i;

    for (i = 0; i < d6Playing; i++)
    {
        s = &d6Player[i]->State;

        PLAYER_FindStart (&s->X, &s->Y);
        s->A = ANM_Add (s->X, s->Y, 0.5f, 1, ANM_LOOP_FOREVER, 0, noAnim, d6Player[i]->Texture, false);
        s->GN = WPN_GetRandomWeapon();
        s->GA = ANM_Add (s->X, s->Y, 0.5f, 1, ANM_LOOP_ONESTOP, 0, d6WpnAnm[s->GN], d6WpnTexture, false);
        ANM_SetAnm (s->GA, 6);
        s->Flags = D6_FLAG_NONE;
        s->Speed = 0;
        s->O = 0;
        s->J = 0;
        s->SI = 0;
        s->Life = D6_MAX_LIFE;
        s->Air = D6_MAX_AIR;
        s->Ammo = d6AmmoRangeMin + rand() % (d6AmmoRangeMax - d6AmmoRangeMin + 1);
        s->Elev = -1;
        s->Bonus = 0;
        s->BD = 0;
        s->SD = 0;
        s->PH->Games++;
        s->InWater = false;
    }

    PLAYER_PrepareViews ();
}

void PLAYER_SetColTex (int i, int gn)
{
    if (gn == D6_COL_WPN_SHT)
    {
        d6Player[i]->State.SD = float (APP_FPS_SPEED * (10 + rand () % 5));
        ANM_SetTexture (d6Player[i]->State.A, d6ShtTexture.GetTexPtr());
    }
}
