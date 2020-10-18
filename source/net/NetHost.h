/*
 * NetHost.h
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_NETHOST_H_
#define SOURCE_NET_NETHOST_H_

#include <memory>
#include <string>
#include <vector>
#include <enet/enet.h>
#include "master/ServerList.h"
#include "Service.h"
#include "Peer.h"
//#include "GameProxy.h"
namespace Duel6 {
    class Game;
    class Console;

    namespace net {
        class ClientGameProxy;
        class ServerGameProxy;

        class NetHost: public Service {
        private:
            Console &console;
            std::vector<std::unique_ptr<Peer>> peers;
            Float64 heartbeatCountDown;
            std::string description = "foobar server";
            enet_uint32 publicIPAddress;
            enet_uint16 publicPort;
            bool enableMasterDiscovery = false;
            bool enableNAT = false;
        public:
            NetHost(ClientGameProxy & clientGameProxy, ServerGameProxy & serverGameProxy, Duel6::Console &console);
            void setServerConfig(enet_uint32 publicIPAddress, enet_uint16 publicPort, const std::string & serverDescription, bool enableMasterDiscovery, bool enableNAT);
            virtual ~NetHost();

            void listen(Game &game, const std::string &host, const Duel6::net::port_t port);
            void natPunch(enet_uint32 address, enet_uint16 port);
        private:
            void onStarting() override;
            void onStopping() override;
            void onStopped() override;

            void onPeerConnected(ENetPeer*) override;
            void onPeerDisconnected(ENetPeer*, enet_uint32 reason) override;

            void registerOnMasterServer(); // connect, expect to be disconnected
            void heartbeatMasterServer(); // just connect to master server to indicate liveliness
            void sendUpdateToMasterServer(std::string description); // connect, send update of game name etc.

            void requestNATPeers(); // connect, expect response from master server, then try to connect all returned peers
            void onNATPeersListReceived(masterserver::peerlist_t &, enet_uint32 publicAddress, enet_uint16 publicPort); // call natPunch

            void runPeriodicalTasks(Float64 elapsedTime) override;

            void sendStunBindingResponse(enet_uint32 address, enet_uint16 port, enet_uint32 publicAddress, enet_uint16 publicPort);
        };
    } /* namespace net */
} /* namespace Duel6 */

#endif /* SOURCE_NET_NETHOST_H_ */
