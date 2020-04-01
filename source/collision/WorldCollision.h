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

#ifndef DUEL6_COLLISION_WORLDCOLLISION_H
#define DUEL6_COLLISION_WORLDCOLLISION_H

#include "../math/Vector.h"
#include "../Level.h"
#include "../Elevator.h"
#include "../ElevatorList.h"
#include "../Rectangle.h"
#include "../Defines.h"

namespace Duel6 {
    class CollisionCheckResult {
    public:
        bool up = false;
        bool right = false;
        bool down = false;
        bool left = false;
        bool inWall = false;
        bool onGround = false;
        bool clearForJump = false;
    };
    class CollidingEntity {
        static constexpr float FLOOR_DISTANCE_THRESHOLD = 0.0001f;
        static constexpr float DELTA = 0.30f;
        static constexpr float VERTICAL_DELTA = 0.5f * DELTA;
        static constexpr float HORIZONTAL_DELTA = 0.25f * DELTA;
        static constexpr float DELTA_HEIGHT = 0.94f;
    public:
        CollidingEntity() = default;

        CollidingEntity(const Vector & position) :
                    position(position) {}

        CollidingEntity(const CollidingEntity & c):
            position(c.position),
            acceleration(c.acceleration),
            externalForces(c.externalForces),
            velocity(c.velocity),
            elevator(c.elevator),
            lastCollisionCheck(c.lastCollisionCheck) {
        }

        Vector position;
        Vector acceleration;
        Vector externalForces;
        Vector externalForcesSpeed;
        Vector velocity;
        Vector dimensions = {1.0f, 1.0f};
        const Elevator *elevator = nullptr;
        CollisionCheckResult lastCollisionCheck;
        Rectangle getCollisionRect() const;
        // aka update
        void collideWithElevators(ElevatorList & elevators, Float32 elapsedTime, Float32 speed = 1.0f);
        void collideWithLevel(const Level & level, Float32 elapsedTime, Float32 speed = 1.0f);
        void initPosition(Float32 x, Float32 y, Float32 z = 0.0f);
        void setPosition(Float32 x, Float32 y, Float32 z = 0.0f);

        bool isInWall() const;
        bool isOnHardSurface() const;
        bool isUnderHardSurface() const;
        bool isOnElevator() const;
        bool isOnGround() const;

    private:
        Vector boundingBoxHorizontal = {HORIZONTAL_DELTA, DELTA_HEIGHT};
        Vector boundingBoxVertical = {VERTICAL_DELTA, DELTA_HEIGHT};
        void checkElevator(Float32 speed);
    };
}

#endif
