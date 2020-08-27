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
        tick_t snapshotTick = 0;
        uint32_t rtt = 0;
        object_id_t id = 0;
        Int32 clientLocalId = 0;
        Vector2D position = {0,0};
        Vector2D acceleration = {0,0};
        Vector2D externalForces = {0,0};
        Vector2D externalForcesSpeed = {0,0};
        Vector2D velocity = {0,0};
        Uint8 lastCollisionCheck = 0;

        Uint8 controls = 0;
        Uint16 flags = 0;
        Uint8 life = 42;
        Uint8 air = 42;
        Int16 ammo = 0;
        Uint8 weaponId = 0;

        BonusType bonusType = BonusType::NONE;
        Float32 bonusRemainingTime = 0;
        Float32 bonusDuration = 0;
        Float32 timeSinceHit = 0;

        Float32 alpha = 0;
        Float32 bodyAlpha = 0;
        Float32 timeToReload = 0;
        PlayerScore score;
        std::array<Uint8, INPUTS> unconfirmedInputs;
        bool orientationLeft = false;

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
            TIME_TO_RELOAD,
            SCORE,
            _SIZE
        };
        std::bitset<Player::FIELDS::_SIZE> changed;

        static void diff(Player &snapshot, const Player &confirmed);

        static void fillinFromPreviousConfirmed(const Player &confirmed, Player &received);

        Player();



#define S(POS, FIELD) if(changed[POS]) result &= s & FIELD
        template<class Stream>
        bool serialize(Stream &s) {
            bool result = s & id;
            result &= s & lastConfirmedTick;

            result &= s & changed;
            result &= s & score.changed;
            if (changed[NO_CHANGE]) return result;
            S(SCORE, score);
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
            S(TIME_TO_RELOAD, timeToReload);
            return result;
        }

    };
}

#endif /* SOURCE_NET_OBJECT_PLAYER_H_ */
