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
namespace Duel6::net {


    class RequestGameState: public Event<RequestGameState, EventType::REQUEST_GAME_STATE> {

    public:
        enum Hair {
                   Normal = 0,
                   None,
                   Short,
                   MAX_COUNT
               };

        class Color {
        public:
            Uint8 red;
            Uint8 green;
            Uint8 blue;
            Uint8 alpha;
            template<typename Stream>
            bool serialize(Stream &s) {
                return s & red &&
                    s & green &&
                    s & blue &&
                    s & alpha;
            }
        };
        class Skin {
        public:

            Hair hair = Normal;
            std::array<Color, 11> colors;
            bool headBand = false;

            template<typename Stream>
            bool serialize(Stream &s) {
                return s & hair &&
                    s & colors &&
                    s & headBand;
            }
        };
        class Player {
        public:
            Int32 clientId; // for pairing inputs from client
            std::string name;
            Int32 team;
            Skin skin;


            template<typename Stream>
            bool serialize(Stream &s) {
                return s & clientId &&
                    s & name &&
                    s & team &&
                    s & skin;
            }
        };

        std::vector<Player> connectingPlayers;

        RequestGameState() {

        }

        template<typename Stream>
        bool serialize(Stream &bs) {
            return bs & connectingPlayers;
        }
    };

    template<typename Stream>
    bool serialize(Stream &s, RequestGameState::Hair &o) {
        if (s.isDeserializer()) {
            type_t r;
            if (!s.safe_max(r, (type_t) (static_cast<type_t>( RequestGameState::Hair::MAX_COUNT) - 1))) {
                return false;
            }
            o = static_cast< RequestGameState::Hair>(r);
            return true;
        } else {
            type_t r = static_cast<type_t>(o);
            return s & r;
        }
    }
}

#endif /* SOURCE_NET_EVENT_REQUESTGAMESTATE_H_ */
