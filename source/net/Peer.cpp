/*
 * Peer.cpp
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */
#include <iostream>
#include "Peer.h"
#include "event/events.h"
#include "object/objects.h"
#include "ClientGameProxy.h"
#include "ServerGameProxy.h"
namespace Duel6 {
    namespace net {

        template<>
        void Peer::handle(ObjectType objectType, binarystream &s) {
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
//                SpawnBonus sb; //TODO
//                if(!(s>>sb)){
//                    D6_THROW(Exception, "Cannot deserialize EventType::PICK_BONUS");
//                }
//                gameProxy->handle(sb);
                break;
            }
            case EventType::PICK_WEAPON: {
//                SpawnBonus sb; //TODO
//                if(!(s>>sb)){
//                    D6_THROW(Exception, "Cannot deserialize EventType::PICK_WEAPON");
//                }
//                gameProxy->handle(sb);
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
            case EventType::SHOT_ADD:break;
            case EventType::SHOT_HIT:break;
            case EventType::PLAYER_HIT:break;
            case EventType::PLAYER_SPAWN:break;
            case EventType::PLAYER_DIED:break;
            case EventType::PLAYER_KILLED:break;
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
        void Peer::disconnect(bool now) {
            if (state == PeerState::DISCONNECTED) {
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

            } else {
                //callbacks.onDisconnected
            }
            gameProxy->peerDisconnected(*this);
            serverGameProxy->remove(this);
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
                enet_peer_reset(peer.get());
            }
            peer.release();
        }

        Peer::Peer(ClientGameProxy &gameProxy, ServerGameProxy &serverGameProxy, ENetPeer *peer, ENetHost *host, size_t pos)
            : gameProxy(&gameProxy),
              serverGameProxy(&serverGameProxy),
              host(host),
              peer(peer),
              pos(pos) {
            peer->data = new PeerRef { pos, this };
            incomingPeerID = peer->incomingPeerID;
            serverGameProxy.add(this);
            gameProxy.setPeerReference(*this);
            state = PeerState::CONNECTED;
            enet_peer_timeout(peer, 50, 1000, 5000);
            enet_peer_throttle_configure(peer, 5000, 2, 2 ); // default: 5000 2 2
        }

        Peer::Peer(ClientGameProxy &gameProxy, ServerGameProxy &serverGameProxy, ENetPeer *peer, ENetHost *host)
            : Peer(gameProxy,
                serverGameProxy,
                peer,
                host,
                0) {
            enet_peer_timeout(peer, 500, 1000, 5000);
            enet_peer_throttle_configure(peer, 5000, 2, 2 );
            gameProxy.setPeerReference(*this);
        }
    } /* namespace net */
} /* namespace Duel6 */
