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

    namespace net {

        class ServerGameProxy: public GameProxy {
        private:
            std::list<Peer *> peers;
        public:
            void playersJoined(std::vector<PlayerDefinition> & playerDefinitions);
            void playersDisconnected(const std::vector<Int32> & ids);
            void playersConnected(const std::vector<Int32> & ids);
            void add(Peer *);
            void remove(Peer *);
            void startRound(Duel6::Level & level) override;
//            void sendGameState(Game & game) override;
            void sendGameStateUpdate(Game & game) override;
            void nextRound() override;
            void handle(ObjectBase &o) override;
            void handle(EventBase &e) override;
//            void gameState(Game & game);
            ServerGameProxy();
            virtual ~ServerGameProxy();
        private:
            void sendInputs(Game &game);
        };

    } /* namespace net */
} /* namespace Duel6 */

#endif /* SOURCE_NET_SERVERGAMEPROXY_H_ */
