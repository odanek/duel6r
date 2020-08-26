/*
 * Player.cpp
 *
 *  Created on: Apr 9, 2020
 *      Author: fanick
 */

#include "Player.h"

namespace Duel6::net {
#define D(POS, FIELD) if(!(confirmed.FIELD == snapshot.FIELD)) snapshot.changed.set(POS)
        void Player::diff(Player &snapshot, const Player &confirmed) {
            PlayerScore::diff(snapshot.score, confirmed.score);

            snapshot.changed.reset();
            if(!snapshot.score.changed[PlayerScore::NO_CHANGE]){
                snapshot.changed.set(SCORE);
            }
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
            D(BONUS_REMAINING_TIME, bonusRemainingTime);
            D(BONUS_DURATION, bonusDuration);
            D(TIME_SINCE_HIT, timeSinceHit);
            D(ALPHA, alpha);
            D(BODY_ALPHA, bodyAlpha);

            if (snapshot.changed.none()) {
                snapshot.changed.set(NO_CHANGE);
            }
        }

#define R(POS, FIELD) if(unchanged || !received.changed[POS]) received.FIELD = confirmed.FIELD;
        void Player::fillinFromPreviousConfirmed(const Player &confirmed, Player &received) {
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
            R(SCORE, score);
            if(received.changed[SCORE]){
                PlayerScore::fillinFromPreviousConfirmed(confirmed.score, received.score);
            }
        }
        Player::Player():alpha(1), bodyAlpha(1) {
            lastConfirmedTick = 0;
            changed.set();
            changed.set(NO_CHANGE, false);
        }
}
