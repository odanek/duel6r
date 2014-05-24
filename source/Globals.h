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

#include <vector>
#include "core/co_core.h"

namespace Duel6
{
	class BlockData;
	class World;
	class InfoMessageQueue;
	class SpriteList;
	class Player;
	class PlayerSkinColors;
	class Menu;
	class Game;

	extern Context* d6Context;
	extern Menu d6Menu;
	extern Game d6Game;
	extern BlockData d6BlockData;
	extern GLuint d6BackgroundTexture;
	extern bool d6Wireframe;
	extern bool d6ShowFps, d6PlayMusic;
	extern int d6AmmoRangeMin, d6AmmoRangeMax;
	extern bool d6ShowRanking;
	extern std::vector<PlayerSkinColors> d6PlayerColors;
	extern InfoMessageQueue d6MessageQueue;
	extern SpriteList d6SpriteList;
	extern World d6World;
}

#endif