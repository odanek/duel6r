/*
 * Service.h
 *
 *  Created on: Mar 17, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_SERVICE_H_
#define SOURCE_NET_SERVICE_H_
#include <memory>
#include <enet/enet.h>
#include "../Exception.h"

#include "Type.h"
#include "binarystream/BinaryStream.h"
#include "Event.h"
#include "Object.h"

namespace Duel6 {
    class Game;

    namespace net {
        class ClientGameProxy;
        class ServerGameProxy;
        enum class ServiceState {
            UNINITIALIZED,
            INITIALIZED,
            STARTING,
            STARTED,
            STOPPING,
            STOPPED
        };
        class Service {
        protected:
            ServiceState state = ServiceState::UNINITIALIZED;
            std::unique_ptr<ENetHost> serviceHost;
            Game *game = nullptr;
            public:
            ClientGameProxy *clientGameProxy;
            ServerGameProxy *serverGameProxy;
            Service(ClientGameProxy &c, ServerGameProxy &s)
                :
                  clientGameProxy(&c),
                  serverGameProxy(&s)
            {
            }

            virtual ~Service() {
            }

            void stopNow();

            void poll(Uint32 timeout) {
                if (state == ServiceState::UNINITIALIZED || state == ServiceState::STOPPED) {
                    return;
                }
                ENetEvent event;
                ENetHost *netHost = serviceHost.get();
                while (enet_host_service(netHost, &event, timeout) > 0) {
                    switch (event.type) {
                    case ENetEventType::ENET_EVENT_TYPE_NONE:
                        break;
                    case ENetEventType::ENET_EVENT_TYPE_CONNECT:
                        onPeerConnected(event.peer);
                        break;
                    case ENetEventType::ENET_EVENT_TYPE_DISCONNECT:
                        onPeerDisconnected(event.peer, event.data);
                        break;
                    case ENetEventType::ENET_EVENT_TYPE_RECEIVE:
                        onPeerReceived(event.peer, event.packet, event.channelID);
                        enet_packet_destroy(event.packet);
                        break;
                    }
                    if (state == ServiceState::UNINITIALIZED) {
                        break;
                    }
                }
            }

            bool isConnected() {
                return state == ServiceState::STARTED
                    || state == ServiceState::STOPPING;
            }

        protected:
            void setGameReference(Game &game) {
                this->game = &game;
            }
            void start(ENetHost *nethost) {
                serviceHost.reset(nethost);
                state = ServiceState::INITIALIZED;

                state = ServiceState::STARTING;
                onStarting();
            }

            void started() {
                state = ServiceState::STARTED;
            }

            void stop() {
                state = ServiceState::STOPPING;
                onStopping();
            }

            void stopped() {
                state = ServiceState::STOPPED;
                onStopped();
                tearDown();
                state = ServiceState::UNINITIALIZED;
            }

            void onPeerReceived(ENetPeer *peer, ENetPacket *packet, enet_uint8 channel);

        private:

            virtual void onStarting() = 0;
            virtual void onStopping() = 0;
            virtual void onStopped() = 0;

            virtual void onPeerConnected(ENetPeer*) = 0;
            virtual void onPeerDisconnected(ENetPeer*, enet_uint32 reason) = 0;

//            virtual void onPeerHandle(ENetPeer *peer, ObjectType, binarystream &bs) = 0;
//            virtual void onPeerHandle(ENetPeer *peer, EventType, binarystream &bs) = 0;

            void tearDown() {
                if (serviceHost.get() != nullptr) {
                    enet_host_destroy(serviceHost.get());
                }
                serviceHost.release();
            }
        };
    }
}
#endif /* SOURCE_NET_SERVICE_H_ */
