/*
 * SpawnBonus.h
 *
 *  Created on: May 10, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_SPAWNWEAPON_H_
#define SOURCE_NET_EVENT_SPAWNWEAPON_H_

#include "../Event.h"
#include "../object/Weapon.h"
namespace Duel6::net {

    class SpawnWeapon: public Event<SpawnWeapon, EventType::SPAWN_WEAPON> {
    public:
        Weapon weapon;

        template<typename Stream>
        bool serialize(Stream &s) {
            return s & weapon;
        }
    };
}

#endif
