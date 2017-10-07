
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

#include "File.h"
#include "Math.h"

namespace Duel6 {
    std::random_device Math::randomDevice;
    std::default_random_engine Math::randomEngine(randomDevice());
    const Float64 Math::Pi = 3.14159265358979323846;

    Int32 Math::random(Int32 max) {
        return random(0, max);
    }

    Int32 Math::random(Int32 min, Int32 max) {
        std::uniform_int_distribution<> uniformDistribution(min, max - 1);
        return uniformDistribution(randomEngine);
    }

    Float32 Math::random(Float32 min, Float32 max) {
        std::uniform_real_distribution<Float32> uniformDistribution(min, max);
        return uniformDistribution(randomEngine);
    }

    Float64 Math::random(Float64 min, Float64 max) {
        std::uniform_real_distribution<Float64> uniformDistribution(min, max);
        return uniformDistribution(randomEngine);
    }
}