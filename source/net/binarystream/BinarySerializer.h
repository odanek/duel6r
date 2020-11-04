/*
 * BinarySerializer.h
 *
 *  Created on: Jan 11, 2017
 *      Author: fanick
 */
#ifndef BINARYSTREAMS_BINARYSERIALIZER_H
#define BINARYSTREAMS_BINARYSERIALIZER_H
#include <stdint.h>

template <typename binarystream>
class BinarySerializer {

private:
    binarystream &s;

public:
    BinarySerializer(binarystream &s)
        : s(s) {
    }
    //TODO Constructors
    template<typename M>
    bool operator &(const M &m) {
        return s << m;
    }

    template<typename M>
    bool operator &(M &m) {
        return s << m;
    }

    constexpr static bool isSerializer() {
        return true;
    }
    constexpr static bool isDeserializer() {
        return false;
    }
};

#endif
