/*
 * StartRound.h
 *
 *  Created on: Mar 30, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_STARTROUND_H_
#define SOURCE_NET_EVENT_STARTROUND_H_

#include "../Event.h"
#include "../object/Level.h"

namespace Duel6::net {
    class StartRound: public Event<StartRound, EventType::ROUND_START>{
    public:
        Int32 round;
        Level world;

        template<class Stream>
        bool serialize(Stream &s) {
            return s & round
                && s & world;
        }
    };
}



#endif /* SOURCE_NET_EVENT_STARTROUND_H_ */
