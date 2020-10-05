/*
 * World.h
 *
 *  Created on: Mar 29, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_WORLD_H_
#define SOURCE_NET_OBJECT_WORLD_H_

#include "../../Type.h"
#include "../Object.h"
#include "Vector.h"
#include "Level.h"

namespace Duel6::net{
    class World : public Object<World, ObjectType::WORLD>{
    public:
        class GameSettings {
        public:
            Uint32 maxRounds;
            bool quickLiquid;
            bool globalAssistances;
            std::set<Uint8> enabledWeapons;

            GameSettings() = default;
            GameSettings(const GameSettings &) = default;
            GameSettings(GameSettings &&) = default;
            GameSettings& operator = (const GameSettings &) = default;
            GameSettings& operator = (GameSettings &&) = default;

            template<class Source>
            GameSettings(const Source &s):
                  maxRounds(s.getMaxRounds()),
                  quickLiquid(s.isQuickLiquid()),
                  globalAssistances(s.isGlobalAssistances()),
                  enabledWeapons(s.getEnabledWeapons()) {
            }

            template<class Stream>
            bool serialize(Stream &s) {
                return s & maxRounds &&
                    s & quickLiquid &&
                    s & globalAssistances &&
                    s & enabledWeapons;
            }

        };

        GameSettings gameSettings;
        Level level;
        Int32 currentRound;
        Int32 playedRounds;


        template<class Stream>
        bool serialize(Stream &s) {
            return s & gameSettings &&
                s & level &&
                s & currentRound &&
                s & playedRounds;
        }
    };
}



#endif /* SOURCE_NET_OBJECT_WORLD_H_ */
