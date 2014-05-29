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

#ifndef CO_CORE_H
#define CO_CORE_H

#include <unordered_set>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "../mylib/mylib.h"
#include "../console/console.h"
#include "../Type.h"

/* Makra a definice */
#define APP_VERSION         "3.10.1"
#define APP_NAME            "Duel 6 Reloaded"
#define APP_FILE_ICON       "data/duel6_icon.bmp"
#define APP_FILE_FONT       "data/duel6.fon"

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
    float       fps;                // Aktualni pocet obrazku za sekundu
    bool        flags;              // Flagy aplikace
    std::unique_ptr<con_c> con;     // Konzole
    bool        active;             // Je aplikace prave aktivni (ma focus?)
    SDL_Window  *window;            // SDL window
	SDL_GLContext glContext;        // OpenGL context
    myBYTE      *font;              // Ukazatel na font
};

struct appVid_s
{
    int         cl_width;           // Client width
    int         cl_height;          // Client height
    int         gl_bpp;             // Current bits per pixel
    int         gl_aa;              // Current anti alliasing samples
    float       gl_farclip;         // OpenGL far clipping plane
    float       gl_nearclip;        // OpenGL far clipping plane
    float       gl_fov;             // OpenGL field of view
    float       gl_aspect;          // OpenGL aspect - pokud je 0 tak dopocitat jako width/height
};

class appInp_s
{
public:
	std::unordered_set<SDL_Keycode> pressedKeys;
    std::vector<SDL_Joystick*> joysticks;

	bool isPressed(SDL_Keycode keyCode)
	{
		return pressedKeys.find(keyCode) != pressedKeys.end();
	}
};

extern app_s    g_app;
extern appVid_s g_vid;
extern appInp_s g_inp;

/*
==================================================
co_main.cpp
==================================================
*/
void CO_Init(void(*errProc)(const char *));
void CO_DeInit();

/*
==================================================
co_input.cpp
==================================================
*/
void CO_JoystickScan();

/*
==================================================
co_font.cpp
==================================================
*/
#define CO_FontColorPal(pal,x)      CO_FontColor (pal[x][0],pal[x][1],pal[x][2])

void            CO_FontLoad         (const std::string& fontFile);
void            CO_FontColor        (GLubyte red, GLubyte green, GLubyte blue);
void            CO_FontPrint        (int x, int y, const char *str);
void            CO_FontPrintf       (int x, int y, const char *str, ...);
void            CO_FontSetMode      (bool x_mul, bool y_mul, bool y_rev);
const myBYTE    *CO_FontGet         ();

/*
==================================================
co_vid.cpp
==================================================
*/
void            VID_SwapBuffers     ();
void            VID_SetMode         (int w, int h, int bpp, int aa, bool full);

#endif
