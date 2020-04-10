/*
 * Player.h
 *
 *  Created on: Mar 15, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_PLAYER_H_
#define SOURCE_NET_OBJECT_PLAYER_H_
#include <bitset>
#include "../Object.h"
#include "Vector2d.h"

namespace Duel6::net {
    class Player: public Object<Player, ObjectType::PLAYER> {
    public:
        tick_t debug = 12345;
        object_id_t id = 0;
        Int32 clientLocalId = 0;
        Vector2D position;
        Vector2D acceleration;
        Vector2D externalForces;
        Vector2D externalForcesSpeed;
        Vector2D velocity;

        Uint8 controls;
        Uint16 flags;
        Uint8 life;
        Uint8 air;
        Int16 ammo;
        Uint8 weaponId;
        std::array<Uint8, 16> unconfirmedInputs;
        bool orientationLeft;


        enum FIELDS {
            NO_CHANGE, // 1 means no other values should be de-/serialized
            ID, //mostly ignored
            CLIENTLOCALID, // mostly unused
            POSITION,
            ACCELERATION,
            EXTERNALFORCES,
            EXTERNALFORCESSPEED,
            VELOCITY,
            CONTROLS,
            FLAGS,
            LIFE,
            AIR,
            AMMO,
            WEAPONID,
            UNCONFIRMEDINPUTS,
            ORIENTATIONLEFT,
            _SIZE
        };
        std::bitset<FIELDS::_SIZE> changed;

#define D(POS, FIELD) result.FIELD = snapshot.FIELD; if(!(confirmed.FIELD == snapshot.FIELD)) result.changed.set(POS)

        static Player diff(const Player &snapshot, const Player &confirmed){
            Player result;
            result.id = snapshot.id;
            result.changed.reset();
            D(CLIENTLOCALID, clientLocalId);
            D(POSITION, position);
            D(ACCELERATION, acceleration);
            D(EXTERNALFORCES, externalForces);
            D(EXTERNALFORCESSPEED, externalForcesSpeed);
            D(VELOCITY, velocity);
            D(CONTROLS, controls);
            D(FLAGS, flags);
            D(LIFE, life);
            D(AIR, air);
            D(AMMO, ammo);
            D(WEAPONID, weaponId);
            D(UNCONFIRMEDINPUTS, unconfirmedInputs);
            D(ORIENTATIONLEFT, orientationLeft);
            if(result.changed.none()){
                result.changed.set(NO_CHANGE);
            }

            return result;
        }

#define R(POS, FIELD) if(unchanged || !received.changed[POS]) received.FIELD = confirmed.FIELD;
        static void fillinFromPreviousConfirmed(const Player &confirmed, Player &received){
                    //result.changed.reset();
                    bool unchanged = received.changed[NO_CHANGE];
                    R(CLIENTLOCALID, clientLocalId);
                    R(POSITION, position);
                    R(ACCELERATION, acceleration);
                    R(EXTERNALFORCES, externalForces);
                    R(EXTERNALFORCESSPEED, externalForcesSpeed);
                    R(VELOCITY, velocity);
                    R(CONTROLS, controls);
                    R(FLAGS, flags);
                    R(LIFE, life);
                    R(AIR, air);
                    R(AMMO, ammo);
                    R(WEAPONID, weaponId);
                    R(UNCONFIRMEDINPUTS, unconfirmedInputs);
                    R(ORIENTATIONLEFT, orientationLeft);
                }
        Player() {
            debug = 12345;
            changed.set();
            changed.set(NO_CHANGE, false);
        }

        bool operator==(const Player & r){
            return id == r.id
                && clientLocalId == r.clientLocalId
                && position == r.position
                && acceleration == r.acceleration
                && externalForces == r.externalForces
                && externalForcesSpeed == r.externalForcesSpeed
                && velocity == r.velocity
                && controls == r.controls
                && flags == r.flags
                && life == r.life
                && air == r.air
                && ammo == r.ammo
                && weaponId == r.weaponId
                && unconfirmedInputs == r.unconfirmedInputs
                && orientationLeft == r.orientationLeft;
        }

#define S(POS, FIELD) if(changed[POS]) result &= s & FIELD
        template<class Stream>
        bool serialize(Stream &s) {
            bool result = s & id;
                result &= s & debug;
                result &= s & changed;
                if(changed[NO_CHANGE]) return result;
                S(CLIENTLOCALID, clientLocalId);
                S(POSITION, position);
                S(ACCELERATION, acceleration);
                S(EXTERNALFORCES, externalForces);
                S(EXTERNALFORCESSPEED, externalForcesSpeed);
                S(VELOCITY, velocity);
                S(CONTROLS, controls);
                S(FLAGS, flags);
                S(LIFE, life);
                S(AIR, air);
                S(AMMO, ammo);
                S(WEAPONID, weaponId);
                S(UNCONFIRMEDINPUTS, unconfirmedInputs);
                S(ORIENTATIONLEFT, orientationLeft);
        return result;
        }

    };
}

#endif /* SOURCE_NET_OBJECT_PLAYER_H_ */
