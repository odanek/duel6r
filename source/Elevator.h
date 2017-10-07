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

#ifndef DUEL6_ELEVATOR_H
#define DUEL6_ELEVATOR_H

#include <vector>
#include "Type.h"
#include "Vector.h"
#include "Texture.h"

namespace Duel6 {
    class Elevator {
    public:
        class ControlPoint {
        private:
            Vector location;

        public:
            ControlPoint(Float32 x, Float32 y)
                    : location(x, y) {}

            ControlPoint(Int32 x, Int32 y)
                    : location(Float32(x), Float32(y)) {}

            const Vector &getLocation() const {
                return location;
            };
        };

    private:
        std::vector<ControlPoint> controlPoints;
        Size section;
        bool forward;
        Float32 distance;
        Float32 travelled;
        Float32 accelerate;
        Vector position;
        Vector velocity;

    public:
        Elevator &addControlPoint(const ControlPoint &point) {
            controlPoints.push_back(point);
            return *this;
        }

        void start();

        void update(Float32 elapsedTime);

        void render(const Texture &texture) const;

        const Vector &getPosition() const {
            return position;
        }

        const Vector getAcceleratedVelocity() const {
            return velocity * accelerate;
        }

        const Vector &getVelocity() const {
            return velocity;
        }

        Float32 getAccelerate() const {
            return accelerate;
        }

    private:
        void nextSection();

        void startSection();
    };
}

#endif
