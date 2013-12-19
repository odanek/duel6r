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

#ifndef __ANIM_H
#define __ANIM_H

#define ANM_MAX             100

#define ANM_FLAG_NONE       0x00
#define ANM_FLAG_USED       0x01
#define ANM_FLAG_DRAW       0x02
#define ANM_FLAG_TRANS      0x04
#define ANM_FLAG_FINISHED   0x08
#define ANM_FLAG_NO_DEPTH   0x10
#define ANM_FLAG_ALL        0x1F

typedef d6VERTEX            anmVERTEX_s;

enum
{
    ANM_LOOP_FOREVER,
    ANM_LOOP_ONESTOP,
    ANM_LOOP_ONEKILL
};

struct anmANM_s
{
    int     I;      // Offset in anm array
    int     Pos;    // Current animation
    float   Del;    // Delay to next animation
    int     Sp;     // Speed of animation
    short   *Pl;    // Source array of animations and delays
    myUINT  *Tex;   // Texture array
    int     Loop;   // Type of looping
    int     O;      // Current orientation
    int     Flags;  // Flags
    float   X;      // Copy from vertex array for speed up
    float   Y;
    float   Z;
    float   Grow;   // Grow factor for explosions
    float   Alpha;  // Transparency ratio
};

void    ANM_Init        (anmVERTEX_s *first, int num);
int     ANM_Add         (float X, float Y, float Z, int S, int loop, int O, short *pl, myUINT *tex, bool T);
void    ANM_ReSet       (int i, float X, float Y, int loop, int O, short *pl);
void    ANM_SetAnm      (int i, int p);
void    ANM_Grow        (int i, float grow);
int     ANM_Pict        (int num);
void    ANM_MoveAll     (void);
void    ANM_DrawAll     (void);
void    ANM_AddFlags    (int i, int f);
void    ANM_RemoveFlags (int i, int f);
bool    ANM_CheckFlags  (int i, int f);
void    ANM_SetAlpha    (int i, float a);
void    ANM_SetTexture  (int i, myUINT *tex);

#endif
