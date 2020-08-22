/*
 * PlayerScore.h
 *
 *  Created on: May 17, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_PLAYERSCORE_H_
#define SOURCE_NET_OBJECT_PLAYERSCORE_H_

#include <bitset>
#include "../../Type.h"

namespace Duel6{
    class Player;
}
namespace Duel6::net {
    class PlayerScore {
    public:
        void loadFromPlayer(Player &player);
        void unloadToPlayer(Player &player);
        bool operator ==(const PlayerScore &r) const;
        Int32 shots;
        Int32 hits;
        Int32 kills;
        Int32 deaths;
        Int32 assistances;
        Int32 wins;
        Int32 penalties;
        Int32 games;
        Int32 timeAlive;
        Int32 totalGameTime;
        Int32 totalDamage;
        Int32 assistedDamage;
        Int32 elo;
        Int32 eloTrend;
        Int32 eloGames;

        enum FIELDS {  // TODO implement diff/fillin same way as in net::Player
            NO_CHANGE,
            SHOTS,
            HITS,
            KILLS,
            DEATHS,
            ASSISTANCES,
            WINS,
            PENALTIES,
            GAMES,
            TIME_ALIVE,
            TOTAL_GAMETIME,
            TOTAL_DAMAGE,
            ASSISTED_DAMAGE,
            ELO,
            ELO_TREND,
            ELO_GAMES,
            _SIZE
        };
        std::bitset<FIELDS::_SIZE> changed;

#define D(POS, FIELD) if(!(confirmed.FIELD == snapshot.FIELD)) snapshot.changed.set(POS)
        static void diff(PlayerScore &snapshot, const PlayerScore &confirmed) {
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
        static void fillinFromPreviousConfirmed(const PlayerScore &confirmed, PlayerScore &received) {
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
#define S(POS, FIELD) if(changed[POS]) result &= s & FIELD
        template<typename Stream>
        bool serialize(Stream &s) {
            bool result = s & changed;
               S(SHOTS, shots);
               S(HITS, hits);
               S(KILLS, kills);
               S(DEATHS, deaths);
               S(ASSISTANCES, assistances);
               S(WINS, wins);
               S(PENALTIES, penalties);
               S(GAMES, games);
               S(TIME_ALIVE, timeAlive);
               S(TOTAL_GAMETIME, totalGameTime);
               S(TOTAL_DAMAGE, totalDamage);
               S(ASSISTED_DAMAGE, assistedDamage);
               S(ELO, elo);
               S(ELO_TREND, eloTrend);
               S(ELO_GAMES, eloGames);
            return result;
        }
    };
}

#endif /* SOURCE_NET_OBJECT_PLAYERSCORE_H_ */
