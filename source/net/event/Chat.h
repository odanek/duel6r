/*
 * Chat.h
 *
 *  Created on: Oct 18, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_CHAT_H_
#define SOURCE_NET_EVENT_CHAT_H_


#include "../Event.h"

namespace Duel6::net {
    class Chat: public Event<Chat, EventType::CHAT> {
    public:
        bool value = false;
        template<typename Stream>
        bool serialize(Stream &s) {
            return s & value;
        }
    };
}



#endif /* SOURCE_NET_EVENT_CHAT_H_ */
