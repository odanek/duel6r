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

#include "Elevator.h"
#include "math/Math.h"
#include "Video.h"

#define D6_ELEV_SPEED 1.83f

namespace Duel6 {
    Elevator::Elevator(bool circular)
            : circular(circular) {}

    void Elevator::start() {
        if(started){
            return;
        }
        position = controlPoints[0].getLocation();
        section = 0;
        forward = true;
        remainingWait = 0;

        if (controlPoints.size() > 1) {
            startSection();
        } else {
            distance = 0;
            travelled = 0;
            velocity = Vector::ZERO;
        }
        started = true;
    }

    void Elevator::update(Float32 elapsedTime) {
        if (controlPoints.size() < 2) {
            return;
        }

        if (remainingWait > 0) {
            remainingWait = std::max(0.0f, remainingWait - elapsedTime);
            return;
        }

        if (travelled >= distance) {
            nextSection();
        }

        position += velocity * elapsedTime;
        travelled += elapsedTime;
    }

    void Elevator::render(Renderer &renderer, Texture texture) const {
        Float32 X = position.x, Y = position.y - 0.3f;
        Material material = Material::makeTexture(texture);

        // Front
        renderer.quadXY(Vector(X, Y, 0.7f), Vector(1.0f, 0.3f), Vector::ZERO, Vector(1, 1), material);

#ifdef D6_RENDER_BACKS
        renderer.quadXY(Vector(X + 1.0f, Y, 0.7f), Vector(-1.0f, 0.3f), Vector::ZERO, Vector(1, 1), material);
#endif
        renderer.quadYZ(Vector(X, Y, 0.3f), Vector(0.0f, 0.3f, 0.4f), Vector::ZERO, Vector(1, 1), material);
        renderer.quadYZ(Vector(X + 1.0f, Y, 0.7f), Vector(0.0f, 0.3f, -0.4f), Vector::ZERO, Vector(1, 1),
                             material);

        renderer.quadXZ(Vector(X, Y + 0.3f, 0.3f), Vector(1.0f, 0.0f, 0.4f), Vector::ZERO, Vector(1, 1), material);
        renderer.quadXZ(Vector(X, Y, 0.7f), Vector(1.0f, 0.0f, -0.4f), Vector::ZERO, Vector(1, 1), material);
    }

    void Elevator::nextSection() {
        Size pointCount = controlPoints.size();

        if (circular) {
            section = (section + 1) % pointCount;
        } else {
            if (forward) {
                if (section + 2 == pointCount) {
                    forward = false;
                } else {
                    ++section;
                }
            } else {
                if (section == 0) {
                    forward = true;
                } else {
                    --section;
                }
            }
        }

        startSection();
    }

    Elevator::Elevator(const std::vector<ControlPoint> &controlPoints,
                       bool circular,
                       Size section,
                       Float32 remainingWait,
                       bool forward,
                       Float32 distance,
                       Float32 travelled,
                       Vector &position,
                       Vector &velocity,
                       bool started)
        : controlPoints(controlPoints),
          circular(circular),
          section(section),
          remainingWait(remainingWait),
          forward(forward),
          distance(distance),
          travelled(travelled),
          position(position),
          velocity(velocity),
          started(started){
    }
    Elevator::Elevator(std::vector<ControlPoint> &&controlPoints,
                           bool circular,
                           Size section,
                           Float32 remainingWait,
                           bool forward,
                           Float32 distance,
                           Float32 travelled,
                           Vector &&position,
                           Vector &&velocity,
                           bool started)
            : controlPoints(controlPoints),
              circular(circular),
              section(section),
              remainingWait(remainingWait),
              forward(forward),
              distance(distance),
              travelled(travelled),
              position(position),
              velocity(velocity),
              started(started){
        }
    void Elevator::startSection() {
        Size pointCount = controlPoints.size();
        Size startIndex = section;
        Size endIndex = (section + 1) % pointCount;

        ControlPoint &startPoint = controlPoints[forward ? startIndex : endIndex];
        ControlPoint &endPoint = controlPoints[forward ? endIndex : startIndex];
        Vector dir = endPoint.getLocation() - startPoint.getLocation();
        distance = dir.length() / D6_ELEV_SPEED;
        travelled = 0;
        velocity = dir / distance;
        remainingWait = startPoint.getWait() / 1000.0f;
    }
}
