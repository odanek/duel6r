/*
 * PickWeapon.h
 *
 *  Created on: May 11, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_PICKWEAPON_H_
#define SOURCE_NET_EVENT_PICKWEAPON_H_

#include "../Event.h"
#include "../object/Weapon.h"

namespace Duel6::net {
    class PickWeapon: public Event<PickWeapon, EventType::PICK_WEAPON> {
    public:
        Int32 weaponId;
        object_id_t playerId;
        template<typename Stream>
        bool serialize(Stream &s) {
            return s & weaponId
                && s & playerId;
        }
    };
}



#endif /* SOURCE_NET_EVENT_PICKWEAPON_H_ */
