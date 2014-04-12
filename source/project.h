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
#include "core/co_core.h"
#include "mylib/mycam.h"
#include "sound/sound.h"
#include "loader.h"
#include "anim.h"

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

#define D6_CL_WIDTH         800
#define D6_CL_HEIGHT        600
#define D6_CL_BPP           32
#define D6_CL_AA            2

#define D6_GL_PERSPECTIVE   1
#define D6_GL_ORTHO         2

#define D6_WEAPONS          17
#define D6_ANM_SPEED        20
#define D6_EXPL_SPEED       0.01f
#define D6_WAVE_HEIGHT      0.1f

#define D6_MAX_PLAYERS      8
#define D6_MAX_LIFE         100
#define D6_MAX_AIR          350
#define D6_MAX_EXPLOSIONS   100
#define D6_GAME_OVER_WAIT   (3.0f * APP_FPS_SPEED)

#define D6_SOUNDS           25

#define D6_SQR(x)           ((x)*(x))

namespace Duel6
{
	class Player; // Forward declaration
	class ColorTexture; // Forward declaration

	enum
	{
		D6_ZM_FULL,
		D6_ZM_SCROLL
	};

	struct d6VIEW_s
	{
		int     X;
		int     Y;
		int     Width;
		int     Height;
	};

	struct d6KEYBOARD_s
	{
		int     Left;
		int     Right;
		int     Up;
		int     Down;
		int     Shoot;
		int     Pick;
	};

	struct d6WEAPONDEF_s
	{
		float   ShotSpeed;
		bool    Blood;
		myDWORD ExplC;
		int     Boom;
		int     Power;
		int     ReloadSpeed;
		char    Name[30];
		int     ShSound;
		int     BmSound;
		float   ExpGrow;
	};

	class d6SHOT_s
	{
	public:
		float           X;
		float           Y;
		d6WEAPONDEF_s   *WD;
		int             A;
		int             O;
		Player          *Author;
		int             GN;
		int             I;

	public:
		int GetExplosionRange();
		int GetExplosionPower();
	};

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

	extern  float       d6Sin[450], d6KeyWait;
	extern  d6WORLD     d6World;
	extern  myUINT      d6BackTex;
	extern  bool        d6InMenu;
	extern  int         d6Wireframe, d6ZoomBlc, d6ZoomMode, d6Playing;
	extern  bool        d6ShowFps, d6PlayMusic;
	extern  int			d6AmmoRangeMin, d6AmmoRangeMax;

	//////////////////////////////////////////////////////////////////////
	//                          duel6.cpp                               //
	//////////////////////////////////////////////////////////////////////
	extern  int          d6Winner;
	extern  float		 d6GameOverWait;

	int     D6_BlockZ(int x, int y);
	int     D6_BlockN(int x, int y);
	float   D6_Sin(int a);
	float   D6_Cos(int a);
	void    D6_ConSwitchW(con_c *con);
	void    D6_ConShowFps(con_c *con);
	void    D6_SetView(d6VIEW_s *w);
	void    D6_SetView(int x, int y, int width, int height);
	void    D6_SetGLMode(int mode);
	void    D6_GameLoop(void);

	//////////////////////////////////////////////////////////////////////
	//                          setup.cpp                               //
	//////////////////////////////////////////////////////////////////////
	void    SET_LoadBackground(int n);
	void    SET_LoadWorld(const std::string& path);

	//////////////////////////////////////////////////////////////////////
	//                          util.cpp                                //
	//////////////////////////////////////////////////////////////////////
	void    UTIL_LoadKH3Texture(GLuint *tarr, const char *fl, int n, bool clamp);
	void    UTIL_SaveScreenTga(int sd);
	void    UTIL_OpenGLInfo(con_c *con);
	void    UTIL_PixelFormatInfo(void);

	//////////////////////////////////////////////////////////////////////
	//                          water.cpp                               //
	//////////////////////////////////////////////////////////////////////
	void    WATER_Free(void);
	void    WATER_Build(void);
	void    WATER_Move(void);

