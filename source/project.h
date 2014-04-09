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

#ifndef __PROJECT_H
#define __PROJECT_H

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

class d6PLAYER_c; // Forward declaration

class d6SHOT_s
{
public:
    float           X;
    float           Y;
    d6WEAPONDEF_s   *WD;
    int             A;
    int             O;
    d6PLAYER_c      *Author;
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
extern  bool        d6InMenu, d6FreqProblem;
extern  int         d6Wireframe, d6ZoomBlc, d6ZoomMode, d6Playing, d6FreqUsed;
extern  bool        d6ShowFps, d6PlayMusic;
extern  int			d6AmmoRangeMin, d6AmmoRangeMax;

//////////////////////////////////////////////////////////////////////
//                          duel6.cpp                               //
//////////////////////////////////////////////////////////////////////
extern  int          d6Winner;
extern  float		 d6GameOverWait;

int     D6_BlockZ       (int x, int y);
int     D6_BlockN       (int x, int y);
float   D6_Sin          (int a);
float   D6_Cos          (int a);
void    D6_ConSwitchW   (con_c *con);
void    D6_ConShowFps   (con_c *con);
void    D6_SetView      (d6VIEW_s *w);
void    D6_SetView      (int x, int y, int width, int height);
void    D6_SetGLMode    (int mode);
void    D6_GameLoop     (void);

//////////////////////////////////////////////////////////////////////
//                          setup.cpp                               //
//////////////////////////////////////////////////////////////////////
void    SET_FixFreqBug      (void);
void    SET_LoadBackground  (int n);
void    SET_LoadWorld       (const char *fl);

//////////////////////////////////////////////////////////////////////
//                          util.cpp                                //
//////////////////////////////////////////////////////////////////////
void    UTIL_LoadKH3Texture     (GLuint *tarr, const char *fl, int n, bool clamp);
void    UTIL_SaveScreenTga      (int sd);
void    UTIL_OpenGLInfo         (con_c *con);
void    UTIL_PixelFormatInfo    (void);

//////////////////////////////////////////////////////////////////////
//                          water.cpp                               //
//////////////////////////////////////////////////////////////////////
void    WATER_Free      (void);
void    WATER_Build     (void);
void    WATER_Move      (void);

//////////////////////////////////////////////////////////////////////
//                          render.cpp                              //
//////////////////////////////////////////////////////////////////////
void    RENDER_MoveScene    (void);
void    RENDER_DrawScene    (void);

//////////////////////////////////////////////////////////////////////
//                          player.cpp                              //
//////////////////////////////////////////////////////////////////////
#define D6_PLAYER_MAX_SPEED     32
#define D6_PLAYER_ACCEL         0.001f
#define D6_PLAYER_JUMP_SPEED    0.08f

#define D6_CAM_TOLPER_X         30
#define D6_CAM_TOLPER_Y         30

#define D6_FLAG_DEAD            0x01
#define D6_FLAG_PICK            0x02
#define D6_FLAG_KNEE            0x04
#define D6_FLAG_REQ_LEFT        0x08
#define D6_FLAG_REQ_RIGHT       0x10
#define D6_FLAG_LYING           0x20
#define D6_FLAG_INWATER         0x40

struct d6PHIST_s
{
    char    Name[20];
    int     Shots;
    int     Hits;
    int     Kills;
    int     Wins;
    int     Games;
};

struct d6CAMPOS_s
{
    float           Left;
    float           Right;
    float           Up;
    float           Down;
    vec3_c<float>   Pos;
    float           TolX;
    float           TolY;
};

struct d6PLSTATE_s
{
    int         Flags;      // Flags
    int         I;          // Offset in d6Player array
    int         A;          // Animation number
    int         GA;         // Gun animation
    int         GN;         // Gun number
    float       Speed;      // Speed of movement
    int         O;          // Orientation (0 = left, 1 = right)
    float       J;          // Jump phase
    float       X;          // X position
    float       Y;          // Y position
    float       SI;         // Shot interval
    int         IBP[2];     // Info bar position
    float       Life;       // Life
    float       Air;        // Air
    int         Ammo;       // Number of bullets
    int         Elev;       // Standing on elevator
    int         Bonus;      // Bonus
    float       BD;         // Bonus duration
    float       SD;         // Sht duration
	bool        InWater;    // Player is in the water
    d6PHIST_s   *PH;        // Player history
};

class d6PLAYER_c
{
    private:
        static int      next_id;

    public:
        myUINT          *Texture;
        int             Textures;
        mycam_c         *Camera;
        d6CAMPOS_s      CamPos;
        d6VIEW_s        View;
        d6KEYBOARD_s    *Keys;
        d6PLSTATE_s     State;

        d6PLAYER_c          (void);
        ~d6PLAYER_c         (void);

        void    Skin        (void);
        void    SetView     (int x, int y, int w, int h);
        void    Left        (void);
        void    Right       (void);
        void    Jump        (void);
        void    Fall        (void);
        void    Pick        (void);
        void    MakeMove    (void);
        void    SetAnm      (void);
        void    CheckKeys   (void);
        void    PrepareCam  (void);
        void    UpdateCam   (void);
        void    SetControls (int n);
        bool    Hit         (float pw, d6SHOT_s *s, bool hit); // Returns true if the shot caused the player to die

		int GetIndex();
		float GetX();
		float GetY();
		
