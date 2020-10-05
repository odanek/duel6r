/*
 * ServerList.h
 *
 *  Created on: Sep 1, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_MASTER_SERVERLIST_H_
#define SOURCE_NET_MASTER_SERVERLIST_H_

#include <string>
#include <vector>
#include <enet/enet.h>
namespace masterserver {
    struct ServerEntry {
        enet_uint32 address;
        enet_uint16 port;
        enet_uint32 publicIPAddress;
        enet_uint16 publicPort;
        enet_uint32 localNetworkAddress;
        enet_uint16 localNetworkPort;
        std::string description;
        bool enableNAT;
    };

    typedef std::vector<ServerEntry> serverlist_t;
}

#endif /* SOURCE_NET_MASTER_SERVERLIST_H_ */
