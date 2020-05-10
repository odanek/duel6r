/*
 * ClientGameProxy.h
 *
 *  Created on: Mar 21, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_CLIENTGAMEPROXY_H_
#define SOURCE_NET_CLIENTGAMEPROXY_H_
#include <list>
#include "GameProxy.h"
#include "Peer.h"
#include "../Person.h"
#include "../PlayerSounds.h"
namespace Duel6 {
    class Game;
    class Player;
    namespace net {

        class ClientGameProxy: public GameProxy {
            Peer *peer;
            Game *game;
            std::list<Person> persons;
            std::map<Int32, Int32> idmap; //player mapping
            std::map<Int32, Int32> idmapBack; //player mapping //todo not used, remove
            PlayerSounds defaultSounds;
            Int32 clientId = 0; // will be set upon receiving gamestate from server
        public:
            void peerDisconnected(Peer &peer);
            bool gameIsServer();
            void lateReceive(tick_t lateTick);
            void netStopped();
            tick_t getTick() const;
            ClientGameProxy() = default;
            virtual ~ClientGameProxy() = default;

            void setGameReference(Game &g);
            void setPeerReference(Peer &peer);

            void handle(Player &p);
            void handle(GameState &s);
            void handle(GameStateUpdate &s);
            void handle(PlayerInputsUpdate &piu);
            void handle(NextRound &nr);
            void handle(PlayersDisconnected &pd);
            void handle(PlayersJoined &pj);
            void handle(StartRound &sr);
            void handle(SpawnBonus &sb);
            void handle(SpawnWeapon &sb);

            template<typename ObjectTypeName>
            void handleObject(ObjectTypeName &o) { //todo I think these are superfluous and can be removed

            }
            template<typename EventTypeName>
            void handleEvent(EventTypeName &e) {

            }

            void handle(Peer &peer, RequestGameState &r) override;
            void handle(ObjectBase &o) override;
            void handle(EventBase &e) override;

            RequestGameState getRequestGameState();
        protected:
            void nextRound() override;
            void startRound(Duel6::Level & level) override {};
        private:
            void joinPlayers(std::vector<PlayerProfile> &playerProfiles);
        };

    } /* namespace net */
} /* namespace Duel6 */

#endif /* SOURCE_NET_CLIENTGAMEPROXY_H_ */
