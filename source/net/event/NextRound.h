/*
 * NextRound.h
 *
 *  Created on: Mar 22, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_NEXTROUND_H_
#define SOURCE_NET_EVENT_NEXTROUND_H_
#include "../Event.h"

namespace Duel6::net {
    class NextRound: public Event<NextRound, EventType::NEXT_ROUND> {
    public:
        int foobar = 42;
        NextRound(){}

        template<typename Stream>
        bool serialize(Stream & s){
            return s & foobar;
        }
    };
}




#endif /* SOURCE_NET_EVENT_NEXTROUND_H_ */
