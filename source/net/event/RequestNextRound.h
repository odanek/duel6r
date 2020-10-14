/*
 * RequestNextRound.h
 *
 *  Created on: Oct 14, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_REQUESTNEXTROUND_H_
#define SOURCE_NET_EVENT_REQUESTNEXTROUND_H_

#include "../Event.h"
#include "../../Type.h"

namespace Duel6::net {
    class RequestNextRound : public Event<RequestNextRound, EventType::CLIENT_REQUESTS_NEXT_ROUND> {
    public:
        int32_t currentRound;
        template<typename Stream>
        bool serialize(Stream &s) {
            return s & currentRound;
        }
    };
}

#endif /* SOURCE_NET_EVENT_REQUESTNEXTROUND_H_ */
