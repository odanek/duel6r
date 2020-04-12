/*
 * GameProxy.h
 *
 *  Created on: Mar 21, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_GAMEPROXY_H_
#define SOURCE_NET_GAMEPROXY_H_
#include "object/objects.h"
#include "event/events.h"
#include "Peer.h"

namespace Duel6 {
    class Level;
    class Game;
    namespace net {

        template <typename To, typename From>
        To container_cast(From && from) {
            using std::begin; using std::end;
            return To(begin(from), end(from));
        }
        class Peer;

        class GameProxy {
        private:

            bool host = false; // todo remove asi

        public:
            virtual void sendGameStateUpdate(Game & game){
                ;
            }
            virtual void sendGameState(Game & game){
                ;
            }
            virtual void nextRound(){
                ;
            }
            virtual void startRound(Duel6::Level & level) = 0;
            virtual void handle(Peer & peer, RequestGameState & r);
            virtual void handle(ObjectBase &o) = 0;
            virtual void handle(EventBase &e) = 0;
            GameProxy();
            virtual ~GameProxy();

        protected:
            void sendGameState(std::list<Peer *> & peers, Game & game);
            void sendGameState(std::list<Peer *> && peers, Game & game);
            void sendGameState(Peer & peer, Game & game);
        };

    } /* namespace net */
} /* namespace Duel6 */

#endif /* SOURCE_NET_GAMEPROXY_H_ */
