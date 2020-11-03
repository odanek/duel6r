/*
 * DoubleJumpEffects.h
 *
 *  Created on: Nov 3, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_DOUBLEJUMPEFFECTS_H_
#define SOURCE_NET_EVENT_DOUBLEJUMPEFFECTS_H_

#include "../Event.h"
#include "../Type.h"
namespace Duel6::net {

    class DoubleJumpEffects : public Event<DoubleJumpEffects, EventType::PLAYER_DOUBLE_JUMP_EFFECTS> {
    public:
        float x,y,angle;
        uint16_t playerId;

        template<typename Stream>
        bool serialize(Stream &s) {
            return s & playerId
                && s & x
                && s & y
                && s & angle;
        }
    };

}

#endif /* SOURCE_NET_EVENT_DOUBLEJUMPEFFECTS_H_ */
