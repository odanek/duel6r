/*
 * RequestGameState.h
 *
 *  Created on: Mar 19, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_REQUESTGAMESTATE_H_
#define SOURCE_NET_EVENT_REQUESTGAMESTATE_H_

#include "../Event.h"
namespace Duel6::net {
    class RequestGameState  : public Event<RequestGameState, EventType::REQUEST_GAME_STATE> {
    private:
         bool request = false;
    public:
        RequestGameState(){

        }

        template<typename Stream>
        bool serialize(Stream & bs){
            return bs & request;
        }
    };
}



#endif /* SOURCE_NET_EVENT_REQUESTGAMESTATE_H_ */
