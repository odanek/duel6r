/*
 * SpawnBonus.h
 *
 *  Created on: May 10, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_SPAWNBONUS_H_
#define SOURCE_NET_EVENT_SPAWNBONUS_H_

#include "../Event.h"
#include "../object/Bonus.h"
namespace Duel6::net {

    class SpawnBonus: public Event<SpawnBonus, EventType::SPAWN_BONUS> {
    public:
        Bonus bonus;

        template<typename Stream>
        bool serialize(Stream &s) {
            return s & bonus;
        }
    };
}

#endif /* SOURCE_NET_EVENT_SPAWNBONUS_H_ */
