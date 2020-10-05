/*
 * MasterServer.h
 *
 *  Created on: Sep 1, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_MASTER_MASTERSERVER_H_
#define SOURCE_NET_MASTER_MASTERSERVER_H_
#include <functional>
#include <enet/enet.h>
#include "../binarystream/BinaryStream.h"
#include "protocol.h"
#include "ServerList.h"
#include "PeerList.h"
#include "../Peer.h"

namespace masterserver {
    typedef std::function<void()> connectedCallback_t;
    typedef std::function<void(serverlist_t &)> serverListReceivedCallback_t;
    typedef std::function<void(peerlist_t &, enet_uint32 address, enet_uint16 port)> peerListReceivedCallback_t;
    typedef std::function<void()> NATPunchReceivedCallback_t; //TODO

    // master server proxy
    // this is not an implementation of the masterserver

    // // stateful object - it can only serve one request type at a time
    // the flow looks like this -
    // 1. set address
    // 2. connect to the master server
    // 2.1. depending on the request type the masterserver sends response immediately
    //      or it expects another message from us
    // 2.2. receive response from the master server
    class MasterServer {
    private:
        ENetPeer *peer = nullptr;
        ENetHost *host;
        std::string address;
        port_t port;
        std::list<connectedCallback_t> connectedCallbacks;
        serverListReceivedCallback_t onServerListReceivedCallback = {};
        peerListReceivedCallback_t onPeerListReceivedCallback = {};

        void writeHeader(binarystream &bs, PACKET_TYPE packetType);

        void send(binarystream &bs);

        binarystream createHeader(PACKET_TYPE packetType);

        packet_update createUpdateRequest(const std::string &description, address_t localAddress, port_t localPort,  address_t publicIPAddress, port_t publicPort, bool needsNAT);

        packet_nat_punch createNatPunchRequest(address_t address, port_t port, address_t localAddress, port_t localPort);

        template<typename Packet>
        binarystream& write(Packet &&p, binarystream &bs) {
            bs << p;
            return bs;
        }


    public:
        MasterServer() = default;

        void setAddressAndPort(const std::string &address, int port);

        void setPeer(ENetPeer *peer);

        void sendHeartBeat();

        void update(ENetHost *host, const std::string &description, address_t localAddress, port_t localPort, address_t publicIPAddress, port_t publicPort, bool needsNAT);

        void requestNatPeers();

        void connectNatToServer(ENetHost *host, address_t address, port_t port, address_t localAddress, port_t localPort);

        void requestServerList(ENetHost *host, serverListReceivedCallback_t callback);

        void onConnected(ENetPeer * peer);

        void onReceived(ENetPeer * peer, ENetPacket * packet);

        void onPeerListReceived(peerListReceivedCallback_t callback);

        static void sendStunBindingRequest(ENetSocket s, enet_uint32 address, enet_uint16 port);

        void sendStunBindingRequest(ENetSocket s);
    private:
        void onConnected(connectedCallback_t callback);

        void disconnect();

        void reconnect(REQUEST_TYPE requestType);

        ENetPeer* connect(ENetHost *host, REQUEST_TYPE requestType);


    };
}

#endif /* SOURCE_NET_MASTER_MASTERSERVER_H_ */
