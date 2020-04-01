/*
 * NetClient.cpp
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */
#include "Peer.h"
#include "NetClient.h"

namespace Duel6 {
    namespace net {
//        void NetClient::onPeerHandle(ENetPeer *peer, ObjectType, binarystream &bs) {
//
//        }
//        void NetClient::onPeerHandle(ENetPeer *peer, EventType, binarystream &bs) {
//
//        }
        NetClient::NetClient(ClientGameProxy & clientGameProxy, ServerGameProxy & serverGameProxy):
            Service(clientGameProxy, serverGameProxy)
        {
            //  client.
            // TODO Auto-generated constructor stub

        }

        NetClient::~NetClient()
        {
            disconnect();
            // TODO Auto-generated destructor stub
        }

        void NetClient::connect(const std::string &host,
                                const Duel6::net::port_t port) {
            if (state != ServiceState::UNINITIALIZED) {
                D6_THROW(Exception, "connecting client that is not uninitialized");
            }
            this->host = host;
            this->port = port;
            ENetHost *nethost = enet_host_create(nullptr,
                1 /* only allow 1 outgoing connection */,
                2 /* allow up 2 channels to be used, 0 and 1 */,
                0 /* assume any amount of incoming bandwidth */,
                0 /* assume any amount of outgoing bandwidth */);
            if (nethost == nullptr) {
                D6_THROW(Exception, "Failed to start net client  \(^^)/");
            }

            start(nethost);
        }

        void NetClient::onStarting() {
            ENetAddress address;
            enet_address_set_host(&address, host.c_str());
            address.port = port;
            peer = std::make_unique<Peer>(*clientGameProxy, *serverGameProxy, enet_host_connect(serviceHost.get(), &address, 2, 42));
        }

        void NetClient::onStopping() {
            if(peer.get() == nullptr) {
                return;
            }
            peer->disconnect();
        }

        void NetClient::onStopped() {
            peer.reset();
        }
        void NetClient::disconnect() {
            stop();
        }

        void NetClient::onPeerConnected(ENetPeer *me) {
            if (!peer->onConnected(me)) {
                return;
            }
            started();
            peer->requestGameState();
        }

        void NetClient::onPeerDisconnected(ENetPeer *me, enet_uint32 reason) {
            if (!peer->onDisconnected(me, reason)) {
                return;
            }

            stopped();
        }

    } /* namespace net */
} /* namespace Duel6 */
