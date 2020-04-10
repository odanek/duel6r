/*
 * Peer.h
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_PEER_H_
#define SOURCE_NET_PEER_H_
#include <memory>
#include <enet/enet.h>
#include "binarystream/BinaryStream.h"
#include "Object.h"
#include "Event.h"
//#include "GameProxy.h"
#include "Service.h"
#include "object/Player.h"
#define CHANNEL 0
#define UNRELIABLE_CHANNEL 1
namespace Duel6 {
    namespace net {
        class ClientGameProxy;
        class ServerGameProxy;
        class Peer;
        enum class PeerState {
            CONNECTING,
            CONNECTED,
            DISCONNECTING,
            DISCONNECTED,
            DESTROYED
        };
        struct PeerRef { // gets recorded in ENets' peer->data
            size_t pos;
            Peer *peer;
        };
        class Peer {
        private:
            PeerState state = PeerState::DISCONNECTED;
            ClientGameProxy *gameProxy = nullptr;
            ServerGameProxy *serverGameProxy = nullptr; //parent
            ENetHost *host = nullptr;
            // Service *service;
            std::unique_ptr<ENetPeer> peer;
            size_t pos = 0;
            peer_id_t incomingPeerID = 0;

            // SomeAppObject callbacks
        public:
            Uint32 getRTT() const;
            std::array<std::map<Uint32, Player>, 32> snapshot;
            uint16_t confirmedInputsTick = 0; //TODO private
            uint16_t receivedInputsTick = 0;
            Peer(ClientGameProxy &gameProxy, ServerGameProxy &serverGameProxy, ENetPeer *peer, ENetHost *host, size_t pos);
            Peer(ClientGameProxy &gameProxy, ServerGameProxy &serverGameProxy, ENetPeer *peer, ENetHost *host);
            Peer() = default;
            Peer(const Peer &peer) = delete;
            Peer(Peer &&peer) = delete;
            Peer& operator =(Peer &&peer) = default;
            Peer& operator =(const Peer &peer) = default;

            void reset();

            void disconnect();

            template<typename MessageType, typename Stream>
            void handle(MessageType type, Stream &s) {

            }
            template<typename MessageObject>
            void sendReliable(MessageObject &msg, uint8_t channel = CHANNEL, bool reliable = true) {
                send(msg, CHANNEL, true);
            }
            template<typename MessageObject>
            void sendUnreliable(MessageObject &msg) {
                send(msg, UNRELIABLE_CHANNEL, false);
                enet_host_flush(host);
            }
            template<typename MessageObject>
            void send(MessageObject &msg, uint8_t channel = CHANNEL, bool reliable = true) {
                binarystream bs;
                msg.send(bs);
                std::string dataStr = bs.str();
                const char *data = dataStr.c_str();
                size_t dataLen = dataStr.length();
                ENetPacket *packet = enet_packet_create(data, dataLen, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
                enet_peer_send(peer.get(), channel, packet);
            }
            template<typename MessageObject>
            void send(MessageObject &&msg) {
                send(msg);
            }
            bool onConnected(ENetPeer *me);

            bool onDisconnected(ENetPeer *me, enet_uint32 reason);

            virtual ~Peer() {
                destroy();
            }

            PeerState getState() {
                return state;
            }

            void requestGameState();

            peer_id_t getIncomingPeerID();

        protected:
            void disconnect(bool now);

            void destroy();
        };
    } /* namespace net */
} /* namespace Duel6 */

#endif /* SOURCE_NET_PEER_H_ */
