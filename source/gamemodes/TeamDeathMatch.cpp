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

#include "TeamDeathMatch.h"

namespace Duel6 {
    namespace {
        std::vector<Team> TEAMS = {
                {"Alpha",   Color(255, 0, 0)},
                {"Bravo",   Color(0, 255, 0)},
                {"Charlie", Color(255, 255, 0)},
                {"Delta",   Color(255, 0, 255)}
        };
    }

    const Team &TeamDeathMatch::getPlayerTeam(Size playerIndex) {
        Size playerTeam = playerIndex % teamsCount;
        return TEAMS[playerTeam];
    }

    void TeamDeathMatch::initializePlayers(std::vector<Game::PlayerDefinition> &definitions) {
        Size index = 0;
        for (auto &definition : definitions) {
            const Team &team = getPlayerTeam(index);
            PlayerSkinColors &colors = definition.getColors();
            colors.set(PlayerSkinColors::Trousers, team.color);
            colors.set(PlayerSkinColors::HairTop, team.color);
            index++;
        }
    }

    void TeamDeathMatch::initializeRound(Game &game, std::vector<Player> &players, World &world) {
        teamMap.clear();
        Size index = 0;
        for (auto &player : players) {
            const Team &team = getPlayerTeam(index);
            teamMap.insert(std::make_pair(&player, &team));
            index++;
        }

        eventListener = std::make_unique<TeamDeathMatchPlayerEventListener>(world.getMessageQueue(), game.getSettings(),
                                                                            friendlyFire, teamMap);
        for (auto &player : players) {
            player.setEventListener(*eventListener);
        }
    }

    bool TeamDeathMatch::checkRoundOver(World &world, const std::vector<Player *> &alivePlayers) {
        if (alivePlayers.empty()) {
            for (const Player &player : world.getPlayers()) {
                world.getMessageQueue().add(player, "End of round - no winner");
            }
            return true;
        }

        const Team *lastAliveTeam = teamMap.at(alivePlayers[0]);
        for (Player *player : alivePlayers) {
            const Team *playerTeam = teamMap.at(player);
            if (playerTeam != lastAliveTeam) {
                return false;
            }
        }

        for (Player &player : world.getPlayers()) {
            const Team *playerTeam = teamMap.at(&player);
            if (playerTeam == lastAliveTeam) {
                world.getMessageQueue().add(player, Format("Team {0} won!") << lastAliveTeam->name);
                player.getPerson().addWins(1);
            }
        }

        return true;
    }

    Ranking TeamDeathMatch::getRanking(const std::vector<Player> &players) const {
        Ranking ranking;

        Size index = 0;
        for (const auto &player : players) {
            Size teamIndex = index % teamsCount;
            const Team &team = TEAMS[teamIndex];
            if (teamIndex < ranking.size()) {
                ranking[teamIndex].points += player.getPerson().getTotalPoints();
            } else {
                ranking.push_back(RankingEntry{team.name, player.getPerson().getTotalPoints(), team.color});
            }
            index++;
        }

        std::sort(ranking.begin(), ranking.end(), [](const RankingEntry &left, const RankingEntry &right) {
            return left.points > right.points;
        });

        return ranking;
    }
}
