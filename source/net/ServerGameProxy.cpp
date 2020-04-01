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

        void ServerGameProxy::startRound(Duel6::Level &level) {
            StartRound sr;
            auto & world = sr.world;
            world.elevators.reserve(level.elevators.size());
            for(auto & e: level.elevators){
                std::vector<Duel6::net::ControlPoint> controlPoints = container_cast<std::vector<Duel6::net::ControlPoint>>(e.controlPoints);
//                e.circular = elevator.circular;
//                e.distance = elevator.distance;
                Elevator elevator(controlPoints,
                           e.circular,
                           e.section,
                           e.remainingWait,
                           e.forward,
                           e.distance,
                           e.travelled,
                           Vector(e.position.x,e.position.y,e.position.z),
                           Vector(e.velocity.x,e.velocity.y,e.velocity.z),
                           e.started);
                sr.world.elevators.emplace_back(std::move(elevator));
             //   Duel6::net::ControlPoint
            }
            world.width = level.width;
            world.height = level.height;
            world.background = level.background;
            world.waterBlock = level.waterBlock;
            world.levelData = level.levelData;
            world.waterLevel = level.waterLevel;
            world.raisingWater = level.raisingWater;

            for (auto &peer : peers) {
                peer->send(sr); // @suppress("Ambiguous problem")
            }
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

        void ServerGameProxy::sendGameStateUpdate(Game &game) {
            GameStateUpdate gsu;
            gsu.players.reserve(game.getPlayers().size());

            for (auto &player : game.getPlayers()) {

                Player p;
                p.id = player.getId();
                p.clientLocalId = player.getClientLocalId();
                const auto &playerPosition = player.getPosition();
                p.position = { playerPosition.x, playerPosition.y, playerPosition.z };
                p.orientationLeft = {player.getOrientation() == Orientation::Left};
                if(player.local){
                    p.controls = player.getControllerState();
                }
                gsu.players.push_back(p);
            }

            for (auto &peer : peers) {
                peer->send(gsu); // @suppress("Ambiguous problem")
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
