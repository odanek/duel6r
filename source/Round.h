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

#ifndef DUEL6_ROUND_H
#define DUEL6_ROUND_H

#include <vector>
#include <queue>
#include "Player.h"
#include "World.h"
#include "SysEvent.h"

namespace Duel6 {
    class Game;

    class Round {
    private:
        Game &game;
        Int32 roundNumber;
        World world;
        bool suddenDeathMode;
        Float32 waterFillWait;
        Float32 showYouAreHere;
        Float32 gameOverWait;
        bool winner;
        std::vector<Player *> alivePlayers;
        LevelScript &levelScript;
        GlobalScript &globalScript;
        Uint32 frame;
    public:
        Round(Game &game, Int32 roundNumber, std::vector<Player> &players, const std::string &levelPath, bool mirror,
              Size background, LevelScript &levelScript, GlobalScript &globalScript);

        void update(Float32 elapsedTime);

        void keyEvent(const KeyPressEvent &event);

        const World &getWorld() const {
            return world;
        }

        bool hasWinner() const {
            return winner;
        }

        Float32 getRemainingYouAreHere() const {
            return showYouAreHere;
        }

        Float32 getRemainingGameOverWait() const {
            return gameOverWait;
        }

        Int32 getRoundNumber() const {
            return roundNumber;
        }

        bool isOver() const;

        bool isLast() const;

    private:
        void preparePlayers();

        void checkWinner();

        void setPlayerViews();

        void splitScreenView(Player &player, Int32 x, Int32 y);

        void switchScreenMode();
    };
}

#endif
