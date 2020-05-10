/*
 * MessageBase.h
 *
 *  Created on: Mar 16, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_MESSAGE_MESSAGEBASE_H_
#define SOURCE_NET_MESSAGE_MESSAGEBASE_H_
#include "Type.h"
#include "Message.h"
namespace Duel6 {
    namespace net {
        enum class EventType : type_t {
            REQUEST_GAME_STATE,
            GAME_STATE,
            GAME_STATE_UPDATE,
            PLAYER_INPUTS_UPDATE,
            NEXT_ROUND,
            PLAYERS_DISCONNECTED,
            PLAYERS_JOINED,
            SPAWN_BONUS,
            SPAWN_WEAPON,
            PICK_BONUS,
            PICK_WEAPON,

            CLIENT_JOINED,
            CLIENT_DISCONNECTED,
            CLIENT_DISCONNECT,

            CLIENT_PLAYERS,

            GAME_START,
            GAME_END,

            ROUND_END,
            ROUND_START,
            ROUND_SET,

            SHOT_UPDATE,
            SHOT_ADD,
            SHOT_HIT,
            PLAYER_HIT,
            PLAYER_SPAWN,
            PLAYER_DIED,
            PLAYER_KILLED,

            MAX_COUNT
        };

        template<typename Stream>
        bool serialize(Stream & s, EventType & o){
            if(s.isDeserializer()){
                type_t r;
                if(!s.safe_max(r, (type_t) (static_cast<type_t>(EventType::MAX_COUNT) - 1))){
                    return false;
                }
                o = static_cast<EventType>(r);
                return true;
            } else {
                type_t r = static_cast<type_t>(o);
                return s & r;
            }
        }

        class EventBase: public Message<EventBase, MessageType::EVENT> {
        protected:
            EventType eventType;

        public:
            EventBase(EventType eventType)
                : eventType(eventType) {
            }

            virtual ~EventBase() {
            }
            bool sendMessage(binarystream &bs) override {
                return bs << eventType
                    && onSendEvent(bs);
            }
        protected:
            virtual bool onSendEvent(binarystream &bs) = 0;
        };

        template<typename EventImpl, EventType type>
        class Event: public EventBase {
        public:
            Event()
                : EventBase(type) {
            }

        private:
            virtual bool onSendEvent(binarystream &bs) override {
                return bs << *static_cast<EventImpl*>(this);
            }
        };
    } /* namespace net */
} /* namespace Duel6 */

#endif /* SOURCE_NET_MESSAGE_MESSAGEBASE_H_ */
