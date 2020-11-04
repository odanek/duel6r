/*
 * Peer.cpp
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */
#include <iostream>
#include <enet/enet.h>
#include "Peer.h"
#include "event/events.h"
#include "object/objects.h"
#include "ClientGameProxy.h"
#include "ServerGameProxy.h"
namespace Duel6 {
    namespace net {

        template<>
        void Peer::handle(ObjectType objectType, binarystream &s) {
            if(peerUpdateState != PeerUpdateState::RUNNING){
                return;
            }
            switch (objectType) {
            case ObjectType::PLAYER: {
                Player p;
                if (s >> p) {
                    gameProxy->handle(p);
                }

                break;
            }
            case ObjectType::PLAYER_INPUTS:
                break;
            case ObjectType::SHOT:
                break;
            case ObjectType::CLIENT:
                break;
            case ObjectType::LEVEL:
                break;
            case ObjectType::WORLD:
                break;
            case ObjectType::BONUS:
                break;
            case ObjectType::WEAPON: {
                Weapon w;
                if (s >> w) {
                    gameProxy->handle(w);
                }
                break;
            }
            case ObjectType::MESSAGE: {
                MessageBroadcast m;
                if(s >> m){
                    gameProxy->handle(m);
                }
                break;
            }
            case ObjectType::MAX_COUNT:
                break;
            }
        }
        template<>
        void Peer::handle(EventType eventType, binarystream &s) {
            switch (eventType) {
            case EventType::REQUEST_GAME_STATE: {
                RequestGameState r;
                if (!(s >> r)) {
                    D6_THROW(Exception, "Cannot deserialize EventType::REQUEST_GAME_STATE");
                }
                /** if server **/
                receivedInputsTick = gameProxy->getTick();
                gameProxy->handle(*this, r);
                /** else not possible **/
                break;
            }
            case EventType::GAME_STATE: {
                GameState gs;
                if (!(s >> gs)) {
                    D6_THROW(Exception, "Cannot deserialize EventType::GAME_STATE");
                }

                receivedInputsTick = gs.tick;
                confirmedInputsTick = gs.tick;
                gameProxy->handle(gs);

                break;
            }
            case EventType::GAME_STATE_UPDATE: {
                GameStateUpdate gs;
                if (!(s >> gs)) {
                    D6_THROW(Exception, "Cannot deserialize EventType::GAME_STATE_UPDATE");
                }
                // these are sent as unreliable unsequenced packets,
                // we must discard those received out of order
                // we must deal with wrap-around of the tick counter (16 bit) - so lets say 30000
                if (((gs.inputTick > receivedInputsTick) && (gs.inputTick - receivedInputsTick < 30000))
                    || receivedInputsTick - gs.inputTick >= 30000) {
                    receivedInputsTick = gs.inputTick;
                    confirmedInputsTick = gs.confirmInputTick;
                    gameProxy->handle(gs);
                } else {
                    gameProxy->lateReceive(gs.inputTick);
                    //out of order / late receive
                }
                break;
            }
            case EventType::PLAYER_INPUTS_UPDATE: {
                PlayerInputsUpdate piu;
                if (!(s >> piu)) {
                    D6_THROW(Exception, "Cannot deserialize EventType::PLAYER_INPUTS_UPDATE");
                }
                if(piu.inputTick > receivedInputsTick || receivedInputsTick - piu.inputTick > 30000)
                {
                    receivedInputsTick = piu.inputTick;
                    confirmedInputsTick = piu.confirmInputTick;
                    gameProxy->handle(piu);
                }
                break;
            }
            case EventType::NEXT_ROUND: {
                NextRound nr;
                if (!(s >> nr)) {
                    D6_THROW(Exception, "Cannot deserialize EventType::NEXT_ROUND");
                }
                gameProxy->handle(nr);
                break;
            }
            case EventType::PLAYERS_DISCONNECTED: {
                PlayersDisconnected pd;
                if(!(s>>pd)){
                    D6_THROW(Exception, "Cannot deserialize EventType::PLAYERS_DISCONNECTED");
                }
                gameProxy->handle(pd);
                break;
            }
            case EventType::PLAYERS_JOINED: {
                PlayersJoined pj;
                if(!(s>>pj)){
                    D6_THROW(Exception, "Cannot deserialize EventType::PLAYERS_JOINED");
                }
                gameProxy->handle(pj);
                break;
            }
            case EventType::RAISE_WATER: {
                RaiseWaterLevel rwl;
                if(!(s>>rwl)){
                    D6_THROW(Exception, "Cannot deserialize EventType::RAISE_WATER");
                }
                gameProxy->handle(rwl);
                break;
            }
            case EventType::SPAWN_BONUS: {
                SpawnBonus sb;
                if(!(s>>sb)){
                    D6_THROW(Exception, "Cannot deserialize EventType::SPAWN_BONUS");
                }
                gameProxy->handle(sb);
                break;
            }
            case EventType::SPAWN_WEAPON: {
                SpawnWeapon sw;
                if(!(s>>sw)){
                    D6_THROW(Exception, "Cannot deserialize EventType::SPAWN_WEAPON");
                }
                gameProxy->handle(sw);
                break;
            }
            case EventType::PICK_BONUS: {
                PickBonus pb;
                if(!(s>>pb)){
                    D6_THROW(Exception, "Cannot deserialize EventType::PICK_BONUS");
                }
                gameProxy->handle(pb);
                break;
            }
            case EventType::PICK_WEAPON: {
                PickWeapon pw;
                if(!(s>>pw)){
                    D6_THROW(Exception, "Cannot deserialize EventType::PICK_WEAPON");
                }
                gameProxy->handle(pw);
                break;
            }
            case EventType::SPAWN_SHOT: {
                SpawnShot ss;
                if(!(s>>ss)){
                    D6_THROW(Exception, "Cannot deserialize EventType::SPAWN_SHOT");
                }
                gameProxy->handle(ss);
                break;
            }
            case EventType::SPAWN_EXPLOSION: {
                SpawnExplosion se;
                if(!(s>>se)){
                    D6_THROW(Exception, "Cannot deserialize EventType::SPAWN_EXPLOSION");
                }
                gameProxy->handle(se);
                break;
            }
            case EventType::PLAY_SAMPLE: {
                PlaySample ps;
                if(!(s>>ps)){
                    D6_THROW(Exception, "Cannot deserialize EventType::PLAY_SAMPLE");
                }
                gameProxy->handle(ps);
                break;
            }
            case EventType::CLIENT_JOINED:break;
            case EventType::CLIENT_DISCONNECTED:break;
            case EventType::CLIENT_DISCONNECT:break;

            case EventType::CLIENT_PLAYERS:break;

            case EventType::GAME_START:break;
            case EventType::GAME_END:break;

            case EventType::ROUND_END:break;
            case EventType::ROUND_START: {
                    StartRound sr;
                    s >> sr;
                    gameProxy->handle(sr);
                    break;
                }
            case EventType::ROUND_SET:break;

            case EventType::SHOT_UPDATE:break;

            case EventType::SHOT_ERASE: {
                EraseShot es;
                if (!(s >> es)) {
                    D6_THROW(Exception, "Cannot deserialize EventType::SHOT_ERASE");
                }
                gameProxy->handle(es);
                break;
            }
            case EventType::SHOT_HIT:break;
            case EventType::PLAYER_SPAWN:break;
            case EventType::PLAYER_DOUBLE_JUMP_EFFECTS:{
                DoubleJumpEffects dje;
                if (!(s >> dje)) {
                    D6_THROW(Exception, "Cannot deserialize EventType::PLAYER_DOUBLE_JUMP_EFFECTS");
                }
                gameProxy->handle(dje);
                break;
            }
            case EventType::CONSOLE:{
                net::Console c;
                if(!(s >> c)){
                    D6_THROW(Exception, "Cannot deserialize EventType::CONSOLE");
                }
                gameProxy->handle(*this, c);
                break;
            }

            case EventType::CHAT:{
                Chat c;
                if(!(s >> c)){
                    D6_THROW(Exception, "Cannot deserialize EventType::CHAT");
                }
                gameProxy->handle(*this, c);
                break;
            }
            case EventType::FOCUS:{
                Focus f;
                if(!(s >> f)){
                    D6_THROW(Exception, "Cannot deserialize EventType::FOCUS");
                }
                gameProxy->handle(*this, f);
                break;
            }

            case EventType::CHAT_MESSAGE:{
                ChatMessage cm;
                if (!(s >> cm)) {
                    D6_THROW(Exception, "Cannot deserialize EventType::CHAT_MESSAGE");
                }
                gameProxy->handle(*this, cm);
                break;
            }
            case EventType::CLIENT_REQUESTS_NEXT_ROUND: {
                RequestNextRound rnr;
                if (!(s >> rnr)) {
                    D6_THROW(Exception, "Cannot deserialize EventType::CLIENT_REQUESTS_NEXT_ROUND");
                }
                gameProxy->handle(rnr);
                break;
            }
            case EventType::MAX_COUNT:
                break;
            default: {
                D6_THROW(Exception, "Cannot deserialize ANYTHING");
                return;
            }
            }
        }

