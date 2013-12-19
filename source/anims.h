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

#ifndef __ANIMS_H
#define __ANIMS_H

// Duel6 animations

short   d6SAnim[22] =
        {0, 200, 1, 30, 0, 30, 2, 30, 0, 90, 3, 15, 4, 15, 3, 15, 4, 15, 3, 30, -1, 0 },
        d6WAnim[10] =
        {6, 5, 5, 5, 6, 5, 7, 5, -1, 0 },
        d6JAnim[4] =
        {8, 50, -1, 0},
        d6DAnim[18] =
        {9, 300, 10, 60, 9, 60, 11, 60, 9, 150, 12, 60, 9, 60, 12, 60, -1, 0},
        d6LAnim[16] =
        {13, 10, 14, 10, 15, 10, 16, 10, 17, 10, 18, 10, 19, 10, -1, 0 },
        d6NAnim[4] =
        {25, 100, -1, 0 },
        d6PAnim[] =
        { 0, 10, 20, 10, 21, 10, 22, 10, 23, 10, 24, 10, 23, 10, 22, 10, 21, 10,  0, 10, -1, 0 };

short   d6WpnAnm[D6_WEAPONS][16] =
{
    { 25, 5, 26, 5, 27, 5, 28, 5, 29, 5, 30, 5, 24, 50, -1, 0 },
    { 32, 5, 33, 5, 34, 5, 35, 5, 36, 5, 37, 5, 31, 50, -1, 0 },
    { 39, 5, 40, 5, 41, 5, 42, 5, 38, 5, 38, 5, 38, 50, -1, 0 },
    { 44, 5, 45, 5, 46, 5, 45, 5, 44, 5, 43, 5, 43, 50, -1, 0 },
    { 48, 5, 47, 5, 47, 5, 47, 5, 47, 5, 47, 5, 47, 50, -1, 0 },
    { 50, 5, 49, 5, 49, 5, 49, 5, 49, 5, 49, 5, 49, 50, -1, 0 },
    { 52, 5, 51, 5, 51, 5, 51, 5, 51, 5, 51, 5, 51, 50, -1, 0 },
    { 54, 5, 55, 5, 56, 5, 53, 5, 53, 5, 53, 5, 53, 50, -1, 0 },
    { 69, 5, 69, 5, 69, 5, 69, 5, 69, 5, 69, 5, 69, 50, -1, 0 },
    { 71, 10, 72, 10, 70, 5, 70, 5, 70, 5, 70, 5, 70, 50, -1, 0 },
    { 75, 10, 76, 10, 74, 5, 74, 5, 74, 5, 74, 5, 74, 50, -1, 0 },
    { 81, 10, 80, 10, 80, 5, 80, 5, 80, 5, 80, 5, 80, 50, -1, 0 },
    { 94, 8, 95, 8, 96, 8, 95, 8, 94, 8, 94, 8, 94, 8, -1, 0 },
    { 100, 8, 101, 8, 100, 8, 101, 8, 100, 8, 100, 8, 100, 8, -1, 0 },
    { 105, 5, 106, 5, 105, 5, 107, 5, 105, 5, 105, 5, 105, 5, -1, 0 },
    { 109, 5, 110, 5, 110, 5, 111, 5, 111, 5, 109, 5, 109, 5, -1, 0 },
    { 114, 5, 114, 5, 114, 5, 114, 5, 114, 5, 114, 5, 114, 5, -1, 0 }
};

short   d6ShotAnm[D6_WEAPONS][18] =
{
    { 0, 50, -1, 0 },
    { 1, 10,  2, 10, -1, 0 },
    { 3, 10,  4, 10, -1, 0 },
    { 5, 50, -1, 0 },
    { 6, 50, -1, 0 },
    { 7, 50, -1, 0 },
    { 8, 50, -1, 0 },
    { 9, 8,  10, 8,  11, 8, 12, 8, 13, 8, 12, 8, 11, 8, 10, 8, -1, 0 },
    { 0, 50, -1, 0 },
    { 73, 50, -1, 0 },
    { 77, 50, -1, 0 },
    { 82, 50, -1, 0 },
    { 97, 10, 98, 10, 99, 10, 98, 10, -1, 0 },
    { 102, 5,  103, 5, -1, 0 },
    { 108, 50, -1, 0 },
    { 112, 50, -1, 0 },
    { 115, 10, 116, 10, 117, 10, 116, 10, -1, 0 }
};

