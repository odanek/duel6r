#ifndef SOURCE_NET_OBJECT_PLAYER_INPUTS_H_
#define SOURCE_NET_OBJECT_PLAYER_INPUTS_H_
#include <bitset>
#include "../Object.h"
#include "../../Type.h"
namespace Duel6::net {
    class PlayerInputs: public Object<PlayerInputs, ObjectType::PLAYER_INPUTS> {
    public:
        object_id_t id = 0;
        Uint8 controls;
        std::array<Uint8, 64> unconfirmedInputs;

        template<class Stream>
        bool serialize(Stream &s) {
            bool result = s & id;
            if (s.isDeserializer()) {
                result &= s & controls;
                if (controls != 0xff) {
                    for (Uint8 &val : unconfirmedInputs) {
                        val = controls;
                    }
                } else {
                    result &= s & unconfirmedInputs;
                }
            } else {
                bool same = true;
                controls = unconfirmedInputs.back();
                for (Uint8 &val : unconfirmedInputs) {
                    if (val != controls) {
                        controls = 0xff;
                        same = false;
                        break;
                    }
                }
                result &= s & controls;
                if (!same) {
                    result &= s & unconfirmedInputs;
                }
            }
            return result;
        }

    };
}

#endif /* SOURCE_NET_OBJECT_PLAYER_H_ */
