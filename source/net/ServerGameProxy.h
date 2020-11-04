/*
 * ServerGameProxy.h
 *
 *  Created on: Mar 21, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_SERVERGAMEPROXY_H_
#define SOURCE_NET_SERVERGAMEPROXY_H_
#include <vector>
#include <memory>
#include "../PlayerDefinition.h"
#include "Peer.h"
#include "GameProxy.h"
#include "event/GameState.h"
namespace Duel6 {
    class Bonus;
    class LyingWeapon;
    class Player;
    class Shot;
    class Explosion;
    namespace net {

        class ServerGameProxy: public GameProxy {
        private:
            std::list<Peer *> peers;
            Int32 lastSyncedWeapon = -1; // for eventual periodical lying weapon sync
        public:
            void spawnBonus(Duel6::Bonus &bonus);
            void spawnWeapon(Duel6::LyingWeapon &lyingWeapon);
            void spawnShot(std::unique_ptr<Duel6::Shot> &shot);

            void pickWeapon(Duel6::Player &player, unsigned int weaponId);
            void pickBonus(Duel6::Player &player, unsigned int bonusId);


            void playersJoined(std::vector<PlayerDefinition> & playerDefinitions);
            void playersDisconnected(const std::vector<Int32> & ids);
            void playersConnected(const std::vector<Int32> & ids);
            void add(Peer *);
            void remove(Peer *);
            void gameEnded();

            void raiseWater();
            void eraseShot(Uint16 id);
            void spawnExplosion(Explosion &explosion);
            void broadcastMessage(Int32 playerId, const std::string & msg);
            void broadcastChatMessage(const std::string &msg, bool system, const std::string & origin);
            void chat(bool value);
            void console(bool value);
            void focus(bool value);
            void doubleJumpEffect(Int32 playerId, Float32 x, Float32 y, Float32 angle);
            void playSample(Int32 playerId, PlayerSounds::Type type);
            void startRound(Int32 playedRounds, Duel6::Level & level) override;
            void sendGameStateUpdate(Game & game) override;
            void nextRound() override;
            void handle(ObjectBase &o) override;
            void handle(EventBase &e) override;
            void handle(Peer &peer, ObjectBase &o) override;
            void handle(Peer &peer, EventBase &e) override;
            void requestNextRound(Int32 currentRound);

            ServerGameProxy();
            virtual ~ServerGameProxy();
        private:
            void sendInputs(Game &game);
        };

    } /* namespace net */
} /* namespace Duel6 */

#endif /* SOURCE_NET_SERVERGAMEPROXY_H_ */
