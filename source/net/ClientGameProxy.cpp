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

        ClientGameProxy::ClientGameProxy()
        {
            // TODO Auto-generated constructor stub

        }

        ClientGameProxy::~ClientGameProxy()
        {
            // TODO Auto-generated destructor stub
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
                player.setPosition(position.x, position.y, position.z);
                //TODO lag compensation
                //        player.lastConfirmedPos = player.getPosition();
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
                if(idmap.count(p.id) == 0){
                    continue;
                }
                auto pos = idmap[p.id];
                auto &player = game->players[pos];
                player.lastConfirmedTick = gsu.confirmInputTick;
                if (!game->isServer) {
                    auto & position = p.position;
                    auto & externalForces = p.externalForces;
                    auto & externalForcesSpeed = p.externalForcesSpeed;
                    auto & velocity = p.velocity;
                    auto & acceleration = p.acceleration;

                    auto & collidingEntity = player.getCollider();
                    collidingEntity.position = {position.x, position.y, position.z};
                    collidingEntity.externalForces = {externalForces.x, externalForces.y, externalForces.z};
                    collidingEntity.externalForcesSpeed = {externalForcesSpeed.x, externalForcesSpeed.y, externalForcesSpeed.z};
                    collidingEntity.velocity = {velocity.x, velocity.y, velocity.z};
                    collidingEntity.acceleration = {acceleration.x, acceleration.y, acceleration.z};


                   // player.setPosition(position.x, position.y, position.z);
                    player.setFlags(p.flags);
                    player.setLife(p.life);
                    player.setAir(p.air);
                    player.setAmmo(p.ammo);
                    // player.setWeapon(p.weaponId);
                    player.setOrientation(p.orientationLeft ? Orientation::Left : Orientation::Right);
                    if( player.local && game->tick - gsu.confirmInputTick < 16) { // cap it at 16 frames to avoid run-away of death
                      game->compensateLag(gsu.confirmInputTick); // run client-side prediction
                    }
                } else {

                }
                if (!player.local) {
                    size_t missed = player.lastConfirmedTick - player.tick;
                    if(missed > 15){
                        missed = 15;
                    }
                    for(size_t i = 0; i <= missed; i++){
                        player.unconfirmedInputs[(player.tick + i) % 128] = p.unconfirmedInputs[15 - missed + i];
                    }
                    //player.unconfirmedInputs[player.tick % 128] = p.controls;
                   // player.setControllerState(p.controls);
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
//
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

                    /// TADY JE NECO ROZESRANYHO //
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
                auto & position = p.position;
                auto & externalForces = p.externalForces;
                auto & externalForcesSpeed = p.externalForcesSpeed;
                auto & velocity = p.velocity;
                auto & acceleration = p.acceleration;

                auto & collidingEntity = player.getCollider();
                collidingEntity.position = {position.x, position.y, position.z};
                collidingEntity.externalForces = {externalForces.x, externalForces.y, externalForces.z};
                collidingEntity.externalForcesSpeed = {externalForcesSpeed.x, externalForcesSpeed.y, externalForcesSpeed.z};
                collidingEntity.velocity = {velocity.x, velocity.y, velocity.z};
                collidingEntity.acceleration = {acceleration.x, acceleration.y, acceleration.z};

             //   game->compensateLag(gsu.confirmInputTick); // run client-side prediction
                player.setFlags(p.flags);
                player.setLife(p.life);
                player.setAir(p.air);
                player.setAmmo(p.ammo);
                // player.setWeapon(p.weaponId);
                player.setOrientation(p.orientationLeft ? Orientation::Left : Orientation::Right);
                player.setControllerState(p.controls);
            }

        }

        void ClientGameProxy::handle(NextRound &nr) {
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
//                if (!player.local) {
//                    continue;
//                }
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
