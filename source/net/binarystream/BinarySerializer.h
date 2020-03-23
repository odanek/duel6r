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
class BinarySerializer{
private:
    binarystream & s;
public:
    BinarySerializer(binarystream & s): s(s){}
    //TODO Constructors
    template<typename M>
    bool operator & (const M & m){
        s << m;
        return s.good();
    }

    template<typename M>
    bool operator & (M & m){
        s << m;
        return s.good();
    }

    template<typename M>
    bool safe_max(M & m, const M max) {
        // SHOULD NOT BE CALLED
        return false;
    }

    template<typename M>
    bool safe_min(M & m, const M min) {
        // SHOULD NOT BE CALLED
        return false;
    }

    template<typename M>
    bool safe_range(M & m, const M min, const M max) {
        // SHOULD NOT BE CALLED
        return false;
    }

    template<typename M>
    bool safe_size(M & m, const uint32_t min, const uint32_t max) {
        // SHOULD NOT BE CALLED
        return false;
    }

    template<typename M, typename N>
    bool safe_read(M & m, const N val_min, const N val_max, const uint32_t min, const uint32_t max) {
        // SHOULD NOT BE CALLED
        return false;
    }

    constexpr static bool isSerializer() {
        return true;
    }
    constexpr static bool isDeserializer() {
        return false;
    }
};

#endif
