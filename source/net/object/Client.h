/*
 * Client.h
 *
 *  Created on: Mar 19, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_CLIENT_H_
#define SOURCE_NET_OBJECT_CLIENT_H_

#include <list>
#include "Player.h"
namespace Duel6::net {
    class Client: public Object<Client, ObjectType::CLIENT> {
    public:
        object_id_t id;
        std::string name;
        std::list<int> players;

        Client() {
        }
        template<class Stream>
        bool serialize(Stream &s) {
            return s & id
                && s & name
                && s & players;
        }
    };
}

#endif /* SOURCE_NET_OBJECT_CLIENT_H_ */
