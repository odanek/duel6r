/*
 * Shot.h
 *
 *  Created on: Mar 15, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_SHOT_H_
#define SOURCE_NET_OBJECT_SHOT_H_

#include "../Object.h"
#include "Vector.h"

namespace Duel6::net {

    class Shot: public Object<Shot, ObjectType::SHOT> {
    public:
        object_id_t id = 0;
        Vector position;

        Shot() {
        }
        template<class Stream>
        bool serialize(Stream &s) {
            return s & id
                && s & position;
        }
    };
}
#endif /* SOURCE_NET_OBJECT_SHOT_H_ */
