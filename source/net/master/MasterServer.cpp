#include "MasterServer.h"
#include "stun.h"

namespace masterserver {

    void MasterServer::writeHeader(binarystream &bs, PACKET_TYPE packetType) {
        packetHeader header;
        header.type = packetType;
        bs << header;
    }

    void MasterServer::send(binarystream &bs) {
        std::string dataStr = bs.str();
        ENetPacket *enetPacket = enet_packet_create(dataStr.c_str(), dataStr.length(), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, enetPacket);
    }

    binarystream MasterServer::createHeader(PACKET_TYPE packetType) {
        binarystream bs;
        writeHeader(bs, packetType);
        return bs;
    }
    packet_update MasterServer::createUpdateRequest(const std::string &description, address_t localAddress,
                                                    port_t localPort, enet_uint32 publicIPAddress,
                                                    enet_uint16 publicPort, bool needsNAT) {
        packet_update p;
        p.descr = description;
        p.localNetworkAddress = localAddress;
        p.localNetworkPort = localPort;
        p.publicIPAddress = publicIPAddress;
        p.publicPort = publicPort;
        p.needsNAT = needsNAT;
        return p;
    }

    packet_nat_punch MasterServer::createNatPunchRequest(address_t address, port_t port, address_t localAddress, port_t localPort) {
        packet_nat_punch p;
        p.address = address;
        p.port = port;
        p.clientLocalNetworkAddress = localAddress;
        p.clientLocalNetworkPort = localPort;
        return p;
    }

    void MasterServer::requestServerList(ENetHost *host, serverListReceivedCallback_t callback) {
        connect(host, REQUEST_TYPE::CLIENT_REQUEST_SERVERLIST);
        onServerListReceivedCallback = [=](serverlist_t &serverlist) {
            callback(serverlist);
            disconnect();
            onServerListReceivedCallback = [](serverlist_t&) {
            };
        };
    }
    void MasterServer::setAddressAndPort(const std::string &address, int port) {
        this->address = address;
        this->port = port;
    }

    void MasterServer::setPeer(ENetPeer *peer) {
        this->peer = peer;
    }

    void MasterServer::sendHeartBeat() {
        reconnect(REQUEST_TYPE::SERVER_REGISTER);
    }

    void MasterServer::update(ENetHost *host, const std::string &description,
                              address_t localAddress, port_t localPort,
                              address_t publicIPAddress, port_t publicPort,
                              bool needsNAT) {
        connect(host, REQUEST_TYPE::SERVER_UPDATE);
        onConnected([=]() {
            binarystream bs = createHeader(PACKET_TYPE::SERVER_UPDATE);
            bs << createUpdateRequest(description, localAddress, localPort, publicIPAddress, publicPort, needsNAT);
            send(bs);
        });
    }

    void MasterServer::MasterServer::requestNatPeers() {
        reconnect(REQUEST_TYPE::SERVER_NAT_GET_PEERS);
    }

    void MasterServer::connectNatToServer(ENetHost *host, address_t address, port_t port, address_t localAddress, port_t localPort) {
        connect(host, REQUEST_TYPE::CLIENT_NAT_CONNECT_TO_SERVER);

        onConnected([this, address, port, localAddress, localPort]() {
            binarystream bs = createHeader(PACKET_TYPE::CLIENT_NAT_PUNCH);
            bs << createNatPunchRequest(address, port, localAddress, localPort);
            send(bs);
            this->disconnect();
        });
    }

    void MasterServer::onConnected(ENetPeer *peer) {
        setPeer(peer);
        for(auto & cb: connectedCallbacks){
            cb();
        }
        connectedCallbacks.clear();
    }

