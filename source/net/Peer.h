/*
 * Peer.h
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_PEER_H_
#define SOURCE_NET_PEER_H_
#include <memory>
#include <cstring>
#include <enet/enet.h>
#include "../Exception.h"
#include "binarystream/BinaryStream.h"
#include "Object.h"
#include "Event.h"
#include "object/Player.h"

#define SNAPSHOTS 64
namespace Duel6 {
    namespace net {
        class ClientGameProxy;
        class ServerGameProxy;
        class Peer;

        // reliable channels
        // - messages sent in different channels don't block other
        // messages in other channels from being delivered
        enum Channel {
            SHOTS = 0,
            SAMPLES,
            BONUSES,
            WEAPONS,
            PLAYERS,
            BROADCAST_MESSAGES,
            DEFAULT,
            UNRELIABLE_CHANNEL_BASELINE
        };
        enum class PeerUpdateState {
            WAITING_FOR_REQUEST,
            REQUESTED_GAMESTATE,
            GAMESTATE_RECEIVED,
            RUNNING
        };
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
            bool isMasterServer = false; // this peer represents connection to the master server (so it needs to be handled differently)
        };
        class Peer {
        private:
            uint8_t UNRELIABLE_CHANNEL  = Channel::UNRELIABLE_CHANNEL_BASELINE;

            PeerState state = PeerState::DISCONNECTED;
            ClientGameProxy *gameProxy = nullptr;
            ServerGameProxy *serverGameProxy = nullptr; //parent
            ENetHost *host = nullptr;
            std::unique_ptr<ENetPeer> peer;
            size_t pos = 0;
            peer_id_t incomingPeerID = 0;
            std::string description;
            // SomeAppObject callbacks
        public:
            const ENetPeer& getEnetPeer() const {
                return *peer;
            }
            PeerUpdateState peerUpdateState = PeerUpdateState::WAITING_FOR_REQUEST;
            Uint32 getRTT() const;
            size_t choke = 64; // 0..64
            std::array<std::map<Uint32, Player>, SNAPSHOTS> snapshot;
            tick_t confirmedInputsTick = 0; //TODO private
            tick_t receivedInputsTick = 0;
            Peer(ClientGameProxy &gameProxy, ServerGameProxy &serverGameProxy, ENetPeer *peer, size_t pos, ENetHost *host, bool isMasterserver = false);
            Peer(ClientGameProxy &gameProxy, ServerGameProxy &serverGameProxy, ENetPeer *peer, ENetHost *host, bool isMasterserver = false);
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
            void sendReliable(MessageObject &msg, uint8_t channel = Channel::DEFAULT) {
                if(peerUpdateState != PeerUpdateState::RUNNING){
                    // we need to linearize reliable messages for the peers not to receive out of order message e.g. spawn shot before
                    // they have the game state
                    channel = Channel::DEFAULT;
                }
                send(msg, channel, true);
            }

            template<typename MessageObject>
            void sendUnreliable(MessageObject &msg) {
                send(msg, UNRELIABLE_CHANNEL++, false);
                // we are cycling the unreliable channels because of bug in ENet < 1.3.15-1 (fixed in 1.3.16)
                if(UNRELIABLE_CHANNEL >= 254){
                    UNRELIABLE_CHANNEL = UNRELIABLE_CHANNEL_BASELINE;
                }
            }

            template<typename MessageObject>
            void send(MessageObject &msg, uint8_t channel = Channel::DEFAULT, bool reliable = true) {
                binarystream bs;
                if (!msg.send(bs)){
                    std::cerr << " FAILED TO SEND \n";
                    return;
                }
                bs.seekp(0, std::ios::end);
                size_t len = bs.tellp();
                send(bs.str().c_str(), len, channel, reliable);
            }


            template<typename MessageObject>
            void send(MessageObject &&msg) {
                sendReliable(msg);
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

            peer_id_t getClientID();

            void setDescription(const std::string & s);

            const std::string & getDescription() ;

        protected:
            void send(const char * data, size_t dataLen, uint8_t channel, bool reliable);

            void disconnect(bool now);

            void destroy();
        };

        template<>
        void Peer::handle(EventType eventType, binarystream &s);

        template<>
        void Peer::handle(ObjectType objectType, binarystream &s);

    } /* namespace net */
} /* namespace Duel6 */

#endif /* SOURCE_NET_PEER_H_ */