		bool HasPowerfulShots();
		bool IsKneeling();
		bool IsLying();
		bool IsDead();
};

extern d6PLAYER_c   *d6Player[D6_MAX_PLAYERS];
extern int          d6PlayerSkin[D6_MAX_PLAYERS][8];

//////////////////////////////////////////////////////////////////////
//                          player.cpp                              //
//////////////////////////////////////////////////////////////////////

void    PLAYER_PrepareViews (void);
void    PLAYER_UpdateAll    (void);
void    PLAYER_PrepareAll   (void);
void    PLAYER_SetColTex    (int i, int gn);

//////////////////////////////////////////////////////////////////////
//                          weapon.cpp                              //
//////////////////////////////////////////////////////////////////////
extern  d6WEAPONDEF_s       d6WpnDef[D6_WEAPONS];
extern  bool                d6WpnEnabled[D6_WEAPONS];
extern  short               d6ShotAnm[D6_WEAPONS][18];
extern  short               d6BoomAnm[D6_WEAPONS][14];

void    WPN_LoadTextures    (void);
void    WPN_FreeTextures    (void);
void    WPN_Init            (void);
void    WPN_Shoot           (d6PLSTATE_s *s);
void    WPN_MoveShots       (void);
void    WPN_Boom            (d6SHOT_s *s, d6PLAYER_c *p);
int     WPN_GetRandomWeapon (void);

//////////////////////////////////////////////////////////////////////
//                          kontrola.cpp                            //
//////////////////////////////////////////////////////////////////////
void    KONTR_Init          (void);
void    KONTR_Kontr         (d6PLAYER_c *p, int c);
bool    KONTR_Shot          (d6SHOT_s *s);
bool    KONTR_CanJump       (d6PLAYER_c *p);

//////////////////////////////////////////////////////////////////////
//                          explode.cpp                             //
//////////////////////////////////////////////////////////////////////
void    EXPL_Init           (void);
void    EXPL_Load           (void);
void    EXPL_Free           (void);
void    EXPL_MoveAll        (void);
void    EXPL_DrawAll        (void);
void    EXPL_Add            (float x, float y, float s, float m, myDWORD c);

//////////////////////////////////////////////////////////////////////
//                          elevator.cpp                            //
//////////////////////////////////////////////////////////////////////
void    ELEV_Init           (void);
void    ELEV_Free           (void);
void    ELEV_MoveAll        (void);
void    ELEV_DrawAll        (void);
void    ELEV_Load           (const char *sou, bool mirror);
void    ELEV_CheckMan       (d6PLSTATE_s *s);
void    ELEV_MoveMan        (d6PLSTATE_s *s);

//////////////////////////////////////////////////////////////////////
//                          info.cpp                                //
//////////////////////////////////////////////////////////////////////
void    INFO_Init           (void);
void    INFO_Add            (int i, const char *tex, ...);
void    INFO_MoveAll        (void);
void    INFO_DrawAll        (void);
void    INFO_ShowRankingSwap (void);

//////////////////////////////////////////////////////////////////////
//                          fire.cpp                                //
//////////////////////////////////////////////////////////////////////
void    FIRE_Init           (void);
void    FIRE_Free           (void);
void    FIRE_Find           (void);
void    FIRE_Check          (float X, float Y, int d);

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

void    BONUS_Init          (int (*bonus)[3]);
void    BONUS_DrawAll       (void);
void    BONUS_AddNew        (void);
void    BONUS_AddDeadManGun (int x, int y, d6PLSTATE_s *state);
void    BONUS_Check         (d6PLSTATE_s *s);
void    BONUS_Pick          (d6PLSTATE_s *s);

//////////////////////////////////////////////////////////////////////
//                          menu.cpp                                //
//////////////////////////////////////////////////////////////////////
void    MENU_Init           (void);
void    MENU_JoyRescan      (void);
void    MENU_Free           (void);
void    MENU_KeyEvent       (int e);
void    MENU_Loop           (void);
void    MENU_Restart        (bool same_level);
void    MENU_Start          (void);
void    MENU_SavePH         (void);

//////////////////////////////////////////////////////////////////////
//                       coltexture.cpp                             //
//////////////////////////////////////////////////////////////////////
#define D6_COL_WPN_SHT      16

class d6COLTEXTURE_c
{
private:
    myUINT          *Texture;
    int             Textures;

public:
    d6COLTEXTURE_c () 
		: Texture (NULL) 
	{}
    
	~d6COLTEXTURE_c (void) 
	{ 
		DeInit(); 
	}

    void    Init (myBYTE reg, myBYTE green, myBYTE blue);
    void    DeInit ();
    bool    IsInited () 
	{ 
		return Texture != NULL; 
	}
    myUINT  *GetTexPtr () 
	{ 
		return Texture; 
	}
};

extern d6COLTEXTURE_c   d6ShtTexture;

//////////////////////////////////////////////////////////////////////
//                       levels.cpp                                 //
//////////////////////////////////////////////////////////////////////
#define D6_LEVELS_MAX       1000

class d6LEVELS_c
{
private:
    char    **m_lev;
    char    **m_name;
    size_t  m_count;
    bool    m_init;

public:
    d6LEVELS_c() : m_lev(NULL), m_name(NULL), m_count(0), m_init(false) {}
    ~d6LEVELS_c() { DeInit(); }

    void        Init(const char *dir);
    void        DeInit();
    size_t      Count() { return m_count; }
    const char  *Name(size_t num) { return m_name[num]; }
    const char  *Path(size_t num) { return m_lev[num]; }
};

extern d6LEVELS_c d6LevelList;

#endif
