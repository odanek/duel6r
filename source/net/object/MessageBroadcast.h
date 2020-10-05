/*
 * Message.h
 *
 *  Created on: Aug 27, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_MESSAGEBROADCAST_H_
#define SOURCE_NET_OBJECT_MESSAGEBROADCAST_H_

#include "../Object.h"
#include "../Type.h"
#include "../../Type.h"

namespace Duel6::net {

    class MessageBroadcast: public Object<MessageBroadcast, ObjectType::MESSAGE> {
    public:
        Int32 playerId;
        std::string text;

        template<typename Stream>
        bool serialize(Stream &s) {
            return s & playerId
                && s & text;
        }
    };
}

#endif /* SOURCE_NET_OBJECT_MESSAGE_H_ */
