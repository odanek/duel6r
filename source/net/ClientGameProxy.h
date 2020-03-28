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
#include "../Person.h"
namespace Duel6 {
    class Game;
    class Player;
    namespace net {

        class ClientGameProxy: public GameProxy {
            Game *game;
            std::list<Person> persons;

        public:
            ClientGameProxy();
            virtual ~ClientGameProxy();

            void setGameReference(Game &g) {
                game = &g;
            }

            void handle(Player &p);
            void handle(GameState &s);
            void handle(NextRound &nr);

            template<typename ObjectTypeName>
            void handleObject(ObjectTypeName &o) {

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

        };

    } /* namespace net */
} /* namespace Duel6 */

#endif /* SOURCE_NET_CLIENTGAMEPROXY_H_ */
