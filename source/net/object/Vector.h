/*
 * Vector.h
 *
 *  Created on: Mar 15, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_VECTOR_H_
#define SOURCE_NET_OBJECT_VECTOR_H_

struct Vector_Net {
    float x, y, z;

    template<class Stream>
    bool serialize(Stream &s) {
        bool result = s & x &&
            s & y &&
            s & z;
        return result;
    }
};

#endif /* SOURCE_NET_OBJECT_VECTOR_H_ */
