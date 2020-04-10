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
#include "Service.h"
#include "Peer.h"
//#include "GameProxy.h"
namespace Duel6 {
    class Game;
    namespace net {
        class ClientGameProxy;
        class ServerGameProxy;

        class NetHost: public Service {
        private:

            std::vector<std::unique_ptr<Peer>> peers;

        public:
            NetHost(ClientGameProxy & clientGameProxy, ServerGameProxy & serverGameProxy);
          //  void setGameProxyReference(ClientGameProxy & clientGameProxy, ServerGameProxy & serverGameProxy);

            void listen(Game &game, const std::string &host,
                        const Duel6::net::port_t port);
            void die();

            virtual ~NetHost();

        private:
            void onStarting() override;
            void onStopping() override;
            void onStopped() override;

            void onPeerConnected(ENetPeer*) override;
            void onPeerDisconnected(ENetPeer*, enet_uint32 reason) override;

//            void onPeerHandle(ENetPeer *peer, ObjectType, binarystream &bs) override;
//            void onPeerHandle(ENetPeer *peer, EventType, binarystream &bs) override;

        };

    } /* namespace net */
} /* namespace Duel6 */

#endif /* SOURCE_NET_NETHOST_H_ */
