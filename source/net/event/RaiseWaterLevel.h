/*
 * RaiseWaterLevel.h
 *
 *  Created on: May 17, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_RAISEWATERLEVEL_H_
#define SOURCE_NET_EVENT_RAISEWATERLEVEL_H_

#include "../Event.h"
namespace Duel6::net {
    class RaiseWaterLevel : public Event<RaiseWaterLevel, EventType::RAISE_WATER> {

    public:
        template<typename Stream>
        bool serialize(Stream &s) {
            return true;
        }
    };
}



#endif /* SOURCE_NET_EVENT_RAISEWATERLEVEL_H_ */
