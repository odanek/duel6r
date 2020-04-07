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
        Vector acceleration;
        Vector externalForces;
        Vector externalForcesSpeed;
        Vector velocity;

        Uint32 controls;
        Uint32 flags;
        Float32 life;
        Float32 air;
        Int32 ammo;
        Uint8 weaponId;
        std::array<uint32_t, 16> unconfirmedInputs;
        bool orientationLeft;
        Player() {
        }
        template<class Stream>
        bool serialize(Stream &s) {
            return s & id // @suppress("Suggested parenthesis around expression")
                && s & clientLocalId // @suppress("Suggested parenthesis around expression")
                && s & position // @suppress("Suggested parenthesis around expression")
                && s & acceleration // @suppress("Suggested parenthesis around expression")
                && s & externalForces // @suppress("Suggested parenthesis around expression")
                && s & externalForcesSpeed // @suppress("Suggested parenthesis around expression")
                && s & velocity // @suppress("Suggested parenthesis around expression")
                && s & controls // @suppress("Suggested parenthesis around expression")
                && s & flags // @suppress("Suggested parenthesis around expression")
                && s & life // @suppress("Suggested parenthesis around expression")
                && s & air // @suppress("Suggested parenthesis around expression")
                && s & ammo // @suppress("Suggested parenthesis around expression")
                && s & weaponId // @suppress("Suggested parenthesis around expression")
                && s & unconfirmedInputs // @suppress("Suggested parenthesis around expression")
                && s & orientationLeft; // @suppress("Suggested parenthesis around expression")
        }
    };
}

#endif /* SOURCE_NET_OBJECT_PLAYER_H_ */
