/*
 * PlayerProfile.h
 *
 *  Created on: Mar 31, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_PLAYERPROFILE_H_
#define SOURCE_NET_EVENT_PLAYERPROFILE_H_

namespace Duel6::net {

    class PlayerProfile {
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
        Int32 playerId;
        Int32 clientId; // for pairing inputs from client
        Int32 clientLocalId;
        std::string name;
        Int32 team;
        Skin skin;

        template<typename Stream>
        bool serialize(Stream &s) {
            return
                s & playerId &&
                s & clientId &&
                s & clientLocalId &&
                s & name &&
                s & team &&
                s & skin;
        }
    };
    template<typename Stream>
    bool serialize(Stream &s, PlayerProfile::Hair &o) {
        if (s.isDeserializer()) {
            type_t r;
            if (!s.safe_max(r, (type_t)(static_cast<type_t>(PlayerProfile::Hair::MAX_COUNT) - 1))) {
                return false;
            }
            o = static_cast<PlayerProfile::Hair>(r);
            return true;
        } else {
            type_t r = static_cast<type_t>(o);
            return s & r;
        }
    }
}

#endif /* SOURCE_NET_EVENT_PLAYERPROFILE_H_ */
