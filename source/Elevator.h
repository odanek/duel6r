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
#include "math/Vector.h"
#include "renderer/RendererTypes.h"
#include "renderer/Renderer.h"

namespace Duel6 {
    namespace net {
        class Elevator;
        class ControlPoint;
        class ServerGameProxy;
    }
    class Elevator {
        friend class Duel6::net::Elevator;
        friend class Duel6::net::ServerGameProxy;
    public:
        class ControlPoint {
            friend class Duel6::net::ControlPoint;
        private:
            Vector location;
            Float32 wait;

        public:
            ControlPoint(Int32 x, Int32 y, Int32 wait)
                    : location(Float32(x), Float32(y)), wait(wait) {}

            const Vector &getLocation() const {
                return location;
            };

            Float32 getWait() const {
                return wait;
            }
        };

    private:
        std::vector<ControlPoint> controlPoints;
        bool circular;
        Size section;
        Float32 remainingWait;
        bool forward;
        Float32 distance;
        Float32 travelled;
        Vector position;
        Vector velocity;
        bool started = false;

    public:
        Elevator() = default;
        Elevator(const Elevator &) = default;
        Elevator& operator=(const Elevator &) = default;
        Elevator(Elevator &&) = default;
        Elevator& operator=(Elevator &&) = default;

        Elevator(bool circular);

        Elevator(const std::vector<ControlPoint> &controlPoints,
                 bool circular,
                 Size section,
                 Float32 remainingWait,
                 bool forward,
                 Float32 distance,
                 Float32 travelled,
                 Vector &position,
                 Vector &velocity,
                 bool started);
        Elevator(std::vector<ControlPoint> &&controlPoints,
                 bool circular,
                 Size section,
                 Float32 remainingWait,
                 bool forward,
                 Float32 distance,
                 Float32 travelled,
                 Vector &&position,
                 Vector &&velocity,
                 bool started);
        Elevator &addControlPoint(const ControlPoint &point) {
            controlPoints.push_back(point);
            return *this;
        }
~Elevator(){
    //?
}
        void start();

        void update(Float32 elapsedTime);

        void render(Renderer &renderer, Texture texture) const;

        const Vector &getPosition() const {
            return position;
        }

        const Vector &getVelocity() const {
            return remainingWait > 0 ? Vector::ZERO : velocity;
        }

    private:
        void nextSection();

        void startSection();
    };
}

#endif
