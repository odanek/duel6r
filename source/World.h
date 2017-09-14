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

#ifndef DUEL6_WORLD_H
#define DUEL6_WORLD_H

#include "Level.h"
#include "InfoMessageQueue.h"
#include "LevelRenderData.h"
#include "Explosion.h"
#include "Fire.h"
#include "ShotList.h"
#include "BonusList.h"
#include "ElevatorList.h"

namespace Duel6 {
    class Game;

    class World {
    private:
        const GameSettings &gameSettings;
        std::vector<Player> &players;
        Level level;
        LevelRenderData levelRenderData;
        InfoMessageQueue messageQueue;
        SpriteList spriteList;
        ShotList shotList;
        ExplosionList explosionList;
        FireList fireList;
        Size background;
        BonusList bonusList;
        ElevatorList elevatorList;

    public:
        World(Game &game, const std::string &levelPath, bool mirror, Size background);

        void update(Float32 elapsedTime);

        void raiseWater();

        const GameSettings &getGameSettings() const {
            return gameSettings;
        }

        std::vector<Player> &getPlayers() {
            return players;
        }

        const std::vector<Player> &getPlayers() const {
            return players;
        }

        Level &getLevel() {
            return level;
        }

        const Level &getLevel() const {
            return level;
        }

        LevelRenderData &getLevelRenderData() {
            return levelRenderData;
        }

        const LevelRenderData &getLevelRenderData() const {
            return levelRenderData;
        }

        InfoMessageQueue &getMessageQueue() {
            return messageQueue;
        }

        const InfoMessageQueue &getMessageQueue() const {
            return messageQueue;
        }

        SpriteList &getSpriteList() {
            return spriteList;
        }

        const SpriteList &getSpriteList() const {
            return spriteList;
        }

        ShotList &getShotList() {
            return shotList;
        }

        const ShotList &getShotList() const {
            return shotList;
        }

        ExplosionList &getExplosionList() {
            return explosionList;
        }

        const ExplosionList &getExplosionList() const {
            return explosionList;
        }

        FireList &getFireList() {
            return fireList;
        }

        const FireList &getFireList() const {
            return fireList;
        }

        Size getBackground() const {
            return background;
        }

        BonusList &getBonusList() {
            return bonusList;
        }

        const BonusList &getBonusList() const {
            return bonusList;
        }

        ElevatorList &getElevatorList() {
            return elevatorList;
        }

        const ElevatorList &getElevatorList() const {
            return elevatorList;
        }
    };
}

#endif