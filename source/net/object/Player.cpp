/*
 * Player.cpp
 *
 *  Created on: Apr 9, 2020
 *      Author: fanick
 */

#include "Player.h"

namespace Duel6::net {
    Player::Player(const Player &r)
        : debug(r.debug),
          rtt(r.rtt),
            id(r.id),
          clientLocalId(r.clientLocalId),
          position(r.position),
          acceleration(r.acceleration),
          externalForces(r.externalForces),
          externalForcesSpeed(r.externalForcesSpeed),
          velocity(r.velocity),
          controls(r.controls),
          flags(r.flags),
          life(r.life),
          air(r.air),
          ammo(r.ammo),
          weaponId(r.weaponId),
          unconfirmedInputs(r.unconfirmedInputs),
          orientationLeft(r.orientationLeft),
          changed(r.changed) {

    }
    Player& Player::operator=(const Player &r) {
        debug = r.debug;
        rtt = r.rtt;
        id = r.id;
        clientLocalId = r.clientLocalId;
        position = r.position;
        acceleration = r.acceleration;
        externalForces = r.externalForces;
        externalForcesSpeed = r.externalForcesSpeed;
        velocity = r.velocity;
        controls = r.controls;
        flags = r.flags;
        life = r.life;
        air = r.air;
        ammo = r.ammo;
        weaponId = r.weaponId;
        unconfirmedInputs = r.unconfirmedInputs;
        orientationLeft = r.orientationLeft;
        changed = r.changed;
        return *this;
    }
}
