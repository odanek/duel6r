
#ifndef SOURCE_NET_MASTER_PEERLIST_H_
#define SOURCE_NET_MASTER_PEERLIST_H_

#include <string>
#include <vector>
#include <enet/enet.h>
namespace masterserver {
    struct PeerEntry {
        enet_uint32 address;
        enet_uint16 port;
    };

    typedef std::vector<PeerEntry> peerlist_t;
}

#endif /* SOURCE_NET_MASTER_SERVERLIST_H_ */
