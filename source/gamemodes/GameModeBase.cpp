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

#include "GameModeBase.h"

namespace Duel6 {
    void GameModeBase::initializePlayerPositions(Game &game, std::vector<Player> &players, World &world) const {
        game.getAppService().getConsole().printLine("...Preparing base players");
        Level::StartingPositionList startingPositions;
        world.getLevel().findStartingPositions(startingPositions);
        std::shuffle(startingPositions.begin(), startingPositions.end(), Math::randomEngine);

        Size playerIndex = 0;
        for (Player &player : players) {
            auto &ammoRange = game.getSettings().getAmmoRange();
            Int32 ammo = Math::random(ammoRange.first, ammoRange.second);
            Level::StartingPosition position = startingPositions[playerIndex % startingPositions.size()];
            player.startRound(world, position.first, position.second, ammo,
                              Weapon::getRandomEnabled(game.getSettings()));
            playerIndex++;
        }
    }

    Ranking GameModeBase::getRanking(const std::vector<Player> &players) const {
        std::vector<const Player *> ranking;
        for (const Player &player : players) {
            ranking.push_back(&player);
        }

        std::sort(ranking.begin(), ranking.end(), [](const Player *pl1, const Player *pl2) {
            return pl1->getPerson().hasHigherScoreThan(pl2->getPerson());
        });

        Ranking result;
        for (auto &player : ranking) {
            Color fontColor(255, player->isAlive() ? 255 : 0, 0);
            Color bcgColor = Color(0, 0, 255, 178);;
            auto entry = Ranking::Entry{player->getPerson().getName(), player->getPerson().getTotalPoints(), fontColor, bcgColor};
            entry.kills = player->getPerson().getKills();
            entry.deaths = player->getPerson().getDeaths();
            entry.penalties = player->getPerson().getPenalties();
            entry.assistances = player->getPerson().getAssistances();
            result.entries.push_back(entry);
        }

        return result;
    }

    bool GameModeBase::checkForSuddenDeathMode(World &world, const std::vector<Player *> &alivePlayers) const {
        return alivePlayers.size() == 2 && world.getPlayers().size() > 2;
    }
}
