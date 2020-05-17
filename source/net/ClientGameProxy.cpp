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
        const uint16_t xor_32768 = 32767;
        const uint16_t xor_32 = 31;
        const uint16_t xor_64 = 63;
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
        //periodical update of lying weapons
        void ClientGameProxy::handle(Weapon &w) {
            for(auto & weapon: game->getRound().getWorld().getBonusList().getLyingWeapons()){
                if(weapon.getId() == w.weaponId){
                    weapon.getCollider().getPosition().set(w.collider.position.x, w.collider.position.y);
                    break;
                }
            }
        }
        void ClientGameProxy::lateReceive(tick_t lateTick) {
            for (auto &player : game->players) {
                player.lateTicks++;
            }
        }

        void ClientGameProxy::handle(PlayerInputsUpdate &piu) {

            if(!game->isServer){
                return;
            }
            switch (peer->peerUpdateState) {
            case PeerUpdateState::WAITING_FOR_REQUEST: {
                return;
                break;
            }
            case PeerUpdateState::REQUESTED_GAMESTATE: {
                if (game->isServer) {
                    peer->peerUpdateState = PeerUpdateState::GAMESTATE_RECEIVED;
                } else {
                    return;
                }
                break;
            }
            case PeerUpdateState::GAMESTATE_RECEIVED: {
                peer->peerUpdateState = PeerUpdateState::RUNNING;
                break;
            }
            case PeerUpdateState::RUNNING: {
                break;
            }
            }

            for (auto &p : piu.playersInputs) {
                if (idmap.count(p.id) == 0) {
                    std::cerr << "Player id " << p.id << " not found, skipping\n";
                    continue;
                }
                auto pos = idmap[p.id];
                auto &player = game->players[pos];

                if (!player.local) {
                    player.lastConfirmedTick = piu.confirmInputTick;
                    uint16_t missed = (uint16_t) (player.lastConfirmedTick - player.tick);
                    if (missed > 64) {
                        missed = 64;
                        for(auto & val : player.unconfirmedInputs){
                            val = 0;
                        }
                    }
                    for (size_t i = 0; i < missed; i++) {
                        player.unconfirmedInputs[(player.tick + i) % 128] = p.unconfirmedInputs[64 - missed + i];
                    }
                    player.setControllerState(p.unconfirmedInputs[63]);
                }
            }
        }

        void ClientGameProxy::handle(GameStateUpdate &gsu) {
            switch (peer->peerUpdateState) {
            case PeerUpdateState::WAITING_FOR_REQUEST: {
                return;
                break;
            }
            case PeerUpdateState::REQUESTED_GAMESTATE: {
                if(game->isServer){
                    peer->peerUpdateState = PeerUpdateState::GAMESTATE_RECEIVED;
                } else {
                    return;
                }
                break;
            }
            case PeerUpdateState::GAMESTATE_RECEIVED: {
                peer->peerUpdateState = PeerUpdateState::RUNNING;
                break;
            }
            case PeerUpdateState::RUNNING: {
                break;
            }
            }
            bool missingSnapshot = false;
            for (auto &p : gsu.players) {
                if (idmap.count(p.id) == 0) {
                    std::cerr << "Player id " << p.id << " not found, skipping\n";
                    continue;
                }
                auto pos = idmap[p.id];
                auto &player = game->players[pos];

                if (!game->isServer) {
                    game->getRound().setWinner(gsu.hasWinner);
                    missingSnapshot = false;
                    bool loadSnapshot = false;
                    if(p.changed[0] || p.changed.count() != Player::FIELDS::_SIZE - 1){ // -1 for _SIZE and NO_CHANGE
                        loadSnapshot = true;
                    }
                    if (loadSnapshot && peer->snapshot[gsu.snapshotTick & xor_64].count(p.id) > 0) {
                        if (peer->snapshot[gsu.snapshotTick & xor_64][p.id].debug == gsu.snapshotTick) {
                            Player &confirmed = peer->snapshot[gsu.snapshotTick & xor_64][p.id];
                            Player::fillinFromPreviousConfirmed(confirmed, p);
                        } else {
                            missingSnapshot = true;
                        }
                    }
                    if(missingSnapshot){
                        // correct snapshot not found,
                        // wait out the server to send full copy
                        peer->confirmedInputsTick = player.lastConfirmedTick;
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
                         collidingEntity.externalForcesSpeed.x += diff.x / 2;
                         collidingEntity.externalForcesSpeed.y += diff.y / 2;
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
                    player.updateBonus(Duel6::BonusType::getById(static_cast<unsigned int>(p.bonusType)), p.bonusDuration, p.bonusRemainingTime);
                    player.setTimeSinceHit(p.timeSinceHit);
                    player.setBodyAlpha(p.bodyAlpha);
                    player.setAlpha(p.alpha);
                    //TODO handle 16bit wrap-around
//                    if (player.local && ((game->tick - gsu.confirmInputTick) & xor_32768 < 32)) { // cap it at 8 frames to avoid run-away of death
//                        Uint32 ms = 1000 / 90;
//                        Uint32 drift = player.rtt / ms;
//                        game->compensateLag(gsu.confirmInputTick);//game->tick - (game->tick -  gsu.confirmInputTick) / 2 ); // run client-side prediction
//                    }
                    peer->snapshot[gsu.inputTick & xor_64][p.id] = p;
                } else {
                    player.lastConfirmedTick = gsu.confirmInputTick;
                    player.rtt = peer->getRTT();
                }
                if (!player.local) {
                    const size_t maxMissed = Player::INPUTS;
                    uint16_t missed = (uint16_t) (player.lastConfirmedTick - player.tick);
                    if (missed > maxMissed) {
                        missed = maxMissed;
                    }
                    for (size_t i = 0; i < maxMissed; i++) {
                        player.unconfirmedInputs[(player.tick + i) % 128] = p.unconfirmedInputs[maxMissed - missed + i];
                    }
                }
            }

            if (!game->isServer) {
                if ((game->tick - gsu.confirmInputTick) & xor_32 < 16) {
                    Uint32 ms = 1000 / 90;
                    Uint32 drift = peer->getRTT() / ms;
                    game->compensateLag(gsu.confirmInputTick);
//                    game->compensateLag(game->tick - (game->tick - gsu.confirmInputTick) / 2);
                }
            }
        }
        void ClientGameProxy::peerDisconnected(Peer &peer){
            std::vector<Int32> removedIds;
            removedIds.reserve(game->players.size());
            for(const auto & p : game->players){
                if(p.getClientId() == peer.getClientID()){
                    removedIds.push_back(p.getId());
                }
            }
            game->disconnectPlayers(removedIds);

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


            for (Bonus &b : sr.bonuses) {
                Vector position = { b.position.x, b.position.y, 0.5f };
                game->spawnBonus(Duel6::Bonus(b.bonusId,
                    static_cast<unsigned int>(b.type),
                    b.duration,
                    position,
                    b.textureIndex));
            }
            for (Weapon &w : sr.weapons) {
                Collider &c = w.collider;
                CollidingEntity collider;
                collider.position.set(c.position.x, c.position.y);
                collider.acceleration.set(c.acceleration.x, c.acceleration.y);
                collider.externalForces.set(c.externalForces.x, c.externalForces.y);
                collider.externalForcesSpeed.set(c.externalForcesSpeed.x, c.externalForcesSpeed.y);
                collider.velocity.set(c.velocity.x, c.velocity.y);

                //Vector position = { w.position.x, w.position.y, 0.5f };
                game->spawnWeapon(Duel6::LyingWeapon(
                    static_cast<unsigned int>(w.type),
                    w.weaponId,
                    w.bullets,
                    w.remainingReloadTime,
                    collider));
            }
            std::vector<PlayerDefinition> playerDefinitions;
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
                    Duel6::Color color { c.red, c.green, c.blue, c.alpha };
                    colors.set((PlayerSkinColors::BodyPart) cid++, color);
                }
                playerDefinitions.emplace_back(
                    person,
                    colors,
                    defaultSounds,
                    PlayerControlsManager::getNoop(),
                    p.team,
                    p.playerId,
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
                player.setFlags(p.flags);
                player.setLife(p.life);
                player.setAir(p.air);
                player.setAmmo(p.ammo);
                player.setWeapon(p.weaponId);
                player.setOrientation(p.orientationLeft ? Orientation::Left : Orientation::Right);
                player.setControllerState(p.controls);
                player.updateBonus(Duel6::BonusType::getById(static_cast<unsigned int>(p.bonusType)), p.bonusDuration, p.bonusRemainingTime);
                player.setBodyAlpha(p.bodyAlpha);
                player.setTimeSinceHit(p.timeSinceHit);
                player.setAlpha(p.alpha);
                peer->snapshot[sr.tick & xor_64][p.id] = p;
            }
            peer->peerUpdateState = PeerUpdateState::GAMESTATE_RECEIVED;

        }
        void ClientGameProxy::netStopped() {
            if(game == &Context::getCurrent()){
                //go back to menu
                game->appService.getConsole().printLine("Connection network stopped. (server disconnected ?)");
                Context::pop();
            }

        }
        void ClientGameProxy::handle(PlayersDisconnected &pd) {
            for(auto id: pd.disconnectedId){
                idmap.erase(id);
            }

            game->disconnectPlayers(pd.disconnectedId);
        }
        void ClientGameProxy::handle(PlayersJoined &pj) {
            joinPlayers(pj.playerProfiles);
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

        void ClientGameProxy::joinPlayers(std::vector<PlayerProfile> &playerProfiles){
            std::vector<PlayerDefinition> playerDefinitions;
            playerDefinitions.reserve(playerProfiles.size());

            for (auto &p : playerProfiles) {
                Person &person = persons.emplace_back(p.name, nullptr);
                PlayerSkinColors colors;
                colors.setHair(p.skin.hair);
                colors.setHeadBand(p.skin.headBand);
                int cid = 0;
                for (auto c : p.skin.colors) {
                    if (cid > 10) {
                        return;
                    }
                    Duel6::Color color { c.red, c.green, c.blue, c.alpha };
                    colors.set((PlayerSkinColors::BodyPart) cid++, color);
                }
                playerDefinitions.emplace_back(
                    person,
                    colors,
                    defaultSounds,
                    PlayerControlsManager::getNoop(),
                    p.team,
                    p.playerId,
                    p.clientId,
                    p.clientLocalId);
            }
            game->joinPlayers(playerDefinitions);
            for (auto &d : playerDefinitions) {
                idmap[d.getPlayerId()] = d.playerPos;
                idmapBack[d.playerPos] = d.getPlayerId();
            }
        }

        //this should only happen on the server side when the client called us with this request
        void ClientGameProxy::handle(Peer &peer, RequestGameState &r) {
            peer.peerUpdateState = PeerUpdateState::REQUESTED_GAMESTATE;
            for (auto &p : r.connectingPlayers) {
                p.clientId = peer.getClientID();
            }

            joinPlayers(r.connectingPlayers);
            sendGameState(peer, *game);
        }

        void ClientGameProxy::handle(ObjectBase &o) {
            handleObject(o);
        }
        void ClientGameProxy::handle(EventBase &e) {
            handleEvent(e);
        }

        bool ClientGameProxy::gameIsServer(){
            return game->isServer;
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
        void ClientGameProxy::handle(RaiseWaterLevel &rwl){
            game->raiseWater();
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
        void ClientGameProxy::handle(SpawnBonus &sb) {
            Bonus &b = sb.bonus;
            Vector position = {b.position.x, b.position.y, 0.5f};
            game->spawnBonus(Duel6::Bonus(b.bonusId,
                static_cast<unsigned int>(b.type),
                b.duration,
                position,
                b.textureIndex));
        }
        void ClientGameProxy::handle(SpawnShot &ss) {
             const Duel6::Weapon & weapon = Duel6::Weapon::getById(ss.weaponId);
             if (idmap.count(ss.playerId) == 0) {
                 std::cerr << "Player id " << ss.playerId << " not found, skipping spawned shot\n";
                 return;
             }
            auto pos = idmap[ss.playerId];
            auto &player = game->players[pos];
            Uint32 id = ss.shot.id;
            Duel6::Vector position(ss.shot.position.x, ss.shot.position.y);
            Duel6::Vector velocity(ss.velocity.x, ss.velocity.y);
            std::unique_ptr<Duel6::Shot> x = weapon.shoot(
                player,
                ss.orientationLeft ? Orientation::Left : Orientation::Right,
                game->getRound().getWorld(),
                id,
                ss.powerful,
                ss.power,
                ss.bulletSpeed,
                position,
                velocity
           );
            game->spawnShot(std::move(x));

         }
        void ClientGameProxy::handle(PlaySample &ps) {
            if (idmap.count(ps.playerId) == 0) {
                std::cerr << "Player id " << ps.playerId << " not found, skipping\n";
                return;
            }
            auto pos = idmap[ps.playerId];
            auto &player = game->players[pos];
            game->playSample(player, static_cast<PlayerSounds::Type>(ps.sample));
        }
        void ClientGameProxy::handle(SpawnExplosion &ss) {
            Duel6::Explosion explosion;
            explosion.centre = { ss.centre.x, ss.centre.y, 0.0f };
            explosion.now = ss.now;
            explosion.max = ss.max;
            explosion.color.set(ss.color.red, ss.color.green, ss.color.blue, ss.color.alpha);
            game->spawnExplosion(std::move(explosion));
        }

        void ClientGameProxy::handle(SpawnWeapon &sw) {
            Weapon &w = sw.weapon;
            Collider &c = w.collider;
            CollidingEntity collider;
            collider.position.set(c.position.x, c.position.y);
            collider.acceleration.set(c.acceleration.x, c.acceleration.y);
            collider.externalForces.set(c.externalForces.x, c.externalForces.y);
            collider.externalForcesSpeed.set(c.externalForcesSpeed.x, c.externalForcesSpeed.y);
            collider.velocity.set(c.velocity.x, c.velocity.y);

            game->spawnWeapon(Duel6::LyingWeapon(
                static_cast<unsigned int>(w.type),
                w.weaponId,
                w.bullets,
                w.remainingReloadTime,
                collider));
        }
        void ClientGameProxy::handle(PickBonus &pb){
            for(auto & player : game->players){
                if(player.getId() == pb.playerId){
                    game->pickBonus(player, pb.bonusId);
                }
            }
        }
        void ClientGameProxy::handle(PickWeapon &pw){
            for(auto & player : game->players){
                if(player.getId() == pw.playerId){
                    game->pickWeapon(player, pw.weaponId);
                }
            }
        }

        void ClientGameProxy::handle(EraseShot &es){
            game->eraseShot(es.id);
        }

        void ClientGameProxy::nextRound() {
            game->onNextRound();
        }
    } /* namespace net */
} /* namespace Duel6 */
