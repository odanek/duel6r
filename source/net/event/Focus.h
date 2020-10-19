/*
 * Focus.h
 *
 *  Created on: Oct 18, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_FOCUS_H_
#define SOURCE_NET_EVENT_FOCUS_H_

#include "../Event.h"

namespace Duel6::net {
    class Focus: public Event<Focus, EventType::FOCUS> {
    public:
        bool value = false;
        template<typename Stream>
        bool serialize(Stream &s) {
            return s & value;
        }
    };
}

#endif /* SOURCE_NET_EVENT_FOCUS_H_ */
