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
            auto & bonusList = game.getRound().getWorld().getBonusList();
            s.bonuses.reserve(bonusList.getBonuses().size());
            s.weapons.reserve(bonusList.getLyingWeapons().size());
            for(const auto & bonus : bonusList.getBonuses()){
                Bonus b;
                b.type = static_cast<BonusType>(bonus.getType()->getId());
                b.duration = bonus.getDuration();
                b.position = {bonus.getPosition().x, bonus.getPosition().y};
                b.textureIndex = bonus.getTextureIndex();
                b.bonusId = bonus.getId();
                s.bonuses.push_back(b);
            }
            for(const auto & weapon : bonusList.getLyingWeapons()){
                Weapon w;
                Collider &c = w.collider;
                const auto &collider = weapon.getCollider();
                w.type = static_cast<WeaponType>(weapon.getWeapon().getId());
                w.bullets = weapon.getBullets();

                c.position = {collider.position.x, collider.position.y};
                c.acceleration = {collider.acceleration.x, collider.acceleration.y};
                c.externalForces = {collider.externalForces.x, collider.externalForces.y};
                c.externalForcesSpeed = {collider.externalForcesSpeed.x, collider.externalForcesSpeed.y};
                c.velocity = {collider.velocity.x, collider.velocity.y};

                w.weaponId = weapon.getId();
                w.pickTimeout = weapon.pickTimeout;
                w.remainingReloadTime = weapon.remainingReloadTime;
                s.weapons.push_back(w);
            }
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

                p.bonusType = static_cast<BonusType>(player.getBonus()->getId());
                p.bonusRemainingTime = player.getBonusRemainingTime();
                p.bonusDuration = player.getBonusDuration();
                p.alpha = player.getAlpha();
                p.bodyAlpha = player.getBodyAlpha();

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
