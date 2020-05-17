/*
 * PLayerScore.cpp
 *
 *  Created on: May 17, 2020
 *      Author: fanick
 */

#include "PlayerScore.h"
#include "../../Player.h"

namespace Duel6::net {
    void PlayerScore::loadFromPlayer(Player &player) {
        shots = player.getPerson().getShots();
        hits = player.getPerson().getHits();
        kills = player.getPerson().getKills();
        deaths = player.getPerson().getDeaths();
        assistances = player.getPerson().getAssistances();
        wins = player.getPerson().getWins();
        penalties = player.getPerson().getPenalties();
        games = player.getPerson().getGames();
        timeAlive = player.getPerson().getTimeAlive();
        totalGameTime = player.getPerson().getTotalGameTime();
        totalDamage = player.getPerson().getTotalDamage();
        assistedDamage = player.getPerson().getAssistedDamage();
        elo = player.getPerson().getElo();
        eloTrend = player.getPerson().getEloTrend();
        eloGames = player.getPerson().getEloGames();
    }

    void PlayerScore::unloadToPlayer(Player &player) {
        player.getPerson().setShots(shots);
        player.getPerson().setHits(hits);
        player.getPerson().setKills(kills);
        player.getPerson().setDeaths(deaths);
        player.getPerson().setAssistances(assistances);
        player.getPerson().setWins(wins);
        player.getPerson().setPenalties(penalties);
        player.getPerson().setGames(games);
        player.getPerson().setTimeAlive(timeAlive);
        player.getPerson().setTotalGameTime(totalGameTime);
        player.getPerson().setTotalDamage(totalDamage);
        player.getPerson().setAssistedDamage(assistedDamage);
        player.getPerson().setElo(elo);
        player.getPerson().setEloTrend(eloTrend);
        player.getPerson().setEloGames(eloGames);
    }
    bool PlayerScore::operator ==(const PlayerScore &r) const {
         return shots == r.shots
         && hits == r.hits
         && kills == r.kills
         && deaths == r.deaths
         && assistances == r.assistances
         && wins == r.wins
         && penalties == r.penalties
         && games == r.games
         && timeAlive == r.timeAlive
         && totalGameTime == r.totalGameTime
         && totalDamage == r.totalDamage
         && assistedDamage == r.assistedDamage
         && elo == r.elo
         && eloTrend == r.eloTrend
         && eloGames == r.eloGames;
    }
}

