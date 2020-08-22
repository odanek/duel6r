/*
 * Player.h
 *
 *  Created on: Mar 15, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_PLAYER_H_
#define SOURCE_NET_OBJECT_PLAYER_H_
#include <bitset>
#include <cstdlib>
#include "../Object.h"
#include "Vector2d.h"
#include "Bonus.h"
#include "PlayerScore.h"

namespace Duel6::net {
    class Player: public Object<Player, ObjectType::PLAYER> {
    public:
        static constexpr const size_t INPUTS = 16;
        tick_t lastConfirmedTick = 0;
        uint32_t rtt = 0;
        object_id_t id = 0;
        Int32 clientLocalId = 0;
        Vector2D position;
        Vector2D acceleration;
        Vector2D externalForces;
        Vector2D externalForcesSpeed;
        Vector2D velocity;
        Uint8 lastCollisionCheck;

        Uint8 controls;
        Uint16 flags;
        Uint8 life;
        Uint8 air;
        Int16 ammo;
        Uint8 weaponId;

        BonusType bonusType;
        Float32 bonusRemainingTime;
        Float32 bonusDuration;
        Float32 timeSinceHit;

        Float32 alpha;
        Float32 bodyAlpha;
        PlayerScore score;
        std::array<Uint8, INPUTS> unconfirmedInputs;
        bool orientationLeft;

        enum FIELDS {
            NO_CHANGE, // 1 means no other values should be de-/serialized
            RTT,
            ID, //mostly ignored
            CLIENTLOCALID, // mostly unused
            POSITION,
            ACCELERATION,
            EXTERNALFORCES,
            EXTERNALFORCESSPEED,
            VELOCITY,
            LAST_COLLISION_CHECK,
            CONTROLS,
            FLAGS,
            LIFE,
            AIR,
            AMMO,
            WEAPONID,
            UNCONFIRMEDINPUTS,
            ORIENTATIONLEFT,
            BONUS_TYPE,
            BONUS_REMAINING_TIME,
            BONUS_DURATION,
            TIME_SINCE_HIT,
            ALPHA,
            BODY_ALPHA,
            SCORE,
            _SIZE
        };
        std::bitset<FIELDS::_SIZE> changed;

#define D(POS, FIELD) if(!(confirmed.FIELD == snapshot.FIELD)) snapshot.changed.set(POS)
#define Dfloat(POS, FIELD, d) if(std::abs(confirmed.FIELD - snapshot.FIELD) > d) snapshot.changed.set(POS)
        static void diff(Player &snapshot, const Player &confirmed) {
            snapshot.changed.reset();
            D(RTT, rtt);
            D(CLIENTLOCALID, clientLocalId);
            D(POSITION, position);
            D(ACCELERATION, acceleration);
            D(EXTERNALFORCES, externalForces);
            D(EXTERNALFORCESSPEED, externalForcesSpeed);
            D(VELOCITY, velocity);
            D(LAST_COLLISION_CHECK, lastCollisionCheck);
            D(CONTROLS, controls);
            D(FLAGS, flags);
            D(LIFE, life);
            D(AIR, air);
            D(AMMO, ammo);
            D(WEAPONID, weaponId);
            D(UNCONFIRMEDINPUTS, unconfirmedInputs);
            D(ORIENTATIONLEFT, orientationLeft);
            D(BONUS_TYPE, bonusType);
            Dfloat(BONUS_REMAINING_TIME, bonusRemainingTime, 0.2);
            D(BONUS_DURATION, bonusDuration);
            Dfloat(TIME_SINCE_HIT, timeSinceHit, 0.4);
            D(ALPHA, alpha);
            D(BODY_ALPHA, bodyAlpha);
            PlayerScore::diff(snapshot.score, confirmed.score);
            if(!snapshot.score.changed[0]){
                snapshot.changed.set(SCORE);
            }
            if (snapshot.changed.none()) {
                snapshot.changed.set(NO_CHANGE);
            }
        }

#define R(POS, FIELD) if(unchanged || !received.changed[POS]) received.FIELD = confirmed.FIELD;
        static void fillinFromPreviousConfirmed(const Player &confirmed, Player &received) {
            bool unchanged = received.changed[NO_CHANGE];
            R(RTT, rtt);
            R(CLIENTLOCALID, clientLocalId);
            R(POSITION, position);
            R(ACCELERATION, acceleration);
            R(EXTERNALFORCES, externalForces);
            R(EXTERNALFORCESSPEED, externalForcesSpeed);
            R(VELOCITY, velocity);
            R(LAST_COLLISION_CHECK, lastCollisionCheck);
            R(CONTROLS, controls);
            R(FLAGS, flags);
            R(LIFE, life);
            R(AIR, air);
            R(AMMO, ammo);
            R(WEAPONID, weaponId);
            R(UNCONFIRMEDINPUTS, unconfirmedInputs);
            R(ORIENTATIONLEFT, orientationLeft);
            R(BONUS_TYPE, bonusType);
            R(BONUS_REMAINING_TIME, bonusRemainingTime);
            R(BONUS_DURATION, bonusDuration);
            R(TIME_SINCE_HIT, timeSinceHit);
            R(ALPHA, alpha);
            R(BODY_ALPHA, bodyAlpha);
            PlayerScore::fillinFromPreviousConfirmed(confirmed.score, received.score);
        }
        Player() {
            lastConfirmedTick = 0;
            changed.set();
            changed.set(NO_CHANGE, false);
        }



#define S(POS, FIELD) if(changed[POS]) result &= s & FIELD
        template<class Stream>
        bool serialize(Stream &s) {
            bool result = s & id;
            result &= s & lastConfirmedTick;
            result &= s & changed;
            if (changed[NO_CHANGE]) return result;
            S(RTT, rtt);
            S(CLIENTLOCALID, clientLocalId);
            S(POSITION, position);
            S(ACCELERATION, acceleration);
            S(EXTERNALFORCES, externalForces);
            S(EXTERNALFORCESSPEED, externalForcesSpeed);
            S(VELOCITY, velocity);
            S(LAST_COLLISION_CHECK, lastCollisionCheck);
            S(CONTROLS, controls);
            S(FLAGS, flags);
            S(LIFE, life);
            S(AIR, air);
            S(AMMO, ammo);
            S(WEAPONID, weaponId);
            S(UNCONFIRMEDINPUTS, unconfirmedInputs);
            S(ORIENTATIONLEFT, orientationLeft);
            S(BONUS_TYPE, bonusType);
            S(BONUS_REMAINING_TIME, bonusRemainingTime);
            S(BONUS_DURATION, bonusDuration);
            S(TIME_SINCE_HIT, timeSinceHit);
            S(ALPHA, alpha);
            S(BODY_ALPHA, bodyAlpha);
            S(SCORE, score);
            return result;
        }

    };
}

#endif /* SOURCE_NET_OBJECT_PLAYER_H_ */
