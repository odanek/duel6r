/*
 * PLayerScore.cpp
 *
 *  Created on: May 17, 2020
 *      Author: fanick
 */

#include "PlayerScore.h"
#include "../../Player.h"

namespace Duel6::net {
    PlayerScore::PlayerScore(){
        changed.reset();
        changed.set(NO_CHANGE, true);
    }

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
        changed.set();
        changed.reset(NO_CHANGE);
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
#define D(POS, FIELD) if(!(confirmed.FIELD == snapshot.FIELD)) snapshot.changed.set(POS)
         void PlayerScore::diff(PlayerScore &snapshot, const PlayerScore &confirmed) {
            snapshot.changed.reset();
            D(SHOTS, shots);
            D(HITS, hits);
            D(KILLS, kills);
            D(DEATHS, deaths);
            D(ASSISTANCES, assistances);
            D(WINS, wins);
            D(PENALTIES, penalties);
            D(GAMES, games);
            D(TIME_ALIVE, timeAlive);
            D(TOTAL_GAMETIME, totalGameTime);
            D(TOTAL_DAMAGE, totalDamage);
            D(ASSISTED_DAMAGE, assistedDamage);
            D(ELO, elo);
            D(ELO_TREND, eloTrend);
            D(ELO_GAMES, eloGames);
            if (snapshot.changed.none()) {
                snapshot.changed.set(NO_CHANGE);
            }
        }

#define R(POS, FIELD) if(unchanged || !received.changed[POS]) received.FIELD = confirmed.FIELD;
         void PlayerScore::fillinFromPreviousConfirmed(const PlayerScore &confirmed, PlayerScore &received) {
            bool unchanged = received.changed[NO_CHANGE];
            R(SHOTS, shots);
            R(HITS, hits);
            R(KILLS, kills);
            R(DEATHS, deaths);
            R(ASSISTANCES, assistances);
            R(WINS, wins);
            R(PENALTIES, penalties);
            R(GAMES, games);
            R(TIME_ALIVE, timeAlive);
            R(TOTAL_GAMETIME, totalGameTime);
            R(TOTAL_DAMAGE, totalDamage);
            R(ASSISTED_DAMAGE, assistedDamage);
            R(ELO, elo);
            R(ELO_TREND, eloTrend);
            R(ELO_GAMES, eloGames);
        }
}

