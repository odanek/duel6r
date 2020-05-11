/*
 *
 *  Created on: May 11, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_PICKBONUS_H_
#define SOURCE_NET_EVENT_PICKBONUS_H_

#include "../Event.h"
#include "../object/Weapon.h"

namespace Duel6::net {
    class PickBonus: public Event<PickBonus, EventType::PICK_BONUS> {
    public:
        Int32 bonusId;
        object_id_t playerId;
        template<typename Stream>
        bool serialize(Stream &s) {
            return s & bonusId
                && s & playerId;
        }
    };
}



#endif
