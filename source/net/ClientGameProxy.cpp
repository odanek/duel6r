/*
 * ClientGameProxy.cpp
 *
 *  Created on: Mar 21, 2020
 *      Author: fanick
 */

#include "ClientGameProxy.h"
#include "../Game.h"
namespace Duel6 {
    namespace net {

        ClientGameProxy::ClientGameProxy()
        {
            // TODO Auto-generated constructor stub

        }

        ClientGameProxy::~ClientGameProxy()
        {
            // TODO Auto-generated destructor stub
        }
        template<>
        void ClientGameProxy::handleEvent(GameState &e) {
            handle(e);
        }
        template<>
        void ClientGameProxy::handleEvent(NextRound &nr) {
            handle(nr);
        }
        void ClientGameProxy::handle(Player & p) {
            auto & position = p.position;
            game->players[p.id].setPosition(position.x, position.y, position.z);
        }

        void ClientGameProxy::handle(GameState & s) {
            for(auto & player: s.players){
                auto & position = player.position;
                auto & dst = game->players[player.id];
                if(!game->isServer) {
                 dst.setPosition(position.x, position.y, position.z);
                }
                dst.setControllerState(player.controls);
            }
        }

        void ClientGameProxy::handle(NextRound & nr) {
            game->onNextRound();
        }

        //this should only happen on the server side when the client called us with this request
        void ClientGameProxy::handle(Peer & peer, RequestGameState & r) {
            sendGameState(peer, *game);
        }

        void ClientGameProxy::handle(ObjectBase &o){
            handleObject(o);
        }
        void ClientGameProxy::handle(EventBase &e){
            handleEvent(e);
        }

        void ClientGameProxy::nextRound() {
            game->onNextRound();
        }
    } /* namespace net */
} /* namespace Duel6 */
