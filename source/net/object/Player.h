/*
 * Player.h
 *
 *  Created on: Mar 15, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_PLAYER_H_
#define SOURCE_NET_OBJECT_PLAYER_H_
#include "../Object.h"
#include "Vector.h"

namespace Duel6::net {
    class Player: public Object<Player, ObjectType::PLAYER> {
    public:
        object_id_t id = 0;
        Int32 clientLocalId = 0;
        Vector position;
        uint32_t controls;
        Player() {
        }
        template<class Stream>
        bool serialize(Stream &s) {
            return s & id // @suppress("Suggested parenthesis around expression")
                && s & clientLocalId // @suppress("Suggested parenthesis around expression")
                && s & position // @suppress("Suggested parenthesis around expression")
                && s & controls; // @suppress("Suggested parenthesis around expression")
        }
    };
}

#endif /* SOURCE_NET_OBJECT_PLAYER_H_ */
