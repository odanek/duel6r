/*
 * NetClient.h
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_NETCLIENT_H_
#define SOURCE_NET_NETCLIENT_H_
#include <memory>
#include <string>
#include <enet/enet.h>
#include "Service.h"
#include "Type.h"
#include "Peer.h"
#include "ClientGameProxy.h"
namespace Duel6 {
    class Console;
    namespace net {

        /**
         * The thing that connects to server
         */
        class NetClient: public Service {
        private:
            std::string host;
            port_t port;
            std::unique_ptr<Peer> peer;
            ENetHost *nethost;
            Console &console;
        public:
            NetClient(ClientGameProxy &clientGameProxy, ServerGameProxy &serverGameProxy, Console &console);
            virtual ~NetClient();

            void connect(Game &game, const std::string &host, const Duel6::net::port_t port);
        private:
            void recordPeerNetStats(ENetPeer *peer) override;

            void onStarting() override;

            void onStopping() override;

            void onStopped() override;

            void onPeerConnected(ENetPeer*) override;

            void onPeerDisconnected(ENetPeer*, enet_uint32 reason) override;

            void onConnectionLost() override;
        };

    } /* namespace net */
} /* namespace Duel6 */

#endif /* SOURCE_NET_NETCLIENT_H_ */
