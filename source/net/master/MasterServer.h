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
    typedef std::function<void(serverlist_t &)> serverListReceivedCallback_t;
    typedef std::function<void(peerlist_t &)> peerListReceivedCallback_t;
    typedef std::function<void()> NATPunchReceivedCallback_t;

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
        ENetPeer *peer;
        ENetHost *host;
        std::string address;
        port_t port;
        std::function<void(void)> onConnectCallback = [](){};
        serverListReceivedCallback_t onServerListReceivedCallback = [](serverlist_t &){};
        peerListReceivedCallback_t onPeerListReceivedCallback = [](peerlist_t &){};

        void writeHeader(binarystream &bs, PACKET_TYPE packetType) {
            packetHeader header;
            header.type = packetType;
            bs << header;
        }

        void send(binarystream &bs) {
            std::string dataStr = bs.str();
            ENetPacket *enetPacket = enet_packet_create(dataStr.c_str(), dataStr.length(), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, enetPacket);
        }

        binarystream createHeader(PACKET_TYPE packetType) {
            binarystream bs;
            writeHeader(bs, packetType);
            return bs;
        }
        packet_update createUpdateRequest(std::string &description) {
            packet_update p;
            p.descr = description;
            return p;
        }

        packet_nat_punch createNatPunchRequest(address_t address, port_t port) {
            packet_nat_punch p;
            p.address = address;
            p.port = port;
            return p;
        }

        template<typename Packet>
        binarystream& write(Packet &&p, binarystream &bs) {
            bs << p;
            return bs;
        }

    public:
        MasterServer() = default;

        void setAddressAndPort(const std::string &address, int port) {
            this->address = address;
            this->port = port;
        }

        void setPeer(ENetPeer *peer) {
            this->peer = peer;
        }

        void sendHeartBeat(){
            reconnect(REQUEST_TYPE::SERVER_REGISTER);
            onConnectCallback  = [this](){
                this->disconnect();
            };
        }

        void update(std::string &description) {
            reconnect(REQUEST_TYPE::SERVER_UPDATE);
            onConnectCallback = [this, & description](){
                binarystream bs = createHeader(PACKET_TYPE::SERVER_UPDATE);
                bs << createUpdateRequest(description);
                send(bs);
            };
        }

        void registerOnMasterServer(ENetHost *host) {
            connect(host, REQUEST_TYPE::SERVER_REGISTER);
            onConnectCallback = [this](){
                this->disconnect();
            };
        }

        void requestNatPeers(peerListReceivedCallback_t callback){
            reconnect(REQUEST_TYPE::SERVER_NAT_GET_PEERS);
            onConnectCallback = [](){

            };

            onPeerListReceivedCallback = [this, & callback](peerlist_t & peerlist){
                callback(peerlist);
                this->disconnect();
            };
        }

        void connectNatToServer(ENetHost *host, address_t address, port_t port){
            connect(host, REQUEST_TYPE::CLIENT_NAT_CONNECT_TO_SERVER);
            onConnectCallback = [this, address, port]() {
                    binarystream bs = createHeader(PACKET_TYPE::CLIENT_NAT_PUNCH);
                    bs << createNatPunchRequest(address, port);
                    send(bs);
                    this->disconnect();
            };
        }
        void requestServerList(ENetHost *host, serverListReceivedCallback_t callback);

        void onConnected(ENetPeer * peer){
            onConnectCallback();
        }

        void onReceived(ENetPeer * peer, ENetPacket * packet){
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
                        serverList.emplace_back(ServerEntry{server.address, server.port, server.descr});
                    }
                    onServerListReceivedCallback(serverList);
                    break;
                }

                case PACKET_TYPE::SERVER_NAT_PEERS: {
                    printf("NAT peers received!\n");
                    packet_nat_peers p;
                    bs >> p;
                    peerlist_t peerList;
                    peerList.reserve(p.peers.size());
                    for (auto &peer : p.peers) {
                        printf(" addr: %s\n", hostToIPaddress(peer.address, peer.port).c_str());
                        peerList.emplace_back(PeerEntry{peer.address, peer.port});
                    }

                    onPeerListReceivedCallback(peerList);
                    break;
                }
                case PACKET_TYPE::CLIENT_NAT_PUNCH:
                case PACKET_TYPE::PACKETS_COUNT: {
                    break;
                }
            }

        }
    private:
        void disconnect(){
            enet_peer_disconnect_later(this->peer, 0);
        }

        void reconnect(REQUEST_TYPE requestType){
            ENetAddress masterAddress;
            enet_address_set_host(&masterAddress, address.c_str());
            masterAddress.port = port;
            ENetPeer *peer = enet_host_connect(host, &masterAddress, 1, static_cast<enet_uint32>(requestType));
            Duel6::net::PeerRef *ref = new Duel6::net::PeerRef();
            peer->data = ref;
            ref->isMasterServer = true;
            ref->peer = nullptr;
            ref->pos = 0;
            this->peer = peer;
        }

        ENetPeer* connect(ENetHost *host, REQUEST_TYPE requestType) {
            this->host = host;
            ENetAddress masterAddress;
            enet_address_set_host(&masterAddress, address.c_str());
            masterAddress.port = port;
            ENetPeer *peer = enet_host_connect(host, &masterAddress, 1, static_cast<enet_uint32>(requestType));
            Duel6::net::PeerRef *ref = new Duel6::net::PeerRef();
            peer->data = ref;
            ref->isMasterServer = true;
            ref->peer = nullptr;
            ref->pos = 0;
            this->peer = peer;
            return peer;
        }
    };
}

#endif /* SOURCE_NET_MASTER_MASTERSERVER_H_ */
