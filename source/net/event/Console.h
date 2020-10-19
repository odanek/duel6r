/*
 * Console.h
 *
 *  Created on: Oct 18, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_CONSOLE_H_
#define SOURCE_NET_EVENT_CONSOLE_H_

#include "../Event.h"

namespace Duel6::net {
    class Console: public Event<Console, EventType::CONSOLE> {
    public:
        bool value = false;
        template<typename Stream>
        bool serialize(Stream &s) {
            return s & value;
        }
    };
}

#endif /* SOURCE_NET_EVENT_CONSOLE_H_ */
