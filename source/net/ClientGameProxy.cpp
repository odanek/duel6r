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
                if((int) weapon.getId() == w.weaponId){
                    weapon.getCollider().getPosition().set(w.collider.position.x, w.collider.position.y);
                    break;
                }
            }
        }

        void ClientGameProxy::handle(MessageBroadcast &m) {
            auto pos = idmap[m.playerId];
            auto &player = game->players[pos];
            game->broadcastMessage(player, m.text, true);
        }

        void ClientGameProxy::handle(Peer &peer, ChatMessage &m) {
            if(game->isServer){
                m.origin = peer.getDescription();
            }
            game->broadcastChatMessage(m.text, true, m.system, m.origin);
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
                        player.unconfirmedInputs[(player.tick + i) & 127] = p.unconfirmedInputs[64 - missed + i];
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
                p.snapshotTick = gsu.inputTick;
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
                    if(p.changed[Player::FIELDS::NO_CHANGE] || p.changed.count() != Player::FIELDS::_SIZE - 1){ // -1 for _SIZE and NO_CHANGE
                        loadSnapshot = true;
                        missingSnapshot = true;
                    }
                    if (loadSnapshot && peer->snapshot[gsu.snapshotTick & xor_64].count(p.id) > 0) {
                        if (peer->snapshot[gsu.snapshotTick & xor_64][p.id].snapshotTick == gsu.snapshotTick) {
                            Player &confirmed = peer->snapshot[gsu.snapshotTick & xor_64][p.id];
                            Player::fillinFromPreviousConfirmed(confirmed, p);
                            missingSnapshot = false;
                        } else {
                            missingSnapshot = true;
                        }
                    }
                    if(missingSnapshot){
                        // correct snapshot not found,
                        // wait out the server to send full copy
                        peer->receivedInputsTick = peer->receivedInputsTick - 64; // cache is trash
                        continue; // skip this player to not screw things
                    } else {
                        uint16_t lctdelta = p.lastConfirmedTick - player.lastConfirmedTick;
                        if(lctdelta < 65000 || player.lastConfirmedTick == 0){ // player.lastConfirmedTick == 0 is at the start
                            player.lastConfirmedTick = p.lastConfirmedTick;
                        } else {
                            peer->receivedInputsTick = peer->receivedInputsTick - 64; // cache is trash
                            continue;
                        }
                    }

                    auto &position = p.position;
                    auto &externalForces = p.externalForces;
                    auto &externalForcesSpeed = p.externalForcesSpeed;
                    auto &velocity = p.velocity;
                    auto &acceleration = p.acceleration;

                    auto &collidingEntity =  player.confirmedCollider;

                    collidingEntity.position = { position.x, position.y, collidingEntity.position.z };
                    collidingEntity.externalForcesSpeed = { externalForcesSpeed.x, externalForcesSpeed.y, collidingEntity.externalForcesSpeed.z};
                    collidingEntity.externalForces = { externalForces.x, externalForces.y, collidingEntity.externalForces.z };
                    collidingEntity.velocity = { velocity.x, velocity.y, collidingEntity.velocity.z };
                    collidingEntity.acceleration = { acceleration.x, acceleration.y, collidingEntity.acceleration.z };
                    collidingEntity.lastCollisionCheck.onGround = p.lastCollisionCheck; //TODO all values
                    player.confirmedFlags = p.flags;
                    player.confirmedLife = p.life;
                    player.setAir(p.air);
                    player.setAmmo(p.ammo);
                    if(!player.local && p.timeToReload > 0.1f){ // prevent multiple-shot-like effect
                        player.setReloadTime(p.timeToReload);
                    }
                    player.rtt = p.rtt;
                    if (player.getWeapon().getId() != p.weaponId) {
                        player.setWeapon(p.weaponId); //resets animation
                    }
                    player.confirmedOrientation = p.orientationLeft ? Orientation::Left : Orientation::Right;
                    player.updateBonus(Duel6::BonusType::getById(static_cast<unsigned int>(p.bonusType)), p.bonusDuration, p.bonusRemainingTime);
                    player.setTimeSinceHit(p.timeSinceHit);
                    player.setBodyAlpha(p.bodyAlpha);
                    player.setAlpha(p.alpha);

                    p.unloadToPlayer(player);
                    p.score.unloadToPlayer(player);

                    peer->snapshot[gsu.inputTick & xor_64][p.id] = p;
                    if((gsu.confirmInputTick - player.lastConfirmedTick) > 0){
                        player.lastConfirmedTick = p.lastConfirmedTick; //
                    }
                }

                if (!player.local) {
                    player.setLife(player.confirmedLife);
                    player.setFlags(player.confirmedFlags);
                    const size_t maxMissed = Player::INPUTS;
                    uint16_t missed = (uint16_t) (player.lastConfirmedTick - player.tick);
                    if (missed > maxMissed) {
                        missed = maxMissed;
                    }
                    for (size_t i = 0; i < maxMissed; i++) {
                        player.unconfirmedInputs[(player.tick + i) & 127] = p.unconfirmedInputs[maxMissed - missed + i];
                    }
                }
            }

        }

        void ClientGameProxy::peerDisconnected(Peer &peer){
            std::vector<Int32> removedIds;
            removedIds.reserve(game->players.size());
            for(const auto & p : game->players){
                if(p.getClientId() == peer.getClientID()){
                    removedIds.push_back(p.getId());
                    // TODO DO Something with the list<Person> - rewrite to use vector ?
//                    auto personToDelete = std::find_if(persons.begin(), persons.end(), [&p](const Person & person){
//                        return &(p.getPerson()) == &person;
//                    });
//                    if(personToDelete != persons.end()){
//                        persons.erase(personToDelete);
//                    }
                }
            }
            game->broadcastChatMessage("Disconnected", false, true, peer.getDescription());
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
            }

            for (auto &p : sr.players) {
                Duel6::Player &player = game->players[idmap[p.id]];
                player.tick = game->tick;
                auto &position = p.position;

                CollidingEntity collidingEntity;
                collidingEntity.position = { position.x, position.y, collidingEntity.position.z };
                collidingEntity.externalForces = {0,0,0}; //{ externalForces.x, externalForces.y, collidingEntity.externalForces.z };
                collidingEntity.externalForcesSpeed = {0,0,0};//{ externalForcesSpeed.x, externalForcesSpeed.y, collidingEntity.externalForcesSpeed.z };
                collidingEntity.velocity = {0,0,0};//{ velocity.x, velocity.y, collidingEntity.velocity.z };
                collidingEntity.acceleration = {0,0,0};//{ acceleration.x, acceleration.y, collidingEntity.acceleration.z };
                player.getCollider() = collidingEntity;
                player.confirmedCollider = collidingEntity;
                player.setFlags(p.flags);
                player.confirmedFlags = p.flags;
                player.setLife(p.life);
                player.confirmedLife = p.life;
                player.setAir(p.air);
                player.setAmmo(p.ammo);
                player.setWeapon(p.weaponId);
                player.setOrientation(p.orientationLeft ? Orientation::Left : Orientation::Right);
                player.confirmedOrientation = player.getOrientation();
                player.setControllerState(p.controls);
                player.updateBonus(Duel6::BonusType::getById(static_cast<unsigned int>(p.bonusType)), p.bonusDuration, p.bonusRemainingTime);
                player.setBodyAlpha(p.bodyAlpha);
                player.setTimeSinceHit(p.timeSinceHit);
                player.setAlpha(p.alpha);
                player.setReloadTime(p.timeToReload);
                p.unloadToPlayer(player);
                p.score.unloadToPlayer(player);
                p.snapshotTick = sr.tick;
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
                // TODO DO Something with the list<Person> - rewrite to use vector ?
//                if(idmap.count(id) > 0){
//                    auto & p = game->players[idmap[id]];
//                    auto personToDelete = std::find_if(persons.begin(), persons.end(), [&p](const Person & person){
//                        return &(p.getPerson()) == &person;
//                    });
//                    if(personToDelete != persons.end()){
//                        persons.erase(personToDelete);
//                    }
//                }
                idmap.erase(id);
            }

            game->disconnectPlayers(pd.disconnectedId);
        }

        void ClientGameProxy::handle(PlayersJoined &pj) {
            joinPlayers(pj.playerProfiles);
        }

        void ClientGameProxy::handle(NextRound &nr) {
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
            }
        }

        //this should only happen on the server side when the client called us with this request
        void ClientGameProxy::handle(Peer &peer, RequestGameState &r) {
            peer.peerUpdateState = PeerUpdateState::REQUESTED_GAMESTATE;
            for (auto &p : r.connectingPlayers) {
                p.clientId = peer.getClientID();
            }
            if (r.connectingPlayers.size() > 0) {
                std::string description = r.connectingPlayers[0].name;
                if (r.connectingPlayers.size() > 1) {
                    std::stringstream ss;
                    ss << " (+" << r.connectingPlayers.size() - 1 << ")";
                    description += ss.str();
                }
                peer.setDescription(description);
            } else {
                peer.setDescription(hostToIPaddress(peer.getEnetPeer().address.host, peer.getEnetPeer().address.port));
            }
            std::stringstream ss;
            ss << peer.getDescription() << " is connecting from " << hostToIPaddress(peer.getEnetPeer().address.host, peer.getEnetPeer().address.port);
            game->broadcastChatMessage(ss.str(), false, true, "SERVER");
            joinPlayers(r.connectingPlayers);
            sendGameState(peer, *game);
        }

        void ClientGameProxy::handle(ObjectBase &o) {
            handleObject(o);
        }

        void ClientGameProxy::handle(EventBase &e) {
            handleEvent(e);
        }

        void ClientGameProxy::handle(Peer & peer, ObjectBase &o) {
            handleObject(peer, o);
        }

        void ClientGameProxy::handle(Peer & peer, EventBase &e) {
            handleEvent(peer, e);
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

        void ClientGameProxy::handle(DoubleJumpEffects &dje) {
            if (idmap.count(dje.playerId) == 0) {
                return;
            }
            auto pos = idmap[dje.playerId];
            auto &player = game->players[pos];
            game->onDoubleJumpEffect(player, dje.x, dje.y, dje.angle);
        }

        void ClientGameProxy::handle(RaiseWaterLevel &rwl) {
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
            game->setPlayedRounds(sr.round);
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
            for (auto &player : game->players) { // fix players orientation
               player.setOrientation(player.confirmedOrientation);
            }
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
            player.getIndicators().getReload().show(player.getReloadInterval() + Indicator::FADE_DURATION);
            player.getIndicators().getBullets().show();
            if(player.local){
                game->spawnShot(std::move(x));
            } else {
                game->spawnRemoteShot(std::move(x));
            }

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

        void ClientGameProxy::handle(RequestNextRound &rnr){
            if(game->getCurrentRound() != rnr.currentRound){
                return;
            }
            game->onNextRound();
        }

        void ClientGameProxy::handle(EraseShot &es){
            game->eraseShot(es.id);
        }

        void ClientGameProxy::nextRound() {
            game->onNextRound();
        }

        void ClientGameProxy::handle(Peer &peer, Chat &c) {
            if (game->isServer) {
                for (auto &player : game->players) {
                    if (player.getClientId() == peer.getClientID()) {
                        player.setChatting(c.value);
                    }
                }
            }
        }

        void ClientGameProxy::handle(Peer &peer, Console &c){
            if (game->isServer) {
                for (auto &player : game->players) {
                    if (player.getClientId() == peer.getClientID()) {
                        player.setInConsole(c.value);
                    }
                }
            }
        }

        void ClientGameProxy::handle(Peer &peer, Focus &f) {
            if (game->isServer) {
                for (auto &player : game->players) {
                    if (player.getClientId() == peer.getClientID()) {
                        player.setFocused(f.value);
                    }
                }
            }
        }
    } /* namespace net */
} /* namespace Duel6 */
