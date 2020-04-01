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
            auto pos = idmap[p.id]; /// ASI NECO BLBE
            auto &player = game->players[pos];
            if(!game->isServer){
                player.setPosition(position.x, position.y, position.z);
            }
            if(!player.local){
                player.setControllerState(p.controls);
                player.setOrientation(p.orientationLeft ? Orientation::Left : Orientation::Right);
            }
        }
        void ClientGameProxy::handle(GameStateUpdate &gsu) {
            for(auto & p : gsu.players){
                handle(p);
            }
        }
        void ClientGameProxy::handle(GameState &sr) {
            clientId = sr.clientId;
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
                            if(clientId == p.clientId){ // player local to the client

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
                        for(auto & d: playerDefinitions){
                            auto playerId = d.getPlayerId();
                            idmap[playerId] = d.playerPos;
                            idmapBack[d.playerPos] = playerId;
                        }

        }

        void ClientGameProxy::handle(NextRound &nr) {
            game->onNextRound();
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
            for(auto & d: playerDefinitions){
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
