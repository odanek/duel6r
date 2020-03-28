/*
 * Peer.cpp
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */

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
            case ObjectType::PLAYER:{
                Player p;
                if(s >> p){
                    gameProxy->handle(p);
                }

                break;}
            case ObjectType::SHOT:
                break;
            case ObjectType::CLIENT:
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
                s >> r;
                /** if server **/
                gameProxy->handle(*this, r);
                /** else not possible **/
                break;
            }
            case EventType::GAME_STATE: {
                GameState gs;
                s >> gs;
                gameProxy->handle(gs);
                break;
            }

            case EventType::NEXT_ROUND:{
                NextRound nr;
                s >> nr;
                gameProxy->handle(nr);
                break;
            }
            case EventType::CLIENT_JOINED:break;
            case EventType::CLIENT_DISCONNECTED:break;
            case EventType::CLIENT_DISCONNECT:break;

            case EventType::CLIENT_PLAYERS:break;

            case EventType::GAME_START:break;
            case EventType::GAME_END:break;

            case EventType::ROUND_END:break;
            case EventType::ROUND_START:break;
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
            enet_peer_disconnect_later(peer.get(), 0);
            if(now){
                enet_peer_disconnect(peer.get(), 0);
                state = PeerState::DISCONNECTED;
            }
        }
        bool Peer::onConnected(ENetPeer *me) {
            if (me != peer.get()) {
                return false;
            }
            state = PeerState::CONNECTED;

            return true;
        }
        void Peer::requestGameState() {

            send(gameProxy->getRequestGameState());
        }
        bool Peer::onDisconnected(ENetPeer *me, enet_uint32 reason) {
            if (state == PeerState::DISCONNECTED) {
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

            destroy();
            return true;
        }

        void Peer::destroy() {
            state = PeerState::DISCONNECTED;
            if (peer.get() != nullptr) {
                delete (PeerRef*) (peer.get()->data);
                enet_peer_reset(peer.get());
            }
            peer.release();
            serverGameProxy->remove(this);
        }
        Peer::Peer(ClientGameProxy & gameProxy, ServerGameProxy & serverGameProxy, ENetPeer *peer, size_t pos)
            : gameProxy(&gameProxy),
              serverGameProxy(&serverGameProxy),
              peer(peer),
              pos(pos) {
            peer->data = new PeerRef{pos, this};
            serverGameProxy.add(this);
            state = PeerState::CONNECTED;
        }
        Peer::Peer(ClientGameProxy & gameProxy, ServerGameProxy & serverGameProxy, ENetPeer *peer)
            : gameProxy(&gameProxy),
              serverGameProxy(&serverGameProxy),
              peer(peer) {
            peer->data = new PeerRef{pos, this};
            serverGameProxy.add(this);
            state = PeerState::CONNECTING;
        }
    } /* namespace net */
} /* namespace Duel6 */
