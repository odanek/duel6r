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

#ifndef DUEL6_GLOBALS_H
#define DUEL6_GLOBALS_H

#include <unordered_set>
#include <vector>
#include <SDL2/SDL.h>
#include "console/console.h"
#include "PlayerSkinColors.h"

#define APP_VERSION         "3.11.0"
#define APP_NAME            "Duel 6 Reloaded"
#define APP_FILE_ICON       "data/duel6_icon.bmp"
#define APP_FILE_FONT       "data/duel6.fon"

#define APP_FLAG_NONE       0
#define APP_FLAG_QUIT       1

#define D6_FILE_CONFIG      "data/config.txt"
#define D6_FILE_ANM         "data/duel6.anm"
#define D6_FILE_COS         "data/sin.tab"
#define D6_FILE_FONT        "data/duel6.fon"
#define D6_FILE_LEVEL       "levels/"
#define D6_FILE_PHIST       "data/duel6.dat"
#define D6_FILE_SKIN        "data/skin.txt"

#define D6_TEXTURE_EXTENSION	".tga"

#define D6_TEXTURE_MAN_PATH     "textures/man/"
#define D6_TEXTURE_MAN_KEY		"man"
#define D6_TEXTURE_BCG_PATH		"textures/backgrounds/"
#define D6_TEXTURE_BCG_KEY		"bcg"
#define D6_TEXTURE_EXPL_PATH	"textures/explosion/"
#define D6_TEXTURE_EXPL_KEY		"expl"
#define D6_TEXTURE_MENU_PATH	"textures/menu/"
#define D6_TEXTURE_MENU_KEY		"menu"
#define D6_TEXTURE_WPN_PATH		"textures/weapon/"
#define D6_TEXTURE_WPN_KEY		"wpn"
#define D6_TEXTURE_BLOCK_PATH	"textures/blocks/"
#define D6_TEXTURE_BLOCK_KEY	"block"

//#define D6_RENDER_BACKS

#define D6_LEVEL_EXTENSION	".lev"

#define D6_SPRITE_SPEED_COEF	61

#define D6_ANM_SPEED			0.328f
#define D6_WAVE_HEIGHT			0.1f

#define D6_MAX_PLAYERS			8
#define D6_WATER_HIT			60
#define D6_GAME_OVER_WAIT		3.0f
#define D6_INFO_DURATION		5.0f
#define D6_PLAYER_MAX_SPEED     0.52f
#define D6_PLAYER_ACCEL         3.721f
#define D6_PLAYER_JPHASE_SPEED  122
#define D6_PLAYER_JUMP_SPEED    4.88f

#define D6_CAM_TOLPER_X         30
#define D6_CAM_TOLPER_Y         30

#define D6_SOUNDS				25

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

	class Input
	{
	public:
		std::unordered_set<SDL_Keycode> pressedKeys;
		std::vector<SDL_Joystick*> joysticks;

		bool isPressed(SDL_Keycode keyCode)
		{
			return pressedKeys.find(keyCode) != pressedKeys.end();
		}
	};

	extern Input d6Input;

	class BlockData;
	class World;
	class InfoMessageQueue;
	class SpriteList;
	class Player;
	class PlayerSkinColors;
	class Menu;
	class Game;
	class TextureManager;
	class Video;
	class Font;

	extern Console d6Console;
	extern Video d6Video;
	extern Menu d6Menu;
	extern Game d6Game;
	extern Font d6Font;
	extern TextureManager d6TextureManager;
	extern bool d6Wireframe;
	extern bool d6ShowFps;	
	extern bool d6ShowRanking;
	extern std::vector<PlayerSkinColors> d6PlayerColors;
	extern InfoMessageQueue d6MessageQueue;
	extern SpriteList d6SpriteList;
}

#endif