/*
 * ServerGameProxy.cpp
 *
 *  Created on: Mar 21, 2020
 *      Author: fanick
 */

#include "ServerGameProxy.h"
#include "event/events.h"
#include "object/objects.h"

#include "../Game.h"
namespace Duel6 {
    namespace net {
        uint8_t xor_128 = 127; // % operator yields also negative results
        auto xor_32768 = 0x7fff;
        auto xor_32 = 31;
        const uint16_t xor_64 = 63;
        void loadNetWeapon(Weapon &w, const LyingWeapon &weapon);
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

        void ServerGameProxy::startRound(Int32 playedRounds, Duel6::Level &level) {
            StartRound sr;
            sr.round = playedRounds;
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

        void ServerGameProxy::gameEnded() {
            peers.clear();
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
        void ServerGameProxy::handle(Peer &peer, ObjectBase &o) {

        }
        void ServerGameProxy::handle(Peer &peer, EventBase &e) {

        }
        void ServerGameProxy::nextRound() {
            for (auto &peer : peers) {
                for (auto &s : peer->snapshot) {
                    s.clear();
                }

                peer->send(NextRound());
            }
        }

        void  ServerGameProxy::requestNextRound(Int32 currentRound) {
            for (auto &peer : peers) {
                if (!(peer->peerUpdateState == PeerUpdateState::GAMESTATE_RECEIVED || peer->peerUpdateState == PeerUpdateState::RUNNING)) {
                    continue;
                }
                RequestNextRound rnr;
                rnr.currentRound = currentRound;
                peer->send(rnr);
            }
        }

        void ServerGameProxy::sendInputs(Game &game) {
            for (auto &peer : peers) {
                if (!(peer->peerUpdateState == PeerUpdateState::GAMESTATE_RECEIVED || peer->peerUpdateState == PeerUpdateState::RUNNING)) {
                    continue;
                }
                PlayerInputsUpdate piu;
                auto &players = game.getPlayers();
                piu.confirmInputTick = peer->receivedInputsTick; // TODO: add explanation why this must not be peer->confirmedIntputsTick (which was wrong)
                piu.inputTick = game.tick;

                piu.playersInputs.reserve(players.size());
                for (auto &player : players) {
                    if(player.isDeleted()){
                        peer->snapshot[game.tick & xor_64].erase(player.getId());
                    }
                    if (!player.local || player.isDeleted()) {
                        continue;
                    }
                    PlayerInputs pi;
                    pi.id = player.getId();
                    player.unconfirmedInputs[player.tick & xor_128] = player.getControllerState();
                    for (size_t i = 0; i < 64; i++) {
                        pi.unconfirmedInputs[i] = player.unconfirmedInputs[(game.tick - 63 + i) & xor_128];
                    }
                    piu.playersInputs.push_back(pi);
                }
                peer->sendUnreliable(piu);
            }
        }

        void ServerGameProxy::sendGameStateUpdate(Game &game) {
            if (!game.isServer) {
                sendInputs(game);
                return;
            }
            Weapon w;
            bool weaponChosen = false;
            for(const auto & weapon : game.getRound().getWorld().getBonusList().getLyingWeapons()){
                if((int) weapon.id > lastSyncedWeapon){
                    lastSyncedWeapon = weapon.id;
                    weaponChosen = true;
                    loadNetWeapon(w, weapon);
                    break;
                }
            }
            if(!weaponChosen){
                lastSyncedWeapon = -1;
            }
            for (auto &peer : peers) {
                game.netstat.choke = peer->choke;
                if (!(peer->peerUpdateState == PeerUpdateState::GAMESTATE_RECEIVED || peer->peerUpdateState == PeerUpdateState::RUNNING)) {
                    continue;
                }
                if(weaponChosen){
                    peer->sendUnreliable(w);
                }
                GameStateUpdate gsu;
                gsu.confirmInputTick = peer->receivedInputsTick;
                gsu.snapshotTick = peer->confirmedInputsTick;
                gsu.hasWinner = game.getRound().hasWinner();
                gsu.inputTick = game.tick;
                if (gsu.inputTick - gsu.snapshotTick < 32) {
                    // crazy trick that actually solves problem with snapshot divergence
                    // we assume that game state updates get through the network most of the time.
                    // If the peer->confirmedInputsTick starts to diverge (client didn't find correct snapshot and
                    // waits for us to serve him fresh copy of the state), we use that one as the last known snapshot

                    // it actually screws things when order delivered out-of - client then lacks the snapshot
                    // edit: mixing snapshotTick and confirmInputTick is mixing server-side counter with client-side counter
                    // and could never have worked correctly
                    //   gsu.snapshotTick = gsu.confirmInputTick;
                    peer->choke = 0;
                    if(gsu.inputTick - gsu.snapshotTick > 2){
                        gsu.snapshotTick -= gsu.inputTick - gsu.snapshotTick  / 2;
                    }

                } else {
                    if(peer->choke < 32){
                        peer->choke = 32;
                    }
                }

                gsu.players.reserve(game.getPlayers().size());

                for (auto &player : game.getPlayers()) {
                    if(player.isDeleted()){
                        peer->snapshot[game.tick & xor_64].erase(player.getId());
                    }
                    if (!(player.local || game.isServer) || player.isDeleted()) {
                        continue;
                    }
                    Player p;
                    if(player.getClientId() == peer->getClientID()){
                        player.rtt = peer->getRTT();
                    }
                    p.rtt = player.rtt;
                    p.id = player.getId();
                    p.clientLocalId = player.getClientLocalId();
                    p.lastConfirmedTick = player.tick;
                    p.snapshotTick = gsu.inputTick;
                    player.unconfirmedInputs[player.tick & xor_128] = player.getControllerState();
                    const size_t maxInputs = Player::INPUTS;
                    for (size_t i = 0; i < maxInputs; i++) { //TODO: figure out if it should be game.tick - (maxInputs) or game.tick - (maxInputs - 1)
                        p.unconfirmedInputs[i] = player.unconfirmedInputs[(player.tick - (maxInputs - 1) + i) & xor_128];
                    }

                    p.controls = player.getControllerState();
//                        p.rtt = 0;
                    if (game.isServer) {
                        const auto &collidingEntity = player.getCollider();
                        const auto &position = collidingEntity.position;
                        const auto &externalForces = collidingEntity.externalForces;
                        const auto &externalForcesSpeed = collidingEntity.externalForcesSpeed;
                        const auto &velocity = collidingEntity.velocity;
                        const auto &lastCollisionCheck = collidingEntity.lastCollisionCheck;
                        const auto &acceleration = collidingEntity.acceleration;
                        p.position = { position.x, position.y };
                        p.externalForces = { externalForces.x, externalForces.y };
                        p.externalForcesSpeed = { externalForcesSpeed.x, externalForcesSpeed.y };
                        p.velocity = { velocity.x, velocity.y };
                        p.acceleration = { acceleration.x, acceleration.y };
                        p.lastCollisionCheck = lastCollisionCheck.onGround; //TODO all values
                        p.flags = player.getFlags();
                        p.life = player.getLife();
                        p.air = player.getAir();
                        p.ammo = player.getAmmo();
                        p.weaponId = player.getWeapon().getId();
                        p.orientationLeft = { player.getOrientation() == Orientation::Left };
                        p.bonusType = static_cast<BonusType>(player.getBonus()->getId());
                        p.bonusRemainingTime = player.getBonusRemainingTime();
                        p.bonusDuration = player.getBonusDuration();
                        p.timeSinceHit = player.getTimeSinceHit();
                        p.alpha = player.getAlpha();
                        p.bodyAlpha = player.getBodyAlpha();
                        p.timeToReload = player.getReloadTime();

                        p.loadFromPlayer(player);
                        p.score.loadFromPlayer(player);
                        peer->snapshot[game.tick & xor_64][p.id] = p;
                    }
                    player.rtt = p.rtt;

                    if(peer->choke > 0){
                        p.changed.set();
                        p.changed[Player::FIELDS::NO_CHANGE] = false;
                    } else
                    if (game.isServer &&
                        (((gsu.inputTick - gsu.snapshotTick) & xor_32768) < 64
                            && peer->snapshot[gsu.snapshotTick & xor_64].count(p.id) > 0
                            && peer->snapshot[gsu.snapshotTick & xor_64][p.id].snapshotTick == gsu.snapshotTick)) {
                        Player::diff(p, peer->snapshot[gsu.snapshotTick & xor_64][p.id]);
                    }
                    gsu.players.push_back(p);
                }
                if(peer->choke > 0) {
                    peer-> choke --;
                }

                peer->sendUnreliable(gsu);
            }
        }

        void ServerGameProxy::playersJoined(std::vector<PlayerDefinition> &playerDefinitions) {
            if(playerDefinitions.size() == 0){
                return; // avoid troubles !
            }
            PlayersJoined pj;
            pj.playerProfiles.reserve(playerDefinitions.size());
            Int32 clientId = 0;
            for (auto &player : playerDefinitions) {
                PlayerProfile cp;
                PlayerProfile::Skin &skin = cp.skin;
                cp.playerId = player.getPlayerId();
                cp.clientLocalId = player.getClientLocalId();
                cp.name = player.getPerson().getName();
                cp.team = player.getTeam();
                cp.clientId = player.getClientId();
                clientId = cp.clientId;
                const PlayerSkinColors &colors = player.getColors();
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
                pj.playerProfiles.push_back(cp);
            }

            for (auto &peer : peers) {
                if(peer->getClientID() != clientId){
                    peer->sendReliable(pj);
                }
            }
        }

        void ServerGameProxy::playersDisconnected(const std::vector<Int32> &ids) {
            PlayersDisconnected pd;
            pd.disconnectedId = ids;
            for (auto &peer : peers) {
                peer->sendReliable(pd);
            }
        }

        void ServerGameProxy::spawnBonus(Duel6::Bonus &bonus) {
            SpawnBonus sb;
            Bonus &b = sb.bonus;
            b.type = static_cast<BonusType>(bonus.getType()->getId());
            b.duration = bonus.getDuration();
            b.position = {bonus.getPosition().x, bonus.getPosition().y};
            b.textureIndex = bonus.getTextureIndex();
            b.bonusId = bonus.getId();

            for (auto &peer : peers) {
                peer->sendReliable(sb, Channel::BONUSES);
            }
        }

        void ServerGameProxy::spawnWeapon(LyingWeapon &weapon) {
            SpawnWeapon sw;
            Weapon &w = sw.weapon;
            loadNetWeapon(w, weapon);
            for (auto &peer : peers) {
                peer->sendReliable(sw, Channel::WEAPONS);
            }
        }

        void ServerGameProxy::pickWeapon(Duel6::Player &player, unsigned int weaponId) {
            PickWeapon pw;
            pw.playerId = player.getId();
            pw.weaponId = weaponId;
            for (auto &peer : peers) {
                peer->sendReliable(pw, Channel::WEAPONS);
            }
        }

        void ServerGameProxy::pickBonus(Duel6::Player &player, unsigned int bonusId) {
            PickBonus pb;
            pb.playerId = player.getId();
            pb.bonusId = bonusId;
            for (auto &peer : peers) {
                peer->sendReliable(pb, Channel::BONUSES);
            }
        }

        void ServerGameProxy::spawnShot(std::unique_ptr<Duel6::Shot> &shotPtr) {
            SpawnShot ss;
            auto & shot = *shotPtr.get();
            auto position = shot.getPositionVector();
            auto velocity = shot.getVelocityVector();

            ss.weaponId = shot.getWeapon().getId();
            ss.playerId = shot.getPlayer().getId();
            ss.shot.id = shot.getId();
            ss.shot.position.x = position.x;
            ss.shot.position.y = position.y;
            ss.orientationLeft = shot.getOrientation() == Orientation::Left;
            ss.powerful = shot.isPowerful();
            ss.power = shot.getPower();
            ss.bulletSpeed = shot.getBulletSpeed();
            ss.velocity.x = velocity.x;
            ss.velocity.y = velocity.y;
            for (auto &peer : peers) {
                peer->sendReliable(ss, Channel::SHOTS);
            }
        }

        void ServerGameProxy::raiseWater() {
            RaiseWaterLevel rwl;
            for (auto &peer : peers) {
                peer->sendReliable(rwl);
            }
        }

        void ServerGameProxy::eraseShot(Uint16 id) {
            EraseShot es;
            es.id = id;
            for (auto &peer : peers) {
                peer->sendReliable(es, Channel::SHOTS);
            }
        }

        void ServerGameProxy::spawnExplosion(Explosion &explosion) {
            SpawnExplosion se;
            se.centre = { explosion.centre.x, explosion.centre.y };
            se.max = explosion.max;
            se.now = explosion.now;
            se.color.red = explosion.color.getRed();
            se.color.green = explosion.color.getGreen();
            se.color.blue = explosion.color.getBlue();
            se.color.alpha = explosion.color.getAlpha();
            for (auto &peer : peers) {
                peer->sendReliable(se, Channel::SHOTS);
            }
        }

        void ServerGameProxy::broadcastMessage(Int32 playerId, const std::string &msg) {
            MessageBroadcast message;
            message.playerId = playerId;
            message.text = msg;

            for (auto &peer : peers) {
                peer->sendReliable(message, Channel::BROADCAST_MESSAGES);
            }
        }

        void ServerGameProxy::broadcastChatMessage(const std::string &msg, bool system, const std::string & origin){
            ChatMessage message;
            message.text = msg;
            message.system = system;
            message.origin = origin;
            for (auto &peer : peers) {
                peer->sendReliable(message, Channel::BROADCAST_MESSAGES);
            }
        }

        void ServerGameProxy::chat(bool value) {
            Chat c;
            c.value = value;
            for (auto &peer : peers) {
                peer->sendReliable(c, Channel::BROADCAST_MESSAGES);
            }
        }

        void ServerGameProxy::console(bool value) {
            net::Console c;
            c.value = value;
            for (auto &peer : peers) {
                peer->sendReliable(c, Channel::BROADCAST_MESSAGES);
            }
        }

        void ServerGameProxy::focus(bool value) {
            Focus f;
            f.value = value;
            for (auto &peer : peers) {
                peer->sendReliable(f, Channel::BROADCAST_MESSAGES);
            }
        }

        void ServerGameProxy::doubleJumpEffect(Int32 playerId, Float32 x, Float32 y, Float32 angle) {
            DoubleJumpEffects dje;
            dje.x = x;
            dje.y = y;
            dje.angle = angle;
            dje.playerId = playerId;
            for (auto &peer : peers) {
                peer->sendReliable(dje, Channel::PLAYERS);
            }
        }

        void ServerGameProxy::playSample(Int32 playerId, PlayerSounds::Type type) {
            PlaySample ps;
            ps.playerId = playerId;
            ps.sample = static_cast<SampleType>(static_cast<type_t>(type));
            for (auto &peer : peers) {
                peer->sendReliable(ps, Channel::SAMPLES);
            }
        }

        void loadNetWeapon(Weapon &w, const LyingWeapon &weapon) {
            Collider &c = w.collider;
            const auto & collider = weapon.getCollider();
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
        }
    } /* namespace net */
} /* namespace Duel6 */