        void Peer::reset() {
            disconnect(true);
            destroy();
        }
        void Peer::disconnect() {
            disconnect(false);
        }
        void Peer::send(const char * data, size_t dataLen, uint8_t channel, bool reliable){
            if(state != PeerState::CONNECTED && state != PeerState::CONNECTING){
                return;
            }
            ENetPacket *packet = enet_packet_create(data, dataLen,
                // ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT crashes the game when network jitter occurs (bug in enet)
                // workaround is to have 254 channels for the communication and switch between them on each message
                reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED |  ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT );
            if (packet == nullptr) {
                D6_THROW(Exception, "Cannot allocate packet");
                return;
            }
            if(enet_peer_send(peer.get(), channel, packet) != 0) {
                std::cerr << __FILE__ << ":" << __LINE__ << ":Failed to send packet over channel " << (int) channel << " to peer. The peer has probably been disconnected and this should not be happening! FIX IT! \n";
            }

        }
        void Peer::disconnect(bool now) {
            if (state == PeerState::DISCONNECTED || state == PeerState::DESTROYED) {
                return;
            }
            state = PeerState::DISCONNECTING;
            if (peer.get() != nullptr) {
                enet_peer_disconnect_later(peer.get(), 0);
            }
            if (now) {
                if (peer.get() != nullptr) {
                    enet_peer_disconnect(peer.get(), 0);
                }
                state = PeerState::DISCONNECTED;
            }
            enet_host_flush(host); //TODO instead of host, pass the *Service into the Peer, then call service.flush();
        }
        bool Peer::onConnected(ENetPeer *me) {
            if (me != peer.get()) {
                return false;
            }
            incomingPeerID = me->incomingPeerID;
            state = PeerState::CONNECTED;

            if(!gameProxy->gameIsServer()){
                requestGameState();
            }
            return true;
        }
        void Peer::requestGameState() {
            peerUpdateState = PeerUpdateState::REQUESTED_GAMESTATE;
            send(gameProxy->getRequestGameState());
        }
        bool Peer::onDisconnected(ENetPeer *me, enet_uint32 reason) {
            if (state == PeerState::DISCONNECTED || state == PeerState::DESTROYED) {
                return true;
            }
            if (me != peer.get()) {
                return false;
            }
            if (state != PeerState::DISCONNECTING) {
                // WTF just happened? probably dropped

                //callbacks.onDropped
                state = PeerState::DISCONNECTING;
            } else {

                //callbacks.onDisconnected
            }
            gameProxy->peerDisconnected(*this);
            destroy();
            return true;
        }

