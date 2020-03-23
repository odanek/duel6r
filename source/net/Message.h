/*
 * NetObject.h
 *
 *  Created on: Mar 14, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_MESSAGE_H_
#define SOURCE_NET_MESSAGE_H_
#include <vector>
#include <functional>
#include "Type.h"
#include "binarystream/BinaryStream.h"
namespace Duel6 {
    namespace net {

        enum class MessageType : type_t {
            OBJECT,
            EVENT,
            MAX_COUNT
        };

        template<typename Stream>
        bool serialize(Stream & s, MessageType & o){
            if(s.isDeserializer()){
                type_t r;
                if(!s.safe_max(r, (type_t) (static_cast<type_t>(MessageType::MAX_COUNT) - 1))){
                    return false;
                }
                o = static_cast<MessageType>(r);
                return true;
            } else {
                type_t r = static_cast<type_t>(o);
                return s & r;
            }
        }
        class MessageBase {
        public:
            bool send(binarystream &bs) {
                return (bs << messageType)
                    && onSendMessage(bs);
            }
            virtual ~MessageBase() {
            }
        protected:
            MessageType messageType;
            public:
            MessageBase(MessageType messageType)
                : messageType(messageType) {
            }

            MessageType getType() {
                return messageType;
            }
            virtual bool onSendMessage(binarystream &bs) = 0;
        };

        template<typename MessageImpl, MessageType type>
        class Message: public MessageBase {
        protected:
            virtual bool sendMessage(binarystream &bs) = 0;

        public:
            Message()
                : MessageBase(type) {
            }

            virtual ~Message() {

            }
        private:
            //            template<class Stream>
//            bool send(Stream &s) {
//                return s << *static_cast<MessageImpl*>(this);
//            }

            virtual bool onSendMessage(binarystream &bs) override {
                return sendMessage(bs);
            }
        };

    } /* namespace net */
} /* namespace Duel6 */

#endif /* SOURCE_NET_MESSAGE_H_ */
