/*
 * Vector.h
 *
 *  Created on: Mar 15, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_VECTOR_H_
#define SOURCE_NET_OBJECT_VECTOR_H_

#include "../../Type.h"

namespace Duel6::net {
    struct Vector {
        Float32 x, y, z;
        Vector(Float32 x, Float32 y, Float32 z):x(x),y(y),z(z){}
        Vector() = default;
        Vector(const Vector &) = default;
        Vector(Vector &&) = default;
        Vector & operator = (const Vector &) = default;
        Vector & operator = (Vector &&) = default;
        template<class V>
        Vector(V v)
            : x(v.x),
              y(v.y),
              z(v.z) {
        }

        template<class V>
        operator V() {
            return V(x, y, z);
        }

        template<class Stream>
        bool serialize(Stream &s) {
            bool result = s & x &&
                s & y &&
                s & z;
            return result;
        }
    };
}
#endif /* SOURCE_NET_OBJECT_VECTOR_H_ */
