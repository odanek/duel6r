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
Popis: Hlavni hlavickovy soubor jadra
*/

#ifndef __CO_CORE_H
#define __CO_CORE_H

/* SDL */
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

/* Moje standardni knihovny */
#include "../mylib/mylib.h"
#include "../console/console.h"

/* Makra a definice */
#define APP_VERSION         "3.8.14 (Maca edition)"
#define APP_NAME            "Duel 6 Reloaded"
#define APP_FILE_ICON       "data/duel6_icon.bmp"
#define APP_FILE_CORE_PCK   "data/duel6.pck"
#define APP_FILE_FONT       "data/duel6.fon"
#define APP_FPS_SPEED       61

/* Flagy */
#define APP_FLAG_NONE       0
#define APP_FLAG_QUIT       1

/*
==================================================
Datove struktury
==================================================
*/
struct app_s
{
    int         fps;                // Aktualni pocet obrazku za sekundu
    float       frame_interval;     // Cas straveny vykreslenim jednoho framu
    bool        flags;              // Flagy aplikace
    con_c       *con;               // Konzole
    bool        active;             // Je aplikace prave aktivni (ma focus?)
    SDL_Surface *screen_surf;       // SDL_surface pro obrazovku
    myBYTE      *font;              // Ukazatel na font
};

struct appVid_s
{
    bool        inited;             // Video inited?

    int         cl_width;           // Client width
    int         cl_height;          // Client height

    int         gl_bpp;             // Current bits per pixel
    int         gl_aa;              // Current anti alliasing samples
    float       gl_farclip;         // OpenGL far clipping plane
    float       gl_nearclip;        // OpenGL far clipping plane
    float       gl_fov;             // OpenGL field of view
    float       gl_aspect;          // OpenGL aspect - pokud je 0 tak dopocitat jako width/height
    float       gl_cdepth;          // OpenGL color depth - 0 je default
    float       gl_zdepth;          // OpenGL z-buffer depth
};

struct appInp_s
{
    int             flags;
    int             keytrans[SDLK_LAST][2];
    bool            key[SDLK_LAST];
    int             lastkey;
    int             lastkeychar;
    int             joy_num;
    SDL_Joystick    *joy_dev[8];    // Maximalne 8 joypadu
    bool            mouse_but[3];
    int             mouse_pos[2];
};

extern app_s    g_app;
extern appVid_s g_vid;
extern appInp_s g_inp;

/*
==================================================
co_main.cpp
==================================================
*/
void            CO_ProcessEvents    (void);

/*
==================================================
co_input.cpp
==================================================
*/
#define APP_INP_INIT_NONE     0x0000
#define APP_INP_INIT_KEY      0x0001
#define APP_INP_INIT_JOY      0x0002

#define APP_INP_CODE(x,y)   ((x) | (y))

#define APP_INP_KEY             0x01000
#define APP_INP_MOUSE           0x02000
#define APP_INP_JOY1            0x03000
#define APP_INP_JOY2            0x04000
#define APP_INP_JOY3            0x05000
#define APP_INP_JOY4            0x06000
#define APP_INP_JOY5            0x07000
#define APP_INP_JOY6            0x08000
#define APP_INP_JOY7            0x09000
#define APP_INP_JOY8            0x0a000

#define APP_INP_MOUSE_LEFT      0x00000
#define APP_INP_MOUSE_RIGHT     0x00001
#define APP_INP_MOUSE_MIDDLE    0x00002

#define APP_INP_JOY_UP          0x00001
#define APP_INP_JOY_DOWN        0x00002
#define APP_INP_JOY_LEFT        0x00003
#define APP_INP_JOY_RIGHT       0x00004
#define APP_INP_JOY_BUT1        0x00005
#define APP_INP_JOY_BUT2        0x00006
#define APP_INP_JOY_BUT3        0x00007
#define APP_INP_JOY_BUT4        0x00008
#define APP_INP_JOY_BUT5        0x00009
#define APP_INP_JOY_BUT6        0x0000a
#define APP_INP_JOY_BUT7        0x0000b
#define APP_INP_JOY_BUT8        0x0000c

void            CO_InpInit          (int flags);
void            CO_InpUpdate        (void);
bool            CO_InpIsPressed     (int code);
int             CO_InpGetKey        (bool trans);

/*
==================================================
co_font.cpp
==================================================
*/
#define CO_FontColorPal(pal,x)      CO_FontColor (pal[x][0],pal[x][1],pal[x][2])

void            CO_FontLoad         (const char *fontFile);
void            CO_FontFree         (void);
void            CO_FontColor        (GLubyte red, GLubyte green, GLubyte blue);
void            CO_FontPrint        (int x, int y, char *str);
void            CO_FontPrintf       (int x, int y, const char *str, ...);
void            CO_FontSetMode      (bool x_mul, bool y_mul, bool y_rev);
const myBYTE    *CO_FontGet         (void);

/*
==================================================
co_fps.cpp
==================================================
*/
void            CO_FpsSyncLoops     (void (*move)(void), void (*draw)(void));

/*
==================================================
co_vid.cpp
==================================================
*/
void            VID_SwapBuffers     (void);
void            VID_Init            (void);
void            VID_SetMode         (int w, int h, int bpp, int aa, bool full);
void            VID_Shutdown        (void);

#endif
