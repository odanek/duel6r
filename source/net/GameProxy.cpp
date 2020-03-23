/*
 * GameProxy.cpp
 *
 *  Created on: Mar 21, 2020
 *      Author: fanick
 */

#include "GameProxy.h"
#include "object/objects.h"
#include "event/events.h"
#include "../Game.h"
namespace Duel6 {
    namespace net {
        GameProxy::GameProxy()
        {
            // TODO Auto-generated constructor stub

        }

        GameProxy::~GameProxy()
        {
            // TODO Auto-generated destructor stub
        }

        void GameProxy::handle(Peer &peer, RequestGameState &r) {

        }
        void GameProxy::sendGameState(std::list<Peer*> &peers, Game &game) {
            GameState s;
            s.players.reserve(game.getPlayers().size());
            size_t cnt = 0;
            for (auto &player : game.getPlayers()) {
                Player p;
                p.id = cnt++;
                const auto &playerPosition = player.getPosition();
                p.position = { playerPosition.x, playerPosition.y, playerPosition.z };
                p.controls = player.getControllerState();
                s.players.push_back(p);

            }
            s.state = GameStateState::RUNNING;
            for (auto &peer : peers) {
                peer->send(s); // @suppress("Ambiguous problem")
            }

        }
        void GameProxy::sendGameState(Peer &peer, Game &game) {
            GameState s;
            s.players.reserve(game.getPlayers().size());
            size_t cnt = 0;
            for (auto &player : game.getPlayers()) {
                Player p;
                p.id = cnt++;
                const auto &playerPosition = player.getPosition();
                p.position = { playerPosition.x, playerPosition.y, playerPosition.z };
                s.players.push_back(p);
            }

            s.state = GameStateState::RUNNING;
            peer.send(s);
        }
    } /* namespace net */
} /* namespace Duel6 */
