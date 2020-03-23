/*
 * ServerGameProxy.cpp
 *
 *  Created on: Mar 21, 2020
 *      Author: fanick
 */

#include "ServerGameProxy.h"
#include "../Game.h"
#include "event/events.h"
namespace Duel6 {
    namespace net {
        void ServerGameProxy::add(Peer *p) {
            peers.push_back(p);
        }
        void ServerGameProxy::remove(Peer *p) {
            peers.remove(p);
        }
        ServerGameProxy::ServerGameProxy()
        {
            // TODO Auto-generated constructor stub

        }

        ServerGameProxy::~ServerGameProxy()
        {
            // TODO Auto-generated destructor stub
        }
//        void ServerGameProxy::gameState(GameState & gameState){
//
//        }

        void ServerGameProxy::handle(ObjectBase &o) {
            ;
        }
        void ServerGameProxy::handle(EventBase &e) {
            ;
        }
        void ServerGameProxy::nextRound() {
            for (auto &peer : peers) {
                peer->send(NextRound());
            }

        }
        void ServerGameProxy::sendGameState(Game &game) {
            GameProxy::sendGameState(peers, game);
        }
        void ServerGameProxy::gameState(Game &game) {
            GameProxy::sendGameState(peers, game);
        }
    } /* namespace net */
} /* namespace Duel6 */
