/*
 * NetHost.cpp
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */

#include "NetHost.h"
#include "ClientGameProxy.h"
namespace Duel6 {
    namespace net {
#define MAX_PEERS 15
#define CHANNELS 4
//
//        void NetHost::onPeerHandle(ENetPeer* peer, ObjectType, binarystream & bs) {
//
//        }
//        void NetHost::onPeerHandle(ENetPeer* peer, EventType, binarystream & bs){
//
//        }
//        void NetHost::setGameProxyReference(ClientGameProxy & clientGameProxy /*, GameProxy & serverGameProxy*/) {
//            this->clientGameProxy = &clientGameProxy;
//           // this->serverGameProxy = &serverGameProxy;
//        }
        NetHost::NetHost(ClientGameProxy &clientGameProxy, ServerGameProxy &serverGameProxy)
            : Service(clientGameProxy, serverGameProxy)
        {
            // TODO Auto-generated constructor stub

        }

        NetHost::~NetHost()
        {
            die();
            // TODO Auto-generated destructor stub
        }

        void NetHost::listen(Game &game) {
            if (state != ServiceState::UNINITIALIZED) {
                D6_THROW(Exception, "starting server that is not uninitialized");
            }
            setGameReference(game);
            ENetAddress address;
            address.host = ENET_HOST_ANY;
            address.port = 2020;
            ENetHost *nethost = enet_host_create(&address, MAX_PEERS, CHANNELS, 0, 0);
            if (nethost == nullptr) {
                D6_THROW(Exception, "cannot start server");
            }
            start(nethost);
            started();
        }

        void NetHost::die() {
            stop();

        }

        void NetHost::onStarting() {

        }

        void NetHost::onStopping() {
            for (auto &peer : peers) {
                peer->disconnect();
            }
            for (const auto &p : peers) {
                if (p->getState() != PeerState::DISCONNECTED) {
                    return;
                }
            }
            stopped();
        }

        void NetHost::onStopped() {
            peers.clear();
        }

        void NetHost::onPeerConnected(ENetPeer *peer) {
            if (state != ServiceState::STARTED) {
                enet_peer_disconnect_now(peer, 42);
            }

            size_t i = 0;
            for (std::unique_ptr<Peer> &p : peers) {
                if (p.get() == nullptr) {

//                    //TODO MEM LEAK HERE
//                    ClientGameProxy * proxy = new ClientGameProxy();
//                    proxy->setGameReference(*game);
//                    //
                    p = std::make_unique<Peer>(*clientGameProxy, *serverGameProxy, peer, i);

                    return;
                }
                i++;
            }
//            //TODO MEM LEAK HERE
//            ClientGameProxy * proxy = new ClientGameProxy();
//            proxy->setGameReference(*game);
//            //
            peers.emplace_back(std::make_unique<Peer>(*clientGameProxy, *serverGameProxy, peer, i));
        }

        void NetHost::onPeerDisconnected(ENetPeer *peer, enet_uint32 reason) {
            PeerRef *peerRef = (PeerRef*) (peer->data);

            if (peerRef->pos > peers.size() - 1) {
                delete peerRef;
                D6_THROW(Exception, "Server Disconnected peer id out of bounds");
            }

            Peer *p = peers[peerRef->pos].get();
            if (p != peerRef->peer) {

            }
            if (!p->onDisconnected(peer, reason)) {
                return;
            }
            if (state != ServiceState::STOPPING) {
                return;
            }
            for (const auto &p : peers) {
                if (p->getState() != PeerState::DISCONNECTED) {
                    return;
                }
            }
            stopped();
        }
    } /* namespace net */
} /* namespace Duel6 */
