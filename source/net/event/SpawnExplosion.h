/*
 * SpawnExplosion.h
 *
 *  Created on: May 17, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_SPAWNEXPLOSION_H_
#define SOURCE_NET_EVENT_SPAWNEXPLOSION_H_
#include "Color.h"
#include "../Event.h"
#include "../Type.h"
#include "../../Type.h"
#include "../object/Vector2d.h"
namespace Duel6::net {


    class SpawnExplosion: public Event<SpawnExplosion, EventType::SPAWN_EXPLOSION> {
    public:
        Vector2D centre;
        Float32 now;
        Float32 max;
        Color color;
        template<typename Stream>
        bool serialize(Stream &s) {
            return s & centre &&
                s & now &&
                s & max &&
                s & color;
        }
    };
}



#endif /* SOURCE_NET_EVENT_SPAWNEXPLOSION_H_ */
