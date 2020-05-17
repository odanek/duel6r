/*
 * Color.h
 *
 *  Created on: May 17, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_COLOR_H_
#define SOURCE_NET_EVENT_COLOR_H_
#include "../../Type.h"
namespace Duel6::net {
    class Color {
    public:
        Uint8 red;
        Uint8 green;
        Uint8 blue;
        Uint8 alpha;
        template<typename Stream>
        bool serialize(Stream &s) {
            return s & red &&
                s & green &&
                s & blue &&
                s & alpha;
        }
    };
}




#endif /* SOURCE_NET_EVENT_COLOR_H_ */
