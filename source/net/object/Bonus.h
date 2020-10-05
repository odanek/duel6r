/*
 * Bonus.h
 *
 *  Created on: May 10, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_BONUS_H_
#define SOURCE_NET_OBJECT_BONUS_H_


#include "../Object.h"
#include "../Type.h"
#include "../../Type.h"
#include "../object/Vector2d.h"
namespace Duel6::net {
    enum class BonusType : type_t {
        NONE,
        PLUS_LIFE,
        MINUS_LIFE,
        FULL_LIFE,
        FAST_RELOAD,
        POWERFUL_SHOTS,
        INVULNERABILITY,
        BULLETS,
        FAST_MOVEMENT,
        INVISIBILITY,
        SPLIT_FIRE,
        VAMPIRE_SHOTS,
        INFINITE_AMMO,
        SNORKEL,
        MAX_COUNT
    };

    template<typename Stream>
    bool serialize(Stream &s, BonusType &o) {
        if (s.isDeserializer()) {
            type_t r;
            if (!s.safe_max(r, (type_t) (static_cast<type_t>(BonusType::MAX_COUNT) - 1))) {
                return false;
            }
            o = static_cast<BonusType>(r);
            return true;
        } else {
            type_t r = static_cast<type_t>(o);
            return s & r;
        }
    }

    class Bonus: public Object<Bonus, ObjectType::BONUS> {
    public:
        BonusType type;
        Int32 duration;
        Vector2D position;
        Int32 textureIndex;
        Uint32 bonusId;

        template<typename Stream>
        bool serialize(Stream &s) {
            return s & type
                && s & duration
                && s & position
                && s & textureIndex
                && s & bonusId;
        }
    };
}



#endif /* SOURCE_NET_OBJECT_BONUS_H_ */
