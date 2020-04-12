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
        void GameProxy::sendGameState(std::list<Peer*> &&peers, Game &game) {
            sendGameState(peers, game);
        }
        void GameProxy::sendGameState(std::list<Peer*> &peers, Game &game) {
            GameState s;
            s.tick = game.tick;
            s.players.reserve(game.getPlayers().size());
            s.playerProfiles.reserve(s.players.size());
            for (auto &player : game.getPlayers()) {
                Player p;
                p.debug = game.tick;
                p.id = player.getId();
                p.clientLocalId = player.getClientLocalId();

                const auto &collidingEntity = player.getCollider();
                const auto &position = collidingEntity.position;
                const auto &externalForces = collidingEntity.externalForces;
                const auto &externalForcesSpeed = collidingEntity.externalForcesSpeed;
                const auto &velocity = collidingEntity.velocity;
                const auto &acceleration = collidingEntity.acceleration;
                p.position = { position.x, position.y};
                p.externalForces = { externalForces.x, externalForces.y};
                p.externalForcesSpeed = { externalForcesSpeed.x, externalForcesSpeed.y};
                p.velocity = { velocity.x, velocity.y};
                p.acceleration = { acceleration.x, acceleration.y};

                p.controls = player.getControllerState();
                p.flags = player.getFlags();
                p.life = player.getLife();
                p.air = player.getAir();
                p.ammo = player.getAmmo();
                p.weaponId = player.getWeapon().getId();
                p.orientationLeft = { player.getOrientation() == Orientation::Left };

                s.players.push_back(p);

                PlayerProfile pp;
                pp.playerId = p.id;
                pp.clientId = player.getClientId();
                pp.clientLocalId = player.getClientLocalId();
                pp.name = player.getPerson().getName();
                pp.team = player.getTeam();

                PlayerProfile::Skin &skin = pp.skin;
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
                s.playerProfiles.push_back(pp);

            }
            auto & world = s.world;

            world.gameSettings = game.settings;
            Level &level = world.level;
            level = Level((game.round->getWorld().getLevel()));
            auto & elvtrs = game.round->getWorld().getElevatorList().getElevators();
            level.elevators = container_cast<std::vector<Elevator>>(elvtrs);
            world.currentRound = game.currentRound;
            world.playedRounds = game.playedRounds;

            s.state = GameStateState::RUNNING;
            for (auto &peer : peers) {
                s.clientId = peer->getIncomingPeerID() + 100;
                peer->send(s); // @suppress("Ambiguous problem")
            }

        }
        void GameProxy::sendGameState(Peer &peer, Game &game) {
            sendGameState(std::list<Peer*>(1, &peer), game);
        }
    } /* namespace net */
} /* namespace Duel6 */
