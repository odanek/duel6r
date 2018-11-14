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
        bool rankingComparator(const Ranking::Entry &left, const Ranking::Entry &right) {
            return left.points > right.points;
        }
    }

    std::vector<Team> TEAMS = {
            {"Alpha",   Color(255, 0, 0)},
            {"Bravo",   Color(0, 255, 0)},
            {"Charlie", Color(255, 255, 0)},
            {"Delta",   Color(255, 0, 255)}
    };

    const Team &TeamDeathMatch::getPlayerTeam(Int32 playerIndex) const {
        Int32 playerTeam = playerIndex % teamsCount;
        return TEAMS[playerTeam];
    }

    void TeamDeathMatch::initializePlayers(std::vector<Game::PlayerDefinition> &definitions) {
        Int32 index = 0;
        for (auto &definition : definitions) {
            const Team &team = getPlayerTeam(index);
            PlayerSkinColors &colors = definition.getColors();
            colors.set(PlayerSkinColors::Trousers, team.color);
            colors.set(PlayerSkinColors::HairTop, team.color);
            index++;
        }
    }

    void TeamDeathMatch::initializePlayerPositions(Game &game, std::vector<Player> &players, World &world) const {
        game.getAppService().getConsole().printLine("...Preparing team players");
        Level::StartingPositionList startingPositions;
        world.getLevel().findStartingPositions(startingPositions);

        Int32 layerSpan = Int32(startingPositions.size()) / teamsCount;
        Int32 randomizer = Math::random(teamsCount);
        Int32 playerIndex = 0;
        for (Player &player : players) {
            auto &ammoRange = game.getSettings().getAmmoRange();
            Int32 ammo = Math::random(ammoRange.first, ammoRange.second);

            Int32 playerTeam = (playerIndex + randomizer) % teamsCount;
            Int32 playerTeamIndex = Math::random(layerSpan);
            Int32 index = (layerSpan * playerTeam) + playerTeamIndex % layerSpan;

            Level::StartingPosition position = startingPositions[index];
            player.startRound(world, position.first, position.second, ammo, Weapon::getRandomEnabled(game.getSettings()));
            playerIndex++;
        }
    }

    void TeamDeathMatch::initializeRound(Game &game, std::vector<Player> &players, World &world) {
        teamMap.clear();
        Int32 index = 0;
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
                if (player.isAlive()) {
                    player.getPerson().addWins(1);
                }
            }
        }

        return true;
    }

    Ranking TeamDeathMatch::getRanking(const std::vector<Player> &players) const {
        Ranking ranking;

        for (Int32 teamIndex = 0; teamIndex < teamsCount; teamIndex++) {
            const Team &team = TEAMS[teamIndex];
            Color bcgColor = team.color.withAlpha(178);
            auto entry = Ranking::Entry{team.name, 0, Color::BLACK, bcgColor};
            ranking.entries.push_back(entry);
        }

        Int32 index = 0;
        for (const auto &player : players) {
            Int32 teamIndex = index % teamsCount;
            Ranking::Entry &teamEntry = ranking.entries[teamIndex];

            teamEntry.points += player.getPerson().getTotalPoints();
            teamEntry.kills += player.getPerson().getKills();
            teamEntry.deaths += player.getPerson().getDeaths();
            teamEntry.penalties += player.getPerson().getPenalties();
            teamEntry.assistances += player.getPerson().getAssistances();
            Color fontColor(255, player.isAlive() ? 255 : 0, 0);
            Color bcgColor = teamEntry.bcgColor.scale(0.2f);

            Ranking::Entry entry(player.getPerson().getName(), player.getPerson().getTotalPoints(), fontColor,
                                 bcgColor);
            entry.kills = player.getPerson().getKills();
            entry.deaths = player.getPerson().getDeaths();
            entry.penalties = player.getPerson().getPenalties();
            entry.assistances = player.getPerson().getAssistances();
            teamEntry.addSubEntry(entry);
            index++;
        }

        std::sort(ranking.entries.begin(), ranking.entries.end(), rankingComparator);
        for (auto &entry : ranking.entries) {
            std::sort(entry.entries.begin(), entry.entries.end(), rankingComparator);
        }

        return ranking;
    }

    bool TeamDeathMatch::checkForSuddenDeathMode(World &world, const std::vector<Player *> &alivePlayers) const {
        std::vector<Uint32> teamCounts(teamsCount, 0);
        Size index = 0;
        for (auto const &player: world.getPlayers()) {
            Size teamIndex = index % teamsCount;
            if (player.isAlive()) {
                teamCounts[teamIndex]++;
            }
            index++;
        }
        for (auto count: teamCounts) {
            if (count < 2) {
                return true;
            }
        }

        return false;
    }
}
