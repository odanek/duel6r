/*
 * Object.h
 *
 *  Created on: Mar 17, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_H_
#define SOURCE_NET_OBJECT_H_
#include "Type.h"
#include "Message.h"
namespace Duel6::net {
    enum class ObjectType : type_t {
        PLAYER,
        PLAYER_INPUTS,
        SHOT,
        CLIENT,
        LEVEL,
        WORLD,
        MAX_COUNT
    };

    template<typename Stream>
    bool serialize(Stream & s, ObjectType & o){
        if(s.isDeserializer()){
            type_t r;
            if(!s.safe_max(r, (type_t) (static_cast<type_t>(ObjectType::MAX_COUNT) - 1))){
                return false;
            }
            o = static_cast<ObjectType>(r);
            return true;
        } else {
            type_t r = static_cast<type_t>(o);
            return s & r;
        }
    }

    class ObjectBase: public Message<ObjectBase, MessageType::OBJECT> {
    private:
        ObjectType objectType;

    public:
        ObjectBase(ObjectType objectType)
            : objectType(objectType) {
        }
        virtual ~ObjectBase() {
        }

        bool sendMessage(binarystream &bs) override {
            return bs << objectType
                && onSendObject(bs);
        }
    protected:
        virtual bool onSendObject(binarystream &bs) = 0;
    };

    template<typename ObjectImpl, ObjectType type>
    class Object: public ObjectBase {

    public:
        Object()
            : ObjectBase(type) {
        }
    private:
        virtual bool onSendObject(binarystream &bs) override {
            return bs << *static_cast<ObjectImpl*>(this);
        }
    };
}

#endif /* SOURCE_NET_OBJECT_H_ */
