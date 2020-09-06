/*
 * NetHost.cpp
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */

#include <string>
#include "NetHost.h"
#include "ClientGameProxy.h"
#include "../Game.h"
#include "../console/Console.h"
#include "master/MasterServer.h"
#include "master/protocol.h"

namespace Duel6 {
    namespace net {
#define MAX_PEERS 15
#define CHANNELS 254

        NetHost::NetHost(ClientGameProxy &clientGameProxy, ServerGameProxy &serverGameProxy, Console &console)
            : Service(clientGameProxy, serverGameProxy), console(console) {
        }

        NetHost::~NetHost() {
            stop();
        }

        void NetHost::listen(Game &game, const std::string &host,
                             const Duel6::net::port_t port) {
            if (state != ServiceState::UNINITIALIZED) {
                D6_THROW(Exception, "starting server that is not uninitialized");
            }
            setGameReference(game);
            ENetAddress address;
            address.host = ENET_HOST_ANY;
            if (host != "any") {
                enet_address_set_host(&address, host.c_str());
            }
            address.port = port;
            ENetHost *nethost = enet_host_create(&address, MAX_PEERS, CHANNELS, 0, 0);
            if (nethost == nullptr) {
                D6_THROW(Exception, "cannot start server (port taken?)");
            }

            console.printLine("NetHost::listen start");
            start(nethost);
            console.printLine("NetHost::listen started ");
            started();
        }

        void NetHost::registerOnMasterServer(){
            masterserver::MasterServer & proxy=  masterServerProxy;

            std::string adresa = "127.0.0.1";
            int port = 5902;
            std::string message = "NetHost::registerOnMasterServer Registering on master server ";
            message += adresa;
            message += ":";
            message += port;
            console.printLine(message);
            proxy.setAddressAndPort(adresa, port);
            proxy.registerOnMasterServer(serviceHost.get());
        }

        void NetHost::natPunch(enet_uint32 address, enet_uint16 port){
            console.printLine(Format("NAT punch! {0}") << hostToIPaddress(address, port));
            // DA DA DA
            ENetAddress peerAddress;
            peerAddress.host = address;
            peerAddress.port = port;
            ENetPeer * peer = enet_host_connect(serviceHost.get(), &peerAddress, CHANNELS, 42);

        }

        void NetHost::onNATPeersListReceived(masterserver::peerlist_t & peerList){
            for(auto & peer: peerList){
                natPunch(peer.address, peer.port);
            }
        }

        void NetHost::onStarting() {

            // if publishToMasterServer == true {
            //   masterServer.announce(...)
            //}
            console.printLine("NetHost::onStarting ");
            registerOnMasterServer();
        }

        void NetHost::onStopping() {
            console.printLine("NetHost::onStopping ");
            for (auto &peer : peers) {
                peer->disconnect();
                flush(); // send out the disconnects, don't wait for any replies
                peer->reset();
            }
            flush(); // send out the disconnects, don't wait for any replies
            stopped();
        }

        void NetHost::onStopped() {
            state = ServiceState::STOPPED;
            console.printLine("NetHost::onStopped()");
            peers.clear();
        }

        void NetHost::onPeerConnected(ENetPeer *peer) {
            console.printLine("NetHost::onPeerConnected");
            if (state != ServiceState::STARTED) {
                enet_peer_disconnect_now(peer, 42);
            }

            size_t i = 0;
            for (std::unique_ptr<Peer> &p : peers) {
                if (p.get() == nullptr) {
                    p = std::make_unique<Peer>(*clientGameProxy, *serverGameProxy, peer, i, serviceHost.get());
                    p->onConnected(peer);
                    return;
                }
                i++;
            }
            auto &p = peers.emplace_back(std::make_unique<Peer>(*clientGameProxy, *serverGameProxy, peer, i, serviceHost.get()));
            p->onConnected(peer);
        }

        void NetHost::onPeerDisconnected(ENetPeer *peer, enet_uint32 reason) {
            console.printLine("NetHost::onPeerDisconnected");
            PeerRef *peerRef = (PeerRef*) (peer->data);

            if (peerRef->pos + 1 > peers.size()) {
                delete peerRef;
                D6_THROW(Exception, "Server Disconnected peer id out of bounds");
            }

            Peer *p = peers[peerRef->pos].get();
            if (p != peerRef->peer) {

            }
            if (!p->onDisconnected(peer, reason)) {
                return;
            }
            if (state == ServiceState::STOPPING) {
                // this server is stopping
                stopped();
            }
        }
    } /* namespace net */
} /* namespace Duel6 */