short   d6BoomAnm[D6_WEAPONS][14] =
{
    { 14, 5,  15, 5, 14, 5, 15, 5, 14, 5, 15, 5, -1, 0 },
    { 16, 5,  17, 5, 16, 5, 17, 5, 16, 5, 17, 5, -1, 0 },
    { 18, 5,  19, 5, 18, 5, 19, 5, 18, 5, 19, 5, -1, 0 },
    { 14, 5,  15, 5, 14, 5, 15, 5, 14, 5, 15, 5, -1, 0 },
    { 20, 5,  21, 5, 20, 5, 21, 5, 20, 5, 21, 5, -1, 0 },
    { 22, 10, -1, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0 },
    { 23, 10, -1, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0 },
    { 16, 5,  17, 5, 16, 5, 17, 5, 16, 5, 17, 5, -1, 0 },
    { 14, 5,  15, 5, 14, 5, 15, 5, 14, 5, 15, 5, -1, 0 },
    { 22, 10, -1, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0 },
    { 78, 5,  79, 5, 78, 5, 79, 5, 78, 5, 79, 5, -1, 0 },
    { 16, 5,  17, 5, 16, 5, 17, 5, 16, 5, 17, 5, -1, 0 },
    { 20, 5,  21, 5, 20, 5, 21, 5, 20, 5, 21, 5, -1, 0 },
    { 104, 10, -1, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0 },
    { 22, 10, -1, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0 },
    { 22, 10, -1, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0 },
    { 113, 10, -1, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0 }
};

short   wtAnim[2][24] =
{
    { 57, 5, 58, 5, 59, 5, 60, 5, 61, 5, 62, 5, 63, 5, 64, 5, 66, 5, 67, 5, -1, 0 },
    { 83, 5, 84, 5, 85, 5, 86, 5, 87, 5, 88, 5, 89, 5, 90, 5, 91, 5, 92, 5, -1, 0 }
};

// Weapons definitions
d6WEAPONDEF_s   d6WpnDef[D6_WEAPONS] =
{
        // Spd  Krev    ExplC       Boom    Uber    Nabiti  Jmeno                    Shot sound        Boom sound      Exp grow
        { 0.15f,true,   0x00,       0,      30,     60,     "APP00095|pistole",      D6_SND_SHOT_PI,   -1,             0 },
        { 0.1f, false,  0xFF0000,   3,      100,    200,    "APP00096|bazooka",      D6_SND_SHOT_BZ,   D6_SND_BOOM_BZ, 0.01f },
        { 0.2f, false,  0x00FFFF,   0,      100,    250,    "APP00097|bleskomet",    D6_SND_SHOT_BL,   D6_SND_BOOM_BL, 0 },
        { 0.15f,true,   0x00,       0,      60,     140,    "APP00098|brokovnice",   D6_SND_SHOT_SG,   -1,             0 },
        { 0.2f, true,   0x00,       0,      25,     20,     "APP00099|plasma",       D6_SND_SHOT_PL,   -1,             0 },
        { 0.25f,true,   0x00,       0,      35,     15,     "APP00100|laser",        D6_SND_SHOT_LS,   -1,             0 },
        { 0.15f,true,   0x00,       0,      40,     40,     "APP00101|kulomet",      D6_SND_SHOT_KM,   -1,             0 },
        { 0.1f, false,  0xFFFF00,   4,      200,    400,    "APP00102|trojzubec",    D6_SND_SHOT_TZ,   D6_SND_BOOM_BZ, 0.04f },
        { 0.18f,true,   0x00,       0,      15,     16,     "APP00103|uzi",          D6_SND_SHOT_PI,   -1,             0 },
        { 0.2f, true,   0x00,       0,      25,     50,     "APP00104|luk",          D6_SND_SHOT_LK,   -1,             0 },
        { 0.13f,true,   0x00,       0,      50,     80,     "APP00105|slizomet",     D6_SND_SHOT_SM,   -1,             0 },
        { 0.2f, true,   0xFFFF00,   2,      80,     210,    "APP00106|dvojity laser",D6_SND_SHOT_LS,   D6_SND_BOOM_BZ, 0.005f },
        { 0.07f,false,  0xFF00FF,   0,      100,    190,    "APP00107|polibek smrti",D6_SND_SHOT_KD1,  D6_SND_SHOT_KD2,0.01f },
        { 0.08f,false,  0x00,       0,      20,     40,     "APP00108|sprej",        D6_SND_SHOT_SPRAY,-1,             0 },
        { 0.11f,true,   0x00,       0,      15,     50,     "APP00109|prak",         D6_SND_SHOT_SLING,-1,             0 },
        { 0.13f,true,   0x00,       0,      10,     55,     "APP00110|spuntovka",    D6_SND_SHOT_SPUNT,-1,             0 },
        { 0.09f,false,  0x00,       2,      0,      120,    "APP00112|lejnomet",     D6_SND_SHOT_SHT,  -1,             0.04f }
};

#endif
