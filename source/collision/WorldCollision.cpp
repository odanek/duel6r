/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "WorldCollision.h"
#include "../Rectangle.h"
namespace Duel6 {
//TODO Duplicity
static const float GRAVITATIONAL_ACCELERATION = -11.0f;

void CollidingEntity::collideWithElevators(ElevatorList & elevators, Float32 elapsedTime, Float32 speed) {
    elevator = elevators.checkCollider(*this, elapsedTime * speed);

    if(elevator != nullptr) {
        position.y = elevator->getPosition().y;
        position += elevator->getVelocity() * elapsedTime;

        velocity.y = 0.0f;
        if(isInWall()) {
            velocity.x = 0.0f;
        }
    }
}

void CollidingEntity::collideWithLevel(const Level & level, Float32 elapsedTime, Float32 speed) {
    static bool bleft = false, bright = false, bup = false, bdown = false;
    {
        Float32 delta = 0.8f * VERTICAL_DELTA;
        Float32 down = position.y - FLOOR_DISTANCE_THRESHOLD;
        Float32 left = position.x + delta;
        Float32 right = position.x + (1 - delta);
        lastCollisionCheck.onGround = level.isWall(left, down, true) || level.isWall(right, down, true);
    }
    {
        Float32 delta = VERTICAL_DELTA;
        Float32 up = position.y + DELTA_HEIGHT;
        Float32 left = position.x + delta;
        Float32 right = position.x + (1.0f - delta);
        lastCollisionCheck.clearForJump = !level.isWall(left, up, true) && !level.isWall(right, up, true);
    }
    lastCollisionCheck.inWall = level.isWall(Int32(getCollisionRect().getCentre().x), Int32(getCollisionRect().getCentre().y), true);
    if (!isOnElevator()) {
       velocity.y += GRAVITATIONAL_ACCELERATION * elapsedTime;    // gravity
    }

    if (!isOnGround()) {
        if (velocity.y < -D6_PLAYER_JUMP_SPEED) {
            velocity.y = -D6_PLAYER_JUMP_SPEED;
        }
    }

    externalForcesSpeed += externalForces;
    externalForces.x = 0;
    externalForces.y = 0;
    if (externalForcesSpeed.length() < 0.01) {
        externalForcesSpeed.x = 0;
        externalForcesSpeed.y = 0;
    }
    //friction
    if(velocity.x > 0.0f) {
        acceleration.x -= std::min(elapsedTime, velocity.x);
    }
    if(velocity.x < 0.0f) {
        acceleration.x += std::min(elapsedTime, std::abs(velocity.x));
    }

    //do not multiply by speed or elapsedTime !!!
    velocity += acceleration;

    acceleration.x = 0;
    acceleration.y = 0;
    // horizontal speed clamping
    if (std::abs(velocity.x * speed) > 0.5f) {
        velocity.x = std::copysign(0.5f, this->velocity.x) / speed;
    }
    if (std::abs(externalForcesSpeed.x * speed) > 0.5f) {
        externalForcesSpeed.x = std::copysign(0.5f, externalForcesSpeed.x) / speed;
    }
    Vector totalSpeed = velocity + externalForcesSpeed;
    bleft = false, bright = false, bup = false, bdown = false;

    //collision detection here we go

    {
        Float32 delta = VERTICAL_DELTA;
        Float32 up;
        Float32 down;
        Float32 left;
        Float32 right;

        /**
         * Down
         */
        up = position.y + DELTA_HEIGHT + totalSpeed.y * speed;
        down = position.y + totalSpeed.y * speed;
        left = position.x + delta + totalSpeed.x * speed;
        right = position.x + (1.0f - delta) + totalSpeed.x * speed;
        if (level.isWall(right, down, true) || level.isWall(left, down, true)) {
            bdown = true;
            velocity.y = 0;
            totalSpeed.y = 0;
            if (!(level.isWall(right, up, true) || level.isWall(left, down, true))) {
                velocity.y = -0.001f;
                totalSpeed.y = -0.001f;
            }
        }

        /**
         * Up
         */
        up = position.y + DELTA_HEIGHT + totalSpeed.y * speed;
        down = position.y + totalSpeed.y * speed;
        left = position.x + delta + totalSpeed.x * speed;
        right = position.x + (1.0f - delta) + totalSpeed.x * speed;

        if (totalSpeed.y > 0.0f && (level.isWall(right, up, true) || level.isWall(left, up, true))) {
            bup = true;
            totalSpeed.y = 0;
            velocity.y = 0;
        }

        /**
         * Right
         */
        delta = HORIZONTAL_DELTA;
        left = position.x + delta + totalSpeed.x * speed;
        down = position.y + 0.1f;
        if (totalSpeed.y > 0) {
            up = position.y + DELTA_HEIGHT + totalSpeed.y * speed;
        } else {
            up = position.y + DELTA_HEIGHT;
        }
        right = position.x + (1.0f - delta) + totalSpeed.x * speed;

        if (totalSpeed.x > 0 && (floorf(right) > floorf(position.x)) &&
            ((level.isWall(right, up, true) || level.isWall(right, down, true)))) {
            bright = true;
        }

        /**
         * Left
         */
        up = position.y + DELTA_HEIGHT;
        down = position.y + 0.1f;
        left = position.x + delta + totalSpeed.x * speed;
        right = position.x + (1.0f - delta) + totalSpeed.x * speed;
        if (totalSpeed.y > 0) {
            up = position.y + DELTA_HEIGHT + totalSpeed.y * speed;
        } else {
            up = position.y + DELTA_HEIGHT;
        }
        if (level.isWall(left, up, true) || level.isWall(left, down, true) || (left < 0)) {
            bleft = true;
        }
        /**
          * Put it all together
          */
         if (!isOnElevator()) {
             if (bdown) {
                 position.y = std::ceil(position.y - 0.5f);
             } else if (bup) {
                 position.y = std::floor(position.y + 0.5f);
             }

             if (bleft) {
                 position.x = std::ceil(position.x + totalSpeed.x * speed) - delta;
             } else if (bright) {
                 position.x = floorf(this->position.x) + delta;
             }
         } else if (isInWall() && (bleft || bright)) {
             externalForces.x = (elevator->getPosition().x - position.x);
         }
         if (bleft || bright) {
             externalForcesSpeed.x *= 0.5;
         }
         if (bup || bdown) {
             externalForcesSpeed.y *= 0.5;
         }
         externalForcesSpeed.x *= 0.9;
         externalForcesSpeed.y *= 0.9;
         position.x = std::max(-0.1f, std::min(position.x, level.getWidth() - 0.9f));
     }

    position.y += totalSpeed.y * speed; // the speed has the elapsedTime already factored in
    position.x += totalSpeed.x * D6_PLAYER_ACCEL * speed; // the speed has the elapsedTime already factored in
    lastCollisionCheck.up = bup;
    lastCollisionCheck.right = bright;
    lastCollisionCheck.down = bdown;
    lastCollisionCheck.left = bleft;
}

void CollidingEntity::initPosition(Float32 x, Float32 y, Float32 z) {
    position = {x, y, z};
    velocity = {0, 0};
    acceleration = {0, 0};
    externalForces = {0, 0};
    externalForcesSpeed = {0, 0};
    lastCollisionCheck = {true, true, true, true, true, true, false};
}

void CollidingEntity::setPosition(const Vector & position) {
    this->position = position;
}

void CollidingEntity::setPosition(Float32 x, Float32 y, Float32 z) {
//    Float32 deltaX = position.x - x;
//    Float32 deltaY = position.y - y;
//    if((deltaX * deltaX + deltaY * deltaY) < 0.1){
//
//        return;
//    } else {
//        externalForces.x -= 0.5 * deltaX; //rubberbanding
//        externalForces.y -= 0.5 * deltaY;
//    }
//    if((deltaX * deltaX + deltaY * deltaY) > 0.9){
        position = {x, y, z};
//    }
}
Rectangle CollidingEntity::getCollisionRect() const {
    return Rectangle::fromCornerAndSize(position, dimensions);
}

bool CollidingEntity::isInWall() const {
    return lastCollisionCheck.inWall;
}

bool CollidingEntity::isOnHardSurface() const {
    return isOnGround() || isOnElevator();
}

bool CollidingEntity::isUnderHardSurface() const {
    return !lastCollisionCheck.clearForJump;
}

bool CollidingEntity::isOnElevator() const {
    return elevator != nullptr;
}

bool CollidingEntity::isOnGround() const {
    return lastCollisionCheck.onGround;
}

Vector CollidingEntity::getPosition() const {
    return position;
}

}
