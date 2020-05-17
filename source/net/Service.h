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
        private:
            bool stopRequested = false;
        public:
            ClientGameProxy *clientGameProxy;
            ServerGameProxy *serverGameProxy;

            Service(ClientGameProxy &c, ServerGameProxy &s)
                : clientGameProxy(&c),
                  serverGameProxy(&s) {
            }

            virtual ~Service() {
            }

            //stop on next poll
            void requestStop();

            void poll(Uint32 timeout);

            bool isConnected() {
                return state == ServiceState::STARTED
                    || state == ServiceState::STOPPING;
            }

        protected:
            void setGameReference(Game &game) {
                this->game = &game;
            }

            void start(ENetHost *nethost) {
                stopRequested = false;
                serviceHost.reset(nethost);
                state = ServiceState::INITIALIZED;

                state = ServiceState::STARTING;
                onStarting();
            }

            void started() {
                if(state != ServiceState::STARTING){
                    D6_THROW(Exception, "called Service::started() while not starting");
                }
                state = ServiceState::STARTED;
            }

            void stop() {
                if(state != ServiceState::STARTED || state == ServiceState::STOPPED){
                    return;
                }
                state = ServiceState::STOPPING;
                onStopping();
            }

            void stopped() {
                if(state != ServiceState::STOPPING){
                    onConnectionLost();
                }
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

            virtual void onConnectionLost(){}

            virtual void recordPeerNetStats(ENetPeer *peer){};
            virtual void onPeerConnected(ENetPeer*) = 0;

            virtual void onPeerDisconnected(ENetPeer*, enet_uint32 reason) = 0;

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
