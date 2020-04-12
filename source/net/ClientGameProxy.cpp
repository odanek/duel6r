/*
 * ClientGameProxy.cpp
 *
 *  Created on: Mar 21, 2020
 *      Author: fanick
 */

#include "ClientGameProxy.h"
#include "../Game.h"
#include "../PersonProfile.h"
#include "../Level.h"
namespace Duel6 {
    namespace net {

        void ClientGameProxy::setPeerReference(Peer &peer) {
            this->peer = &peer;
        }

        void ClientGameProxy::setGameReference(Game &g) {
            game = &g;
            defaultSounds = PlayerSounds::makeDefault(game->getAppService().getSound());
        }
        void ClientGameProxy::handle(Player &p) {
            auto &position = p.position;
            auto pos = idmap[p.id];
            auto &player = game->players[pos];

            if (!game->isServer) {
                player.setPosition(position.x, position.y, 0);
            }
            if (!player.local) {
                player.setControllerState(p.controls);
                player.setOrientation(p.orientationLeft ? Orientation::Left : Orientation::Right);
            }
        }
        void ClientGameProxy::lateReceive(tick_t lateTick) {
            for (auto &player : game->players) {
                player.lateTicks++;
            }
        }
        void ClientGameProxy::handle(GameStateUpdate &gsu) {

            for (auto &p : gsu.players) {
                if (idmap.count(p.id) == 0) {
                    std::cerr << "Player id " << p.id << " not found, skipping\n";
                    continue;
                }
                auto pos = idmap[p.id];
                auto &player = game->players[pos];

                if (!game->isServer) {
                    uint16_t xor_32768 = 32767;
                    uint16_t xor_32 = 31;
                    bool shouldLoadSnapshot = false;
                    if(p.changed[0] || p.changed.count() != Player::FIELDS::_SIZE - 1){
                        shouldLoadSnapshot = true;
                    }
                    if (((gsu.inputTick - gsu.confirmInputTick) & xor_32768) < 32
                        && peer->snapshot[gsu.confirmInputTick & xor_32].count(p.id) > 0) {
                        if (peer->snapshot[gsu.confirmInputTick & xor_32][p.id].debug == gsu.confirmInputTick) {
                            Player &confirmed = peer->snapshot[gsu.confirmInputTick & xor_32][p.id];
                            Player::fillinFromPreviousConfirmed(confirmed, p);
                            shouldLoadSnapshot = false;
                        } else {
                            /* debug */
                        }
                    }
                    if(shouldLoadSnapshot){
                        // correct snapshot not found,
                        // wait out the server to send full copy
                        continue; // skip this player to not screw things
                    } else {
                        player.lastConfirmedTick = gsu.confirmInputTick;
                    }

                    auto &position = p.position;
                    auto &externalForces = p.externalForces;
                    auto &externalForcesSpeed = p.externalForcesSpeed;
                    auto &velocity = p.velocity;
                    auto &acceleration = p.acceleration;

                    auto &collidingEntity = player.getCollider();
                    Vector2D diff = {
                        position.x - collidingEntity.position.x,
                        position.y - collidingEntity.position.y
                    };
                     if(diff.x * diff.x + diff.y * diff.y > 0.9){
                         collidingEntity.position = { position.x, position.y, collidingEntity.position.z };
                         collidingEntity.externalForcesSpeed = { externalForcesSpeed.x, externalForcesSpeed.y, collidingEntity.externalForcesSpeed.z };
                     } else {
                         collidingEntity.externalForcesSpeed.x += diff.x;
                         collidingEntity.externalForcesSpeed.y += diff.y;
                     }
                    collidingEntity.externalForces = { externalForces.x, externalForces.y, collidingEntity.externalForces.z };
                    collidingEntity.velocity = { velocity.x, velocity.y, collidingEntity.velocity.z };
                    collidingEntity.acceleration = { acceleration.x, acceleration.y, collidingEntity.acceleration.z };
                    player.setFlags(p.flags);
                    player.setLife(p.life);
                    player.setAir(p.air);
                    player.setAmmo(p.ammo);
                    player.rtt = p.rtt;
                    if (player.getWeapon().getId() != p.weaponId) {
                        player.setWeapon(p.weaponId); //resets animation
                    }
                    player.setOrientation(p.orientationLeft ? Orientation::Left : Orientation::Right);
                    //TODO handle 16bit wrap-around
                    if (player.local && ((game->tick - gsu.confirmInputTick) & xor_32768 < 8)) { // cap it at 8 frames to avoid run-away of death
                        Uint32 ms = 1000 / 90;
                        Uint32 drift = player.rtt / ms;
                        game->compensateLag(game->tick - (game->tick -  gsu.confirmInputTick) / 2 ); // run client-side prediction
                    }
                    peer->snapshot[gsu.inputTick & xor_32][p.id] = p;
                } else {
                    player.lastConfirmedTick = gsu.confirmInputTick;
                    player.rtt = peer->getRTT();
                }
                if (!player.local) {
                    size_t missed = player.lastConfirmedTick - player.tick;
                    if (missed > 16) {
                        missed = 16;
                    }
                    for (size_t i = 0; i < missed; i++) {
                        player.unconfirmedInputs[(player.tick + i) % 128] = p.unconfirmedInputs[16 - missed + i];
                    }
                }
            }
        }
        void ClientGameProxy::handle(GameState &sr) {
            clientId = sr.clientId;
            game->tick = sr.tick;

            auto &w = sr.world.level;
            std::vector<Duel6::Elevator> elevators;
            elevators.reserve(w.elevators.size());

            for (auto &e : w.elevators) {
                Duel6::Vector pos(e.position.x, e.position.y, e.position.z);
                Duel6::Vector vel(e.velocity.x, e.velocity.y, e.velocity.z);
                elevators.emplace_back(
                    std::move(container_cast<std::vector<Duel6::Elevator::ControlPoint>>(e.controlPoints)),
                    e.circular,
                    e.section,
                    e.remainingWait,
                    e.forward,
                    e.distance,
                    e.travelled,
                    pos,
                    vel,
                    e.started);
            }

            game->getSettings().setEnabledWeapons(sr.world.gameSettings.enabledWeapons);
            game->getSettings().setMaxRounds(sr.world.gameSettings.maxRounds);
            game->currentRound = sr.world.currentRound;
            game->playedRounds = sr.world.playedRounds;

            game->onStartRound(std::make_unique<Duel6::Level>(
                w.width,
                w.height,
                w.background,
                w.waterBlock,
                w.levelData,
                w.waterLevel,
                w.raisingWater,
                elevators
                ));
            game->maxPlayerId = 0; // todo: reset players counter
            std::vector<Game::PlayerDefinition> playerDefinitions;
            playerDefinitions.reserve(sr.players.size());

            for (auto &p : sr.playerProfiles) {
                if (clientId == p.clientId) { // player local to the client
                    idmap[p.playerId] = p.clientLocalId;
                    game->players[p.clientLocalId].setId(p.playerId);
                    game->players[p.clientLocalId].setClientId(p.clientId);
                    idmapBack[p.clientLocalId] = p.playerId;
                    continue; // local player
                }
                Person &person = persons.emplace_back(p.name, nullptr);
                PlayerSkinColors colors;
                colors.setHair(p.skin.hair);
                colors.setHeadBand(p.skin.headBand);
                int cid = 0;
                for (auto c : p.skin.colors) {
                    if (cid > 10) {
                        return;
                    }
                    Color color { c.red, c.green, c.blue, c.alpha };
                    colors.set((PlayerSkinColors::BodyPart) cid++, color);
                }
                playerDefinitions.emplace_back(
                    person,
                    colors,
                    defaultSounds,
                    PlayerControlsManager::getNoop(),
                    p.team,
                    p.clientId,
                    p.playerId);
            }
            game->joinPlayers(playerDefinitions);
            for (auto &d : playerDefinitions) {
                auto playerId = d.getPlayerId();
                idmap[playerId] = d.playerPos;
                idmapBack[d.playerPos] = playerId;
            }

            for (auto &p : sr.players) {
                Duel6::Player &player = game->players[idmap[p.id]];
                player.tick = game->tick;
                auto &position = p.position;
                auto &externalForces = p.externalForces;
                auto &externalForcesSpeed = p.externalForcesSpeed;
                auto &velocity = p.velocity;
                auto &acceleration = p.acceleration;

                auto &collidingEntity = player.getCollider();
                collidingEntity.position = { position.x, position.y, collidingEntity.position.z };
                collidingEntity.externalForces = { externalForces.x, externalForces.y, collidingEntity.externalForces.z };
                collidingEntity.externalForcesSpeed = { externalForcesSpeed.x, externalForcesSpeed.y, collidingEntity.externalForcesSpeed.z };
                collidingEntity.velocity = { velocity.x, velocity.y, collidingEntity.velocity.z };
                collidingEntity.acceleration = { acceleration.x, acceleration.y, collidingEntity.acceleration.z };

                //   game->compensateLag(gsu.confirmInputTick); // run client-side prediction
                player.setFlags(p.flags);
                player.setLife(p.life);
                player.setAir(p.air);
                player.setAmmo(p.ammo);
                player.setWeapon(p.weaponId);
                player.setOrientation(p.orientationLeft ? Orientation::Left : Orientation::Right);
                player.setControllerState(p.controls);

                peer->snapshot[sr.tick % 32][p.id] = p;
            }

        }
        void ClientGameProxy::netStopped() {
            Context::pop();
        }
        void ClientGameProxy::handle(NextRound &nr) {
            for (auto &s : peer->snapshot) {
                s.clear();
            }
            game->onNextRound();
        }
        tick_t ClientGameProxy::getTick() const {
            return game->tick;
        }
        //this should only happen on the server side when the client called us with this request
        void ClientGameProxy::handle(Peer &peer, RequestGameState &r) {
            std::vector<Game::PlayerDefinition> playerDefinitions;
            playerDefinitions.reserve(r.connectingPlayers.size());

            for (auto &p : r.connectingPlayers) {
                Person &person = persons.emplace_back(p.name, nullptr);
                PlayerSkinColors colors;
                colors.setHair(p.skin.hair);
                colors.setHeadBand(p.skin.headBand);
                int cid = 0;
                for (auto c : p.skin.colors) {
                    if (cid > 10) {
                        return;
                    }
                    Color color { c.red, c.green, c.blue, c.alpha };
                    colors.set((PlayerSkinColors::BodyPart) cid++, color);
                }
                playerDefinitions.emplace_back(
                    person,
                    colors,
                    defaultSounds,
                    PlayerControlsManager::getNoop(),
                    p.team,
                    peer.getIncomingPeerID() + 100,
                    p.clientLocalId);
            }
            game->joinPlayers(playerDefinitions);
            for (auto &d : playerDefinitions) {
                idmap[d.getPlayerId()] = d.playerPos;
                idmapBack[d.playerPos] = d.getPlayerId();
            }
            sendGameState(peer, *game);
        }