	//////////////////////////////////////////////////////////////////////
	//                          render.cpp                              //
	//////////////////////////////////////////////////////////////////////
	void    RENDER_MoveScene(void);
	void    RENDER_DrawScene(void);

	//////////////////////////////////////////////////////////////////////
	//                          weapon.cpp                              //
	//////////////////////////////////////////////////////////////////////
	extern  d6WEAPONDEF_s       d6WpnDef[D6_WEAPONS];
	extern  bool                d6WpnEnabled[D6_WEAPONS];
	extern  short               d6ShotAnm[D6_WEAPONS][18];
	extern  short               d6BoomAnm[D6_WEAPONS][14];

	void    WPN_LoadTextures(void);
	void    WPN_FreeTextures(void);
	void    WPN_Init(void);
	void    WPN_DeInit(void);
	void    WPN_LevelInit(void);
	void    WPN_Shoot(Player& player);
	void    WPN_MoveShots(void);
	void    WPN_Boom(d6SHOT_s *s, Player *p);
	int     WPN_GetRandomWeapon(void);

	//////////////////////////////////////////////////////////////////////
	//                          kontrola.cpp                            //
	//////////////////////////////////////////////////////////////////////
	void    KONTR_Init(void);
	void    KONTR_Kontr(Player& player, int c);
	bool    KONTR_Shot(d6SHOT_s *s);
	bool    KONTR_CanJump(Player *p);

	//////////////////////////////////////////////////////////////////////
	//                          explode.cpp                             //
	//////////////////////////////////////////////////////////////////////
	void    EXPL_Init(void);
	void    EXPL_Load(void);
	void    EXPL_Free(void);
	void    EXPL_MoveAll(void);
	void    EXPL_DrawAll(void);
	void    EXPL_Add(float x, float y, float s, float m, myDWORD c);

	//////////////////////////////////////////////////////////////////////
	//                          elevator.cpp                            //
	//////////////////////////////////////////////////////////////////////
	void    ELEV_Init(void);
	void    ELEV_Free(void);
	void    ELEV_MoveAll(void);
	void    ELEV_DrawAll(void);
	void    ELEV_Load(const std::string& path, bool mirror);
	void    ELEV_CheckMan(Player& player);
	void    ELEV_MoveMan(Player& player);

	//////////////////////////////////////////////////////////////////////
	//                          info.cpp                                //
	//////////////////////////////////////////////////////////////////////
	void    INFO_Init(void);
	void    INFO_Add(Player& player, const char *tex, ...);
	void    INFO_MoveAll(void);
	void    INFO_DrawAll(void);
	void    INFO_ShowRankingSwap(void);

	//////////////////////////////////////////////////////////////////////
	//                          fire.cpp                                //
	//////////////////////////////////////////////////////////////////////
	void    FIRE_Init(void);
	void    FIRE_Free(void);
	void    FIRE_Find(void);
	void    FIRE_Check(float X, float Y, int d);

	//////////////////////////////////////////////////////////////////////
	//                          bonus.cpp                               //
	//////////////////////////////////////////////////////////////////////
	enum
	{
		D6_BONUS_LIFEP = 19,
		D6_BONUS_LIFEM,
		D6_BONUS_LIFEF,
		D6_BONUS_SHOTS,
		D6_BONUS_SHOTP,
		D6_BONUS_INVUL,
		D6_BONUS_BULLT,
		D6_BONUS_GUESS,
		D6_BONUS_SPEED = 46,
		D6_BONUS_INVIS = 53
	};

	void    BONUS_Init(int(*bonus)[3]);
	void    BONUS_DrawAll(void);
	void    BONUS_AddNew(void);
	void    BONUS_AddDeadManGun(int x, int y, Player& player);
	void    BONUS_Check(Player& player);
	void    BONUS_Pick(Player& player);

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

#include "Player.h"

#endif
