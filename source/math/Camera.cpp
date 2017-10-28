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

#include "Camera.h"
#include "Math.h"

namespace Duel6 {
    Camera::Camera()
            : position(Vector::ZERO), front(Vector::UNIT_Z), up(Vector::UNIT_Y), side(Vector::UNIT_X),
              yaw(0), pitch(0), roll(0) {}

    Vector Camera::getPosition() const {
        return position;
    }

    void Camera::setPosition(const Vector &position) {
        this->position = position;
    }

    Vector Camera::getFront() const {
        return front;
    }

    Vector Camera::getUp() const {
        return up;
    }

    Vector Camera::getSide() const {
        return side;
    }

    Float32 Camera::getYaw() const {
        return yaw;
    }

    Float32 Camera::getPitch() const {
        return pitch;
    }

    Float32 Camera::getRoll() const {
        return roll;
    }

    void Camera::rotate(Float32 yaw, Float32 pitch, Float32 roll) {
        this->yaw += yaw;
        this->pitch += pitch;
        this->roll += roll;
        update();
    }

    void Camera::update() {
        Float32 a = Math::degSin(pitch);
        Float32 b = Math::degCos(pitch);
        Float32 c = Math::degSin(yaw);
        Float32 d = Math::degCos(yaw);
        Float32 e = Math::degSin(roll);
        Float32 f = Math::degCos(roll);

        front.x = c * b;
        front.y = -a;
        front.z = b * d;

        side.x = d * f + c * a * e;
        side.y = e * b;
        side.z = c * e + d * a * f;;

        up.x = -d * e + c * a * f;
        up.y = f * b;
        up.z = c * e + d * a * f;
    }
}