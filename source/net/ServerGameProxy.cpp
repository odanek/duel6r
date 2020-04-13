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
        uint8_t xor_128 = 127; // % operator yields also negative results
        auto xor_32768 = 0x7fff;
        auto xor_32 = 31;
        const uint16_t xor_64 = 63;

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
            auto &world = sr.world;
            world.elevators.reserve(level.elevators.size());
            for (auto &e : level.elevators) {
                std::vector<Duel6::net::ControlPoint> controlPoints = container_cast<std::vector<Duel6::net::ControlPoint>>(e.controlPoints);
                Elevator elevator(controlPoints,
                    e.circular,
                    e.section,
                    e.remainingWait,
                    e.forward,
                    e.distance,
                    e.travelled,
                    Vector(e.position.x, e.position.y, e.position.z),
                    Vector(e.velocity.x, e.velocity.y, e.velocity.z),
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
            peers.clear();
        }

        void ServerGameProxy::handle(ObjectBase &o) {
            ;
        }
        void ServerGameProxy::handle(EventBase &e) {
            ;
        }
        void ServerGameProxy::nextRound() {
            for (auto &peer : peers) {
                for (auto &s : peer->snapshot) {
                    s.clear();
                }
                peer->send(NextRound());
            }
        }

        void ServerGameProxy::sendInputs(Game &game) {
            for (auto &peer : peers) {
                if(!(peer->peerUpdateState == PeerUpdateState::GAMESTATE_RECEIVED || peer->peerUpdateState == PeerUpdateState::RUNNING)){
                    continue;
                }
                PlayerInputsUpdate piu;
                auto & players = game.getPlayers();

                piu.confirmInputTick = peer->confirmedInputsTick;
                piu.inputTick = game.tick;

                piu.playersInputs.reserve(players.size());
                for(auto & player: players){
                    if(!player.local){
                        continue;
                    }
                    PlayerInputs pi;
                    pi.id = player.getId();
                    player.unconfirmedInputs[game.tick & xor_128] = player.getControllerState();
                    for (size_t i = 0; i < 16; i++) {
                        pi.unconfirmedInputs[i] = player.unconfirmedInputs[(game.tick - 15 + i) & xor_128];
                    }
                    piu.playersInputs.push_back(pi);
                }
                peer->sendUnreliable(piu);
            }
        }

        void ServerGameProxy::sendGameStateUpdate(Game &game) {
            if(!game.isServer){
                sendInputs(game);
                return;
            }
            for (auto &peer : peers) {
                if(!(peer->peerUpdateState == PeerUpdateState::GAMESTATE_RECEIVED || peer->peerUpdateState == PeerUpdateState::RUNNING)){
                    continue;
                }
                GameStateUpdate gsu;
                gsu.confirmInputTick = peer->receivedInputsTick;
                gsu.snapshotTick = peer->confirmedInputsTick;
                gsu.inputTick = game.tick;
                gsu.players.reserve(game.getPlayers().size());

                for (auto &player : game.getPlayers()) {
                    if (!(player.local || game.isServer)) {
                        continue;
                    }
                    Player p;
                    p.rtt = peer->getRTT();
                    p.id = player.getId();
                    p.clientLocalId = player.getClientLocalId();
                    p.debug = game.tick;

                    if (player.local) {
                        player.unconfirmedInputs[game.tick & xor_128] = player.getControllerState();
                        for (size_t i = 0; i < 64; i++) {
                            p.unconfirmedInputs[i] = player.unconfirmedInputs[(game.tick - 63 + i) & xor_128];
                        }
                        p.changed[Player::FIELDS::CONTROLS] = true;
                        p.changed[Player::FIELDS::UNCONFIRMEDINPUTS] = true;
                        p.controls = player.getControllerState();
                        p.rtt = 0;
                    }
                    if (game.isServer) {
                        const auto &collidingEntity = player.getCollider();
                        const auto &position = collidingEntity.position;
                        const auto &externalForces = collidingEntity.externalForces;
                        const auto &externalForcesSpeed = collidingEntity.externalForcesSpeed;
                        const auto &velocity = collidingEntity.velocity;
                        const auto &acceleration = collidingEntity.acceleration;
                        p.position = { position.x, position.y };
                        p.externalForces = { externalForces.x, externalForces.y };
                        p.externalForcesSpeed = { externalForcesSpeed.x, externalForcesSpeed.y };
                        p.velocity = { velocity.x, velocity.y };
                        p.acceleration = { acceleration.x, acceleration.y };
                        p.flags = player.getFlags();
                        p.life = player.getLife();
                        p.air = player.getAir();
                        p.ammo = player.getAmmo();
                        p.weaponId = player.getWeapon().getId();
                        p.orientationLeft = { player.getOrientation() == Orientation::Left };

                        peer->snapshot[game.tick & xor_64][p.id] = p;
                    }

                    if (game.isServer &&
                        (((gsu.inputTick - gsu.snapshotTick) & xor_32768) < 32
                            && peer->snapshot[gsu.snapshotTick & xor_64].count(p.id) > 0
                            && peer->snapshot[gsu.snapshotTick & xor_64][p.id].debug == gsu.snapshotTick)) {
                        Player::diff(p, peer->snapshot[gsu.snapshotTick & xor_64][p.id]);
                    }
                    gsu.players.push_back(p);
                }
                peer->sendUnreliable(gsu);
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
