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
                for(auto &s : peer->snapshot){
                    s.clear();
                }
                peer->send(NextRound());
            }
        }

        void ServerGameProxy::sendGameStateUpdate(Game &game) {
            for (auto &peer : peers) {
                GameStateUpdate gsu;
                gsu.inputTick = game.tick;
                gsu.players.reserve(game.getPlayers().size());

                for (auto &player : game.getPlayers()) {
                    if (!(player.local || game.isServer)){
                        continue;
                    }
                    Player p;
                    p.id = player.getId();
                    p.clientLocalId = player.getClientLocalId();

                    if (player.local) {
                        player.unconfirmedInputs[game.tick % 128] = player.getControllerState();
                        for(size_t i = 0; i < 16 ; i++){
                            p.unconfirmedInputs[i] = player.unconfirmedInputs[(game.tick - 15 + i) % 128];
                        }
                        p.changed[Player::FIELDS::CONTROLS] = true;
                        p.changed[Player::FIELDS::UNCONFIRMEDINPUTS] = true;
                        p.controls = player.getControllerState();
                    }
                    if (game.isServer) {
                        const auto &collidingEntity = player.getCollider();
                        const auto &position = collidingEntity.position;
                        const auto &externalForces = collidingEntity.externalForces;
                        const auto &externalForcesSpeed = collidingEntity.externalForcesSpeed;
                        const auto &velocity = collidingEntity.velocity;
                        const auto &acceleration = collidingEntity.acceleration;
                        p.position = { position.x, position.y};
                        p.externalForces = { externalForces.x, externalForces.y };
                        p.externalForcesSpeed = { externalForcesSpeed.x, externalForcesSpeed.y};
                        p.velocity = { velocity.x, velocity.y};
                        p.acceleration = { acceleration.x, acceleration.y};
                        p.flags = player.getFlags();
                        p.life = player.getLife();
                        p.air = player.getAir();
                        p.ammo = player.getAmmo();
                        p.weaponId = player.getWeapon().getId();
                        p.orientationLeft = { player.getOrientation() == Orientation::Left };
                        peer->snapshot[game.tick % 32][p.id] = p;
                    }


                    if( game.isServer && (std::abs(game.tick - peer->receivedInputsTick % 32000) < 32 || peer->snapshot[peer->receivedInputsTick % 32].count(p.id) > 0)){
                        Player result = Player::diff(p, peer->snapshot[peer->receivedInputsTick % 32][p.id]);
                        gsu.players.push_back(result);
                    } else {
                        gsu.players.push_back(p);
                    }

                }


                gsu.confirmInputTick = peer->receivedInputsTick;
                peer->sendUnreliable(gsu); // @suppress("Ambiguous problem")
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
