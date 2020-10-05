/*
 * EraseShot.h
 *
 *  Created on: May 17, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_ERASESHOT_H_
#define SOURCE_NET_EVENT_ERASESHOT_H_

#include "../Event.h"
#include "../Type.h"
namespace Duel6::net {

    class EraseShot: public Event<EraseShot, EventType::SHOT_ERASE> {
    public:
        object_id_t id;

        template<typename Stream>
        bool serialize(Stream &s){
            return s & id;
        }
    };
}



#endif /* SOURCE_NET_EVENT_ERASESHOT_H_ */
