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

#ifndef DUEL6_PROJECT_H
#define DUEL6_PROJECT_H

#include <string>
#include <vector>
#include "core/co_core.h"
#include "mylib/mycam.h"
#include "sound/sound.h"
#include "World.h"
#include "SpriteList.h"
#include "Type.h"
#include "Orientation.h"
#include "ScreenMode.h"
#include "Player.h"
#include "Shot.h"
#include "InfoMessageQueue.h"

#define D6_FILE_CONFIG      "data/config.txt"
#define D6_FILE_ART         "data/duel6.kh3"
#define D6_FILE_ANM         "data/duel6.anm"
#define D6_FILE_SIN         "data/sin.tab"
#define D6_FILE_BACK        "data/pozadi.kh3"
#define D6_FILE_WEAPON      "data/wpn.kh3"
#define D6_FILE_PLAYER      "data/man.kh3"
#define D6_FILE_FONT        "data/duel6.fon"
#define D6_FILE_EXPLODE     "data/explode.kh3"
#define D6_FILE_LEVEL       "levels/"
#define D6_FILE_PHIST       "data/duel6.dat"
#define D6_FILE_LABEL       "data/label.kh3"
#define D6_FILE_SKIN        "data/skin.txt"

#define D6_LEVEL_EXTENSION	".lev"

#define D6_SPEED_COEF		61
#define D6_CL_WIDTH         800
#define D6_CL_HEIGHT        600
#define D6_CL_BPP           32
#define D6_CL_AA            2

#define D6_GL_PERSPECTIVE   1
#define D6_GL_ORTHO         2

#define D6_ANM_SPEED        0.328
#define D6_EXPL_SPEED       0.61f
#define D6_WAVE_HEIGHT      0.1f

#define D6_MAX_PLAYERS      8
#define D6_WATER_HIT		60
#define D6_MAX_EXPLOSIONS   100
#define D6_GAME_OVER_WAIT   3.0f

#define D6_SOUNDS           25

#define D6_SQR(x)           ((x)*(x))

namespace Duel6
{
	enum
	{
		D6_SND_DEAD,
		D6_SND_BNPICK,
		D6_SND_SHOT_BL,
		D6_SND_BOOM_BL,
		D6_SND_SHOT_LS,
		D6_SND_SHOT_PI,
		D6_SND_SHOT_PL,
		D6_SND_SHOT_SG,
		D6_SND_SHOT_BZ,
		D6_SND_BOOM_BZ,
		D6_SND_WATER,
		D6_SND_SHOT_TZ,
		D6_SND_SHOT_KM,
		D6_SND_HIT,
		D6_SND_SHOT_LK,
		D6_SND_SHOT_SM,
		D6_SND_SHOT_KD1,
		D6_SND_SHOT_KD2,
		D6_SND_SHOT_SPRAY,
		D6_SND_SHOT_SLING,
		D6_SND_SHOT_SPUNT,
		D6_SND_SHOT_SHIT,
		D6_SND_SHOT_SHIT_HIT,
		D6_SND_LETS_ROCK,
		D6_SND_GAME_OVER
	};

	extern float d6Sin[450], d6KeyWait;
	extern World d6World;
	extern GLuint d6BackgroundTexture;
	extern bool d6InMenu;
	extern bool d6Wireframe;
	extern int d6ZoomBlc;
	extern bool d6ShowFps, d6PlayMusic;
	extern int d6AmmoRangeMin, d6AmmoRangeMax;
	extern ScreenMode d6ScreenMode;
	extern bool d6ShowRanking;
	extern std::vector<Player> d6Players;
	extern std::vector<PlayerSkinColors> d6PlayerColors;
	extern InfoMessageQueue d6MessageQueue;
	extern SpriteList d6SpriteList;
	extern std::vector<GLuint> d6WpnTextures;

	//////////////////////////////////////////////////////////////////////
	//                          duel6.cpp         
	//
	//////////////////////////////////////////////////////////////////////
	extern  int          d6Winner;
	extern  float		 d6GameOverWait;
	extern  int			 d6PlayedRounds;
	extern  int			 d6MaxRounds;

	float   D6_Sin(Int32 a);
	float   D6_Cos(Int32 a);
	void    D6_ConSwitchW(con_c *con);
	void    D6_ConShowFps(con_c *con);
	void    D6_SetGLMode(int mode);
	void    D6_StartGame(const std::string& levelPath);
	void    D6_GameLoop(void);

	//////////////////////////////////////////////////////////////////////
	//                          setup.cpp                               //
	//////////////////////////////////////////////////////////////////////
	void    SET_LoadBackground(int n);

	//////////////////////////////////////////////////////////////////////
	//                          util.cpp                                //
	//////////////////////////////////////////////////////////////////////
	GLuint  UTIL_LoadKH3Texture(const char *fl, int n, bool clamp);
	void    UTIL_SaveScreenTga(int sd);
	void    UTIL_OpenGLInfo(con_c *con);
	void    UTIL_PixelFormatInfo(void);

	//////////////////////////////////////////////////////////////////////
	//                          water.cpp                               //
	//////////////////////////////////////////////////////////////////////
	void    WATER_Build(void);
	void    WATER_Move(float elapsedTime);

	//////////////////////////////////////////////////////////////////////
	//                          render.cpp                              //
	//////////////////////////////////////////////////////////////////////
	void	RENDER_InitScreen();
	void    RENDER_DrawScene(ScreenMode screenMode);

	//////////////////////////////////////////////////////////////////////
	//                          kontrola.cpp                            //
	//////////////////////////////////////////////////////////////////////
	void    KONTR_Kontr(Player& player, int c);
	bool    KONTR_Shot(Shot& s);
	bool    KONTR_CanJump(Player *p);

	//////////////////////////////////////////////////////////////////////
	//                          explode.cpp                             //
	//////////////////////////////////////////////////////////////////////
	void    EXPL_Init(void);
	void    EXPL_Load(void);
	void    EXPL_Free(void);
	void    EXPL_MoveAll(float elapsedTime);
	void    EXPL_DrawAll(void);
	void    EXPL_Add(float x, float y, float s, float m, const Color& color);

	//////////////////////////////////////////////////////////////////////
	//                          elevator.cpp                            //
	//////////////////////////////////////////////////////////////////////
	void    ELEV_Init(void);
	void    ELEV_Free(void);
	void    ELEV_MoveAll(float elapsedTime);
	void    ELEV_DrawAll(void);
	void    ELEV_Load(std::vector<Int32>& elevatorData, bool mirror);
	void    ELEV_CheckMan(Player& player);
	void    ELEV_MoveMan(Player& player, float elapsedTime);

	//////////////////////////////////////////////////////////////////////
	//                          fire.cpp                                //
	//////////////////////////////////////////////////////////////////////
	void    FIRE_Init(void);
	void    FIRE_Find(void);
	void    FIRE_Check(Float32 X, Float32 Y, Float32 d);

	//////////////////////////////////////////////////////////////////////
	//                          menu.cpp                                //
	//////////////////////////////////////////////////////////////////////
	void    MENU_Init(void);
	void    MENU_JoyRescan(void);
	void    MENU_Free(void);
	void    MENU_KeyEvent(int e);
	void    MENU_Loop(void);
	void    MENU_Restart(bool same_level);
	void    MENU_Start(void);
	void    MENU_SavePH(void);
}

#endif
