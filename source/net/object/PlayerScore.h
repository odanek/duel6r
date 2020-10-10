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
        PlayerScore();
        void loadFromPlayer(Player &player);
        void unloadToPlayer(Player &player);
        bool operator ==(const PlayerScore &r) const;
        Int32 shots = 0;
        Int32 hits = 0;
        Int32 kills = 0;
        Int32 roundKills = 0;
        Int32 deaths = 0;
        Int32 assistances = 0;
        Int32 wins = 0;
        Int32 penalties = 0;
        Int32 games = 0;
        Int32 timeAlive = 0;
        Int32 totalGameTime = 0;
        Int32 totalDamage = 0;
        Int32 assistedDamage = 0;
        Int32 elo = 0;
        Int32 eloTrend = 0;
        Int32 eloGames = 0;

        enum FIELDS {  // TODO implement diff/fillin same way as in net::Player
            NO_CHANGE,
            SHOTS,
            HITS,
            KILLS,
            ROUND_KILLS,
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

        static void diff(PlayerScore &snapshot, const PlayerScore &confirmed);

        static void fillinFromPreviousConfirmed(const PlayerScore &confirmed, PlayerScore &received);

#define S(POS, FIELD) if(changed[POS]) result &= s & FIELD
        template<typename Stream>
        bool serialize(Stream &s) {
            bool result = s & changed;
               if (changed[NO_CHANGE]) return result;
               S(SHOTS, shots);
               S(HITS, hits);
               S(KILLS, kills);
               S(ROUND_KILLS, roundKills);
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