    void MasterServer::onReceived(ENetPeer *peer, ENetPacket *packet) {
        binarystream bs(packet->data, packet->dataLength);
        packetHeader header;
        bs >> header;

        switch (header.type) {
        case PACKET_TYPE::SERVER_UPDATE: {
            break;
        }
        case PACKET_TYPE::SERVER_LIST: {
            printf("It's a server list!\n");
            packet_serverlist s;
            bs >> s;
            serverlist_t serverList;
            serverList.reserve(s.servers.size());
            for (auto &server : s.servers) {
                printf(" descr: %s , addr: %s, port: %u \n", server.descr.c_str(), hostToIPaddress(server.address, server.port).c_str(), server.port);
                serverList.emplace_back(
                    ServerEntry { server.address, server.port,
                    server.publicIPAddress, server.publicPort,
                    server.localNetworkAddress, server.localNetworkPort,
                    server.descr, server.needsNAT
                });
            }
            onServerListReceivedCallback(serverList);
            break;
        }

        case PACKET_TYPE::SERVER_NAT_PEERS: {
//            printf("NAT peers received\n");
            packet_nat_peers p;
            bs >> p;
            peerlist_t peerList;
            peerList.reserve(p.peers.size());
            for (auto &peer : p.peers) {
                printf(" addr: %s / %s \n", hostToIPaddress(peer.address, peer.port).c_str(),
                    hostToIPaddress(peer.localNetworkAddress, peer.localNetworkPort).c_str());
                peerList.emplace_back(PeerEntry { peer.address, peer.port, peer.localNetworkAddress, peer.localNetworkPort });
            }

            onPeerListReceivedCallback(peerList, p.yourPublicAddress, p.yourPublicPort);
            this->disconnect();
            break;
        }
        case PACKET_TYPE::CLIENT_NAT_PUNCH:
        case PACKET_TYPE::PACKETS_COUNT: {
            break;
        }
        }
    }

    void MasterServer::onPeerListReceived(peerListReceivedCallback_t callback) {
        onPeerListReceivedCallback = callback;
        ENetSocket s = host->socket;

        ENetBuffer buffer;
        enet_uint32 data = 0xdeadbabe;
        buffer.data = &data;
        buffer.dataLength = sizeof(data);
        // fingers crossed
        enet_socket_send(s, &peer->address, &buffer, 1);
        enet_socket_send(s, &peer->address, &buffer, 1);
    }

    void MasterServer::disconnect() {
        if (this->peer != nullptr) {
            enet_peer_disconnect_later(this->peer, 0);
        }
    }

    void MasterServer::reconnect(REQUEST_TYPE requestType) {
        ENetAddress masterAddress;
        enet_address_set_host(&masterAddress, address.c_str());
        masterAddress.port = port;
        if(this->peer != nullptr && this->peer->state == ENET_PEER_STATE_CONNECTING) {
         //   std::cerr << "Whoa, we still haven't connected to the master server from previous attempt\n";
            return;
        };
        ENetPeer *peer = enet_host_connect(host, &masterAddress, 1, static_cast<enet_uint32>(requestType));
        if(peer == nullptr){
            std::cerr << "Whoa, we can't create additional peer to communicate with the master server\n";
            return;
        }
        Duel6::net::PeerRef *ref = new Duel6::net::PeerRef();
        peer->data = ref;
        ref->isMasterServer = true;
        ref->peer = nullptr;
        ref->pos = 0;
        this->peer = peer;
    }

    void MasterServer::onConnected(connectedCallback_t callback) {
        connectedCallbacks.push_back(callback);
        if(connectedCallbacks.size() > 5){
            connectedCallbacks.pop_front();
        }
    }

    ENetPeer* MasterServer::connect(ENetHost *host, REQUEST_TYPE requestType) {
        this->host = host;
        reconnect(requestType);
        return this->peer;
    }

    void MasterServer::sendStunBindingRequest(ENetSocket s) {
        ENetAddress masterAddress;
        enet_address_set_host(&masterAddress, this->address.c_str());
        masterAddress.port = this->port;
    }

    void MasterServer::sendStunBindingRequest(ENetSocket s, enet_uint32 address, enet_uint16 port) {
        stun::message message;
        message.type = stun::type_t::BINDING_REQUEST;
        stun::changeAttribute ca;
        ca.flags = 0;
        message.changeAttributes.push_back(ca);
        char *mem;
        size_t len = message.send(&mem);
        ENetAddress peerAddress;
        peerAddress.host = address;
        peerAddress.port = port;

        ENetBuffer buffer;
        buffer.data = mem;
        buffer.dataLength = len;
        // fingers crossed
        enet_socket_send(s, &peerAddress, &buffer, 1);
        free(mem);
    }

}
