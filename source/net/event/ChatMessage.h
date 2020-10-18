/*
 * Message.h
 *
 *  Created on: Aug 27, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_CHATMESSAGE_H_
#define SOURCE_NET_EVENT_CHATMESSAGE_H_

#include "../Event.h"
#include "../Type.h"
#include "../../Type.h"

namespace Duel6::net {
    class Peer;
    class ChatMessage: public Event<ChatMessage, EventType::CHAT_MESSAGE> {
    public:
        bool system;
        std::string origin;
        std::string text;
        template<typename Stream>
        bool serialize(Stream &s) {
            return s & system
                && s & origin
                && s & text;
        }
    };
}

#endif /* SOURCE_NET_OBJECT_MESSAGE_H_ */
