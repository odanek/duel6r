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

//        enum FIELDS {  TODO implement diff/fillin same way as in net::Player
//            SHOTS,
//            HITS,
//            KILLS,
//            DEATHS,
//            ASSISTANCES,
//            WINS,
//            PENALTIES,
//            GAMES,
//            TIME_ALIVE,
//            TOTAL_GAMETIME,
//            TOTAL_DAMAGE,
//            ASSITED_DAMAGE,
//            ELO,
//            ELO_TREND,
//            ELO_GAMES,
//            _SIZE
//        };
//        std::bitset<FIELDS::_SIZE> changed;
        template<typename Stream>
        bool serialize(Stream &s) {
            return s & shots
                && s & hits
                && s & kills
                && s & deaths
                && s & assistances
                && s & wins
                && s & penalties
                && s & games
                && s & timeAlive
                && s & totalGameTime
                && s & totalDamage
                && s & assistedDamage
                && s & elo
                && s & eloTrend
                && s & eloGames;
        }
    };
}

#endif /* SOURCE_NET_OBJECT_PLAYERSCORE_H_ */
