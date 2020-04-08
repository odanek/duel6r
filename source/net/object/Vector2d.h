
#ifndef SOURCE_NET_OBJECT_VECTOR2D_H_
#define SOURCE_NET_OBJECT_VECTOR2D_H_

#include "../../Type.h"

namespace Duel6::net {
    struct Vector2D {
        Float32 x, y;
        Vector2D(Float32 x, Float32 y):x(x),y(y){}
        Vector2D() = default;
        Vector2D(const Vector2D &) = default;
        Vector2D(Vector2D &&) = default;
        Vector2D & operator = (const Vector2D &) = default;
        Vector2D & operator = (Vector2D &&) = default;
        bool operator == (const Vector2D &r) const{
            return x == r.x && y == r.y;
        }
        template<class V>
        Vector2D(V v)
            : x(v.x),
              y(v.y){
        }

        template<class V>
        operator V() {
            return V(x, y, 0);
        }

        template<class Stream>
        bool serialize(Stream &s) {
            bool result = s & x &&
                s & y;
            return result;
        }
    };
}
#endif /* SOURCE_NET_OBJECT_VECTOR_H_ */
