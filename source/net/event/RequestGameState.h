/*
 * RequestGameState.h
 *
 *  Created on: Mar 19, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_REQUESTGAMESTATE_H_
#define SOURCE_NET_EVENT_REQUESTGAMESTATE_H_

#include <array>
#include "../Event.h"
#include "../../Type.h"
#include "PlayerProfile.h"
namespace Duel6::net {

    class RequestGameState: public Event<RequestGameState, EventType::REQUEST_GAME_STATE> {

    public:

        std::vector<PlayerProfile> connectingPlayers;

        RequestGameState() {

        }

        template<typename Stream>
        bool serialize(Stream &bs) {
            return bs & connectingPlayers;
        }
    };

}

#endif /* SOURCE_NET_EVENT_REQUESTGAMESTATE_H_ */
