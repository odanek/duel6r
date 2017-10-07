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

#ifndef DUEL6_MATH_H
#define DUEL6_MATH_H

#include <random>
#include <string>
#include <cstdlib>
#include <cmath>
#include "Type.h"

namespace Duel6 {
    class Math {
    public:
        static const Float64 Pi;
        static std::random_device randomDevice;
        static std::default_random_engine randomEngine;

    public:
        template<class T>
        static T radianSin(T angle) {
            return std::sin(angle);
        }

        template<class T>
        static T radianCos(T angle) {
            return std::cos(angle);
        }

        template<class T>
        static T radianTan(T angle) {
            return std::tan(angle);
        }

        template<class T>
        static T degSin(T angle) {
            return radianSin(angleToRadians(angle));
        }

        template<class T>
        static T degCos(T angle) {
            return radianCos(angleToRadians(angle));
        }

        template<class T>
        static T degTan(T angle) {
            return radianTan(angleToRadians(angle));
        }

        template<class T>
        static T sqr(T val) {
            return val * val;
        }

        template<class T>
        static T norm(T x, T y, T z = 0) {
            return std::sqrt(sqr(x) + sqr(y) + sqr(z));
        }

        template<class T>
        static T distance(T x1, T y1, T x2, T y2, T z1 = 0, T z2 = 0) {
            return norm(x1 - x2, y1 - y2, z1 - z2);
        }

        template<class T>
        static Int32 sign(T val) {
            return val < 0 ? -1 : (val > 0 ? 1 : 0);
        }

        template<class T>
        static T angleToRadians(T angle) {
            return T(angle * Pi / 180.0);
        }

        template<class T>
        static T radiansToAngle(T radians) {
            return T(radians * 180.0 / Pi);
        }

        static Float32 angleDiff(Float32 left, Float32 right) {
            Float32 diff = std::abs(left - right);
            return std::min(diff, 360.0f - diff);
        }

        static Int32 random(Int32 max);

        static Int32 random(Int32 min, Int32 max);

        static Float32 random(Float32 min, Float32 max);

        static Float64 random(Float64 min, Float64 max);
    };
}

#endif