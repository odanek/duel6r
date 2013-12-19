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

struct d6WATERVERTEX
{
    int                     X;
    float                   Y;
    d6VERTEX                *V;
    struct d6WATERVERTEX    *Next;
};

static d6WATERVERTEX    *d6WV = NULL;
static float            d6WAngle = 0;

static void WATER_AddVertex (int n)
{
    d6WATERVERTEX   *w;

    w = D6_MALLOC (d6WATERVERTEX, 1);

    w->X = (int) d6World.Vertex[n].X;
    w->Y = d6World.Vertex[n].Y - D6_WAVE_HEIGHT;
    w->V = &d6World.Vertex[n];
    w->Next = d6WV;
    d6WV = w;
}

void WATER_Free (void)
{
    d6WATERVERTEX   *w;

    while (d6WV != NULL)
    {
        w = d6WV->Next;
        MY_Free (d6WV);
        d6WV = w;
    }
}

void WATER_Move (void)
{
    float           s = 0;
    d6WATERVERTEX   *w = d6WV;
    int             oldX = -1;

    d6WAngle += 2 * g_app.frame_interval;
    if (d6WAngle >= 360)
        d6WAngle -= 360;

    while (w != NULL)
    {
        if (w->X != oldX)
        {
            s = D6_Sin (int(d6WAngle) + 60 * w->X) * D6_WAVE_HEIGHT;
            oldX = w->X;
        }

        w->V->Y = w->Y + s;
        w = w->Next;
    }
}

void WATER_Build (void)
{
    int         i, j, s, e;
    d6VERTEX    *v = d6World.Vertex;

    WATER_Free ();

    s = (d6World.Blocks + d6World.Sprites) << 2;
    e = (d6World.Blocks + d6World.Sprites + d6World.Waters) << 2;

    g_app.con->printf (MY_L("APP00083|...Sestavuji water-list\n"));
    for (i = 0; i <= d6World.Level.SizeX; i++)
        for (j = s; j < e; j++)
            if (v[j].Flags == D6_FLAG_FLOW && (int) v[j].X == i)
                WATER_AddVertex (j);
}
