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

    void TeamDeathMatch::initializePlayer(PlayerDefinition &definition) {
        const Team &team = getPlayerTeam(definition.getTeam() - 1);
        PlayerSkinColors &colors = definition.getColors();
        auto hair = colors.getHair();
        if(hair == PlayerSkinColors::Hair::None || hair == PlayerSkinColors::Hair::Short){
            colors.setHeadBand(true);
        }
        colors.set(PlayerSkinColors::HeadBand, team.color);
        colors.set(PlayerSkinColors::Trousers, team.color);
        colors.set(PlayerSkinColors::HairTop, team.color);
    }

    void TeamDeathMatch::initializePlayerPositions(Game &game, std::vector<Player> &players, World &world) const {
        game.getAppService().getConsole().printLine("...Preparing team players");
        Level::StartingPositionList startingPositions;
        world.getLevel().findStartingPositions(startingPositions);

        Int32 layerSpan = Int32(startingPositions.size()) / teamsCount;
        Int32 randomizer = Math::random(teamsCount);
        Int32 playerIndex = 0;
        for (Player &player : players) {
            if(player.isDeleted()) continue;
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

    void TeamDeathMatch::joinPlayers(Game &game, std::vector<Player> &players, std::vector<size_t> joined, World &world) {
        Level::StartingPositionList startingPositions;
        world.getLevel().findStartingPositions(startingPositions);

        Int32 layerSpan = Int32(startingPositions.size()) / teamsCount;
        Int32 randomizer = Math::random(teamsCount);

        for (size_t i : joined) {
            Player &player = players[i];
            player.setEventListener(*eventListener);

            auto &ammoRange = game.getSettings().getAmmoRange();
            Int32 ammo = Math::random(ammoRange.first, ammoRange.second);
            if (player.getTeam() == 0) {
                Level::StartingPosition position = startingPositions[i % startingPositions.size()];
                player.startRound(world, position.first, position.second, ammo, Weapon::getRandomEnabled(game.getSettings()));
                continue;
            }
            const Team &team = getPlayerTeam(player.getTeam() - 1);
            teamMap.insert(std::make_pair(&player, &team));

            Int32 playerTeam = (player.getTeam() - 1 + randomizer) % teamsCount;
            Int32 playerTeamIndex = Math::random(layerSpan);
            Int32 index = (layerSpan * playerTeam) + playerTeamIndex % layerSpan;

            Level::StartingPosition position = startingPositions[index];
            player.startRound(world, position.first, position.second, ammo, Weapon::getRandomEnabled(game.getSettings()));
        }
    }

    void TeamDeathMatch::initializeRound(Game &game, std::vector<Player> &players, World &world) {
        teamMap.clear();
        eventListener = std::make_unique<TeamDeathMatchPlayerEventListener>(world.getMessageQueue(), game.getSettings(),
            friendlyFire, teamMap, globalAssistances);
        for (auto &player : players) {
            if(player.isDeleted()) continue;
            player.setEventListener(*eventListener);
            if (player.getTeam() == 0) {
                continue;
            }
            const Team &team = getPlayerTeam(player.getTeam() - 1);
            teamMap.insert(std::make_pair(&player, &team));
        }
    }

    bool TeamDeathMatch::checkRoundOver(World &world, const std::vector<Player *> &alivePlayers) {
        if(world.getPlayers().size() < 2){
            return false;
        }
        if (alivePlayers.empty()) {
            for (const Player &player : world.getPlayers()) {
                world.getMessageQueue().add(player, "End of round - no winner");
            }
            return true;
        }
        if(alivePlayers[0]->getTeam() == 0 && alivePlayers.size() > 1){
            return false;
        }
        const Team *lastAliveTeam = nullptr;
        bool hasPlayersWithTeam = false;

        for (Player *player : alivePlayers) {
            if (player->getTeam() != 0) {
                hasPlayersWithTeam = true;
                lastAliveTeam = teamMap.at(player);
                break;
            }
        }

        if (!hasPlayersWithTeam) {
            if(alivePlayers.size() < 2){
                for (Player *player : alivePlayers) {
                    player->getPerson().addWins(1);
                }
                return true;
            }
            return false;
        }

        for (Player *player : alivePlayers) {
            if(player->getTeam() == 0){
                return false;
            }
            const Team *playerTeam = teamMap.at(player);
            if (playerTeam != lastAliveTeam) {
                return false;
            }
        }

        for (Player &player : world.getPlayers()) {
            if(player.getTeam() == 0 || !player.isInGame()){
                continue;
            }
            const Team *playerTeam = teamMap.at(&player);
            if (playerTeam == lastAliveTeam) {
                world.getMessageQueue().add(player, Format("Team {0} won!") << lastAliveTeam->name);
                if (player.isAlive() ) {
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
            Color color = team.color;
            color.add(255,255,255,150);
            auto entry = Ranking::Entry{team.name, 0, color, bcgColor, true};
            ranking.entries.push_back(entry);
        }
        for (const auto &player : players) {
            if(player.getTeam() == 0) {
                auto entry = Ranking::Entry{"no team", 0, Color::WHITE, Color::BLUE.withAlpha(178)};
                ranking.entries.push_back(entry);
                break;
            }
        }
        Int32 index = 0;

        for (const auto &player : players) {
            if(player.isDeleted()){
                continue;
            }
            Int32 teamIndex = (player.getTeam() - 1) % teamsCount;
            if(player.getTeam() == 0) {
                teamIndex = teamsCount;
            }
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
            entry.rtt = player.rtt;
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
        if (quickLiquid) {
            return true;
        }
        std::vector<Uint32> teamCounts(teamsCount + 1, 0);
        Uint32 inGame = 0;

        for (auto const &player: world.getPlayers()) {
            if(! player.isInGame()) {
                continue;
            }
            inGame++;
            Size teamIndex = (player.getTeam()) % teamsCount;
            if (player.isAlive()) {
                teamCounts[teamIndex]++;
            }
        }
        if(inGame == alivePlayers.size()){
            return false; //don't raise water when everybody is still alive;
        }
        for (auto count: teamCounts) {
            if (count < 2) {
                return true;
            }
        }

        return false;
    }
}
