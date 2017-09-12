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

#include "World.h"
#include "Game.h"
#include "Weapon.h"

namespace Duel6 {
    World::World(Game &game, const std::string &levelPath, bool mirror, Size background)
            : gameSettings(game.getSettings()), players(game.getPlayers()),
              level(levelPath, mirror, game.getResources().getBlockMeta()),
              levelRenderData(level, D6_ANM_SPEED, D6_WAVE_HEIGHT), messageQueue(D6_INFO_DURATION),
              explosionList(game.getResources(), D6_EXPL_SPEED), fireList(game.getResources(), spriteList),
              background(background), bonusList(game.getSettings(), game.getResources(), *this),
              elevatorList(game.getResources().getElevatorTextures()) {
        Console &console = game.getAppService().getConsole();
        console.printLine(Format("...Width   : {0}") << level.getWidth());
        console.printLine(Format("...Height  : {0}") << level.getHeight());
        console.printLine("...Preparing faces");
        levelRenderData.generateFaces();
        console.printLine(Format("...Walls   : {0}") << levelRenderData.getWalls().getFaces().size());
        console.printLine(Format("...Sprites : {0}") << levelRenderData.getSprites().getFaces().size());
        console.printLine(Format("...Water   : {0}") << levelRenderData.getWater().getFaces().size());

        console.printLine("...Level initialization");
        console.printLine("...Loading elevators");
        elevatorList.load(levelPath, mirror);
        fireList.find(levelRenderData.getSprites());
    }

    void World::update(Float32 elapsedTime) {
        spriteList.update(elapsedTime * D6_SPRITE_SPEED_COEF);
        explosionList.update(elapsedTime);
        levelRenderData.update(elapsedTime);
        shotList.update(*this, elapsedTime);
        elevatorList.update(elapsedTime);
        messageQueue.update(elapsedTime);

        // Add new bonuses
        Int32 mod = Int32(3.0f / elapsedTime);

        if (mod != 0 && Math::random(mod) == 0) {
            bonusList.addRandomBonus();
        }
    }

    void World::raiseWater() {
        level.raiseWater();
        levelRenderData.generateWater();
    }
}