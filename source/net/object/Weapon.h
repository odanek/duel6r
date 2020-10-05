/*
 * Bonus.h
 *
 *  Created on: May 10, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_WEAPON_H_
#define SOURCE_NET_OBJECT_WEAPON_H_


#include "../Event.h"
#include "../Type.h"
#include "../../Type.h"
#include "../Object.h"
#include "../object/Vector2d.h"
#include "../object/Collider.h"

namespace Duel6::net {
    enum class WeaponType : type_t {
        PISTOL,
        BAZOOKA,
        LIGHTNING,
        SHOTGUN,
        PLASMA,
        LASER,
        MACHINE_GUN,
        TRITON,
        UZI,
        BOW,
        SLIME,
        DOUBLE_LASER,
        KISS_OF_DEATH,
        SPRAY,
        SLING,
        STOPPER_GUN,
        SHIT_THROWER,
        MAX_COUNT
    };

    template<typename Stream>
    bool serialize(Stream &s, WeaponType &o) {
        if (s.isDeserializer()) {
            type_t r;
            if (!s.safe_max(r, (type_t) (static_cast<type_t>(WeaponType::MAX_COUNT) - 1))) {
                return false;
            }
            o = static_cast<WeaponType>(r);
            return true;
        } else {
            type_t r = static_cast<type_t>(o);
            return s & r;
        }
    }

    class Weapon: public Object<Weapon, ObjectType::WEAPON> {
    public:
        WeaponType type;
        Int32 bullets;
        Collider collider;
        Int32 weaponId;
        Float32 pickTimeout;
        Float32 remainingReloadTime;

        template<typename Stream>
        bool serialize(Stream &s) {
            return s & type
                && s & bullets
                && s & collider
                && s & weaponId
                && s & pickTimeout
                && s & remainingReloadTime;
        }
    };
}



#endif /* SOURCE_NET_OBJECT_BONUS_H_ */
