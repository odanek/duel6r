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

#ifndef DUEL6_DEFINES_H
#define DUEL6_DEFINES_H

#define APP_VERSION              "5.0.0"
#define APP_NAME                 "Duel 6 Reloaded"
#define APP_FILE_ICON            "data/duel6_icon.bmp"

#define D6_FILE_CONFIG           "data/config.script"
#define D6_FILE_BLOCK_META       "data/blocks.json"
#define D6_FILE_TTF_FONT         "data/font.ttf"
#define D6_FILE_LEVEL            "levels/"
#define D6_FILE_PHIST            "data/persons.json"
#define D6_FILE_PROFILES         "profiles"
#define D6_FILE_WEAPON_SOUNDS    "sound/weapon/"
#define D6_FILE_PLAYER_SOUNDS    "sound/player/"

#define D6_FILE_WATER_BLUE       "sound/game/water-blue.wav"
#define D6_FILE_WATER_RED        "sound/game/water-red.wav"
#define D6_FILE_WATER_GREEN      "sound/game/water-green.wav"

#define D6_FILE_PROFILE_SKIN     "skin.json"
#define D6_FILE_PROFILE_SOUNDS   "sounds.json"

//#define D6_RENDER_BACKS

#define D6_LEVEL_EXTENSION       ".json"

#define D6_UPDATE_FREQUENCY      200

#define D6_SPRITE_SPEED_COEF     61

#define D6_ANM_SPEED             0.328f
#define D6_WAVE_HEIGHT           0.1f

#define D6_EXPL_SPEED            0.61f

#define D6_MAX_PLAYERS           15
#define D6_AIR_RECHARGE_SPEED    60
#define D6_GAME_OVER_WAIT        6.0f
#define D6_ROUND_OVER_WAIT       1.0f
#define D6_INFO_DURATION         5.0f
#define D6_PLAYER_MAX_SPEED      0.52f
#define D6_PLAYER_ACCEL          3.721f
#define D6_PLAYER_JUMP_SPEED     4.88f
#define D6_YOU_ARE_HERE_DURATION 3.0f

#define D6_RAISE_WATER_WAIT      3.0f

#define D6_CAM_TOLPER_X          30
#define D6_CAM_TOLPER_Y          30

#define D6_MAX_LIFE              100.0f
#define D6_MAX_AIR               200.0f
#define D6_PLAYER_HPREGEN_DELAY  6.0f

#define D6_PLAYER_STUCK          5.0f

#endif