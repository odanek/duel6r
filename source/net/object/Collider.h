/*
 * Collider.h
 *
 *  Created on: May 10, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_COLLIDER_H_
#define SOURCE_NET_OBJECT_COLLIDER_H_
#include "Vector2d.h"

namespace Duel6::net {
    class Collider {
    public:
        Vector2D position;
        Vector2D acceleration;
        Vector2D externalForces;
        Vector2D externalForcesSpeed;
        Vector2D velocity;

        template<typename Stream>
        bool serialize(Stream &s) {
            return s & position
                && s & acceleration
                && s & externalForces
                && s & externalForcesSpeed
                && s & velocity;
        }
    };
}



#endif /* SOURCE_NET_OBJECT_COLLIDER_H_ */
