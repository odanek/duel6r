/*
 * BinaryDeserializer.h
 *
 *  Created on: Jan 11, 2017
 *      Author: fanick
 */
#ifndef BINARYSTREAMS_BINARYDESERIALIZER_H
#define BINARYSTREAMS_BINARYDESERIALIZER_H
#include <stdint.h>

//TODO add safe string read (max len etc.)
template <typename binarystream>
class BinaryDeserializer {
private:
    binarystream & stream;
public:
    BinaryDeserializer(binarystream & s):stream(s){}

    template<typename M>
    bool operator &(M & m) {
        return stream >> m;
    }

    template<typename M>
    bool safe_max(M & m, const M max) {
        return (stream >> m && m <= max);
    }

    template<typename M>
    bool safe_min(M & m, const M min) {
        return (stream >> m && m >= min);
    }

    template<typename M>
    bool safe_range(M & m, const M min, const M max) {
        return (stream >> m && ( m >= min && m <= max));
    }

    template<typename M>
    bool safe_size(M & m, const uint32_t min, const uint32_t max) {
        return stream.s_read(m, min, max);
    }
    template<typename M, typename N>
    bool safe_read(M & m, const N val_min, const N val_max, const uint32_t min, const uint32_t max) {
        return stream.s_read(m, val_min, val_max, min, max);
    }
    constexpr static bool isSerializer() {
        return false;
    }
    constexpr static bool isDeserializer() {
        return true;
    }
};
#endif