        void ClientGameProxy::handle(ObjectBase &o) {
            handleObject(o);
        }
        void ClientGameProxy::handle(EventBase &e) {
            handleEvent(e);
        }

        RequestGameState ClientGameProxy::getRequestGameState() {
            RequestGameState r;

            r.connectingPlayers.reserve(game->getPlayers().size());
            for (auto &player : game->getPlayers()) {
                PlayerProfile cp;
                PlayerProfile::Skin &skin = cp.skin;
                cp.playerId = player.getId();
                cp.clientLocalId = player.getId();
                const Person &person = player.getPerson();
                cp.name = person.getName();
                cp.team = player.getTeam();
                const PlayerSkinColors &colors = player.getSkin().getColors();
                skin.hair = (PlayerProfile::Hair) colors.getHair();
                skin.headBand = colors.hasHeadBand();

                size_t i = 0;
                for (auto &c : skin.colors) {
                    PlayerSkinColors::BodyPart bp = (PlayerSkinColors::BodyPart) i;
                    c.red = colors.get(bp).getRed();
                    c.green = colors.get(bp).getGreen();
                    c.blue = colors.get(bp).getBlue();
                    c.alpha = colors.get(bp).getAlpha();

                    i++;
                }
                r.connectingPlayers.push_back(cp);
            }
            return r;
        }

        void ClientGameProxy::handle(StartRound &sr) {
            std::vector<Duel6::Elevator> elevators;
            elevators.reserve(sr.world.elevators.size());

            for (auto &e : sr.world.elevators) {
                Duel6::Vector pos(e.position.x, e.position.y, e.position.z);
                Duel6::Vector vel(e.velocity.x, e.velocity.y, e.velocity.z);
                elevators.emplace_back(
                    std::move(container_cast<std::vector<Duel6::Elevator::ControlPoint>>(e.controlPoints)),
                    e.circular,
                    e.section,
                    e.remainingWait,
                    e.forward,
                    e.distance,
                    e.travelled,
                    pos,
                    vel,
                    e.started);
            }

            auto &w = sr.world;
            game->onStartRound(std::make_unique<Duel6::Level>(
                w.width,
                w.height,
                w.background,
                w.waterBlock,
                w.levelData,
                w.waterLevel,
                w.raisingWater,
                elevators
                ));
        }

        void ClientGameProxy::nextRound() {
            game->onNextRound();
        }
    } /* namespace net */
} /* namespace Duel6 */
