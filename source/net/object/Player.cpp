/*
 * Player.cpp
 *
 *  Created on: Apr 9, 2020
 *      Author: fanick
 */

#include "Player.h"
#include "../../Player.h"
namespace Duel6::net {

    void Player::loadFromPlayer(Duel6::Player &player) {

        // TODO include rest of the attributes, clean up code that should call this code instead
        auxFlags = 0;
        if (player.isChatting()) {
            auxFlags |= (1 << AUX_FLAGS::CHAT);
        }
        if(player.isInConsole()){
            auxFlags |= (1 << AUX_FLAGS::CONSOLE);
        }
        if(player.isFocused()){
            auxFlags |= (1 << AUX_FLAGS::FOCUS);
        }
    }

    void Player::unloadToPlayer(Duel6::Player &player) {
        player.setChatting((auxFlags & (1 << AUX_FLAGS::CHAT)) == (1 << AUX_FLAGS::CHAT));
        player.setInConsole((auxFlags & (1 << AUX_FLAGS::CONSOLE)) == (1 << AUX_FLAGS::CONSOLE));
        player.setFocused((auxFlags & (1 << AUX_FLAGS::FOCUS)) == (1 << AUX_FLAGS::FOCUS));
    }

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
            D(AUX_FLAGS, auxFlags);
            D(UNCONFIRMEDINPUTS, unconfirmedInputs);
            D(ORIENTATIONLEFT, orientationLeft);
            D(BONUS_TYPE, bonusType);
            D(BONUS_REMAINING_TIME, bonusRemainingTime);
            D(BONUS_DURATION, bonusDuration);
            D(TIME_SINCE_HIT, timeSinceHit);
            D(ALPHA, alpha);
            D(BODY_ALPHA, bodyAlpha);
            D(TIME_TO_RELOAD, timeToReload);

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
            R(AUX_FLAGS, auxFlags);
            R(UNCONFIRMEDINPUTS, unconfirmedInputs);
            R(ORIENTATIONLEFT, orientationLeft);
            R(BONUS_TYPE, bonusType);
            R(BONUS_REMAINING_TIME, bonusRemainingTime);
            R(BONUS_DURATION, bonusDuration);
            R(TIME_SINCE_HIT, timeSinceHit);
            R(ALPHA, alpha);
            R(BODY_ALPHA, bodyAlpha);
            R(TIME_TO_RELOAD, timeToReload);
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
