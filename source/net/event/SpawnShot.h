/*
 * SpawnShot.h
 *
 *  Created on: May 12, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_SPAWNSHOT_H_
#define SOURCE_NET_EVENT_SPAWNSHOT_H_

#include "../Event.h"
#include "../Type.h"
#include "../../Type.h"
#include "../object/Shot.h"
#include "../object/Vector2d.h"
namespace Duel6::net {
    class SpawnShot: public Event<SpawnShot, EventType::SPAWN_SHOT>{
    public:
        Uint8 weaponId;
        object_id_t playerId;
        Shot shot;
        bool orientationLeft;
        bool powerful;
        Int32 power;
        Float32 bulletSpeed;
        Vector2D velocity;

        template<typename Stream>
        bool serialize(Stream &s) {
            return s & weaponId
                && s & playerId
                && s & shot
                && s & orientationLeft
                && s & powerful
                && s & power
                && s & bulletSpeed
                && s & velocity;
        }
    };
}



#endif /* SOURCE_NET_EVENT_SPAWNSHOT_H_ */