        peer_id_t Peer::getIncomingPeerID() {
            return incomingPeerID;
        }

        peer_id_t Peer::getClientID() {
            return incomingPeerID + 100;
        }

        Uint32 Peer::getRTT() const {
            return peer->lastRoundTripTime;
        }

        void Peer::destroy() {
            if (state == PeerState::DESTROYED) {
                return;
            }
            state = PeerState::DESTROYED;
            if (peer.get() != nullptr) {
                delete (PeerRef*) (peer.get()->data);
                peer.get()->data = nullptr;
                enet_peer_reset(peer.get());
            }
            serverGameProxy->remove(this);
            peer.release();
        }

        Peer::Peer(ClientGameProxy &gameProxy, ServerGameProxy &serverGameProxy, ENetPeer *peer, size_t pos, ENetHost *host,  bool isMasterserver)
            : gameProxy(&gameProxy),
              serverGameProxy(&serverGameProxy),
              host(host),
              peer(peer),
              pos(pos) {
            peer->data = new PeerRef { pos, this, isMasterserver };
            incomingPeerID = peer->incomingPeerID;

            if(isMasterserver){
                enet_peer_timeout(peer, 500, 200, 1000);
            } else {
                serverGameProxy.add(this);

                //TODO Get rid of this bullshit - on server: every time  a peer connects, we overwrite the peer reference
                // it is useful only on client where there is only single peer
                gameProxy.setPeerReference(*this);
                // 15 sec timeout during game
                // the 500 is not 500 ms, it is 500 x current RTT (just in case)
                enet_peer_timeout(peer, 500, 5000, 15000);
                enet_peer_throttle_configure(peer, 5000, 2, 2 );
            }

            state = PeerState::CONNECTED;
        }

        Peer::Peer(ClientGameProxy &gameProxy, ServerGameProxy &serverGameProxy, ENetPeer *peer, ENetHost *host, bool isMasterserver)
            : Peer(gameProxy,
                serverGameProxy,
                peer,
                0,
                host,
                isMasterserver
                ) {
            PeerRef * ref = (PeerRef *)peer->data;
            ref->peer = this;
            if(isMasterserver){
                enet_peer_timeout(peer, 500, 200, 1000);
            } else {
                // 15 sec timeout during game
                //
                // the 500 is not 500 ms, it is 500 x current RTT (just in case)
                // this is probably all messed up config and only the 15sec rule applies
                enet_peer_timeout(peer, 500, 5000, 15000);
                enet_peer_throttle_configure(peer, 5000, 2, 2 );
                gameProxy.setPeerReference(*this);
            }
        }

        void Peer::setDescription(const std::string &s) {
            description = s;
        }

        const std::string& Peer::getDescription() {
            return description;
        }

    } /* namespace net */
} /* namespace Duel6 */
