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

#ifndef DUEL6_VERTEX_H
#define DUEL6_VERTEX_H

#include "Type.h"

namespace Duel6 {
    class Vertex {
    public:
        enum class Flag {
            None,
            Flow
        };

    private:
        Flag flag;

    public:
        Float32 x;
        Float32 y;
        Float32 z;
        Float32 u;
        Float32 v;

    public:
        Vertex(Size order, Float32 x, Float32 y, Float32 z, Flag flag = Flag::None) {
            this->x = x;
            this->y = y;
            this->z = z;
            u = (order == 0 || order == 3) ? 0.0f : 0.99f;
            v = (order == 0 || order == 1) ? 0.0f : 0.99f;
            this->flag = flag;
        }

        Vertex(Size order, Int32 x, Int32 y, Int32 z, Flag flag = Flag::None)
                : Vertex(order, Float32(x), Float32(y), Float32(z), flag) {}

        Flag getFlag() const {
            return flag;
        }
    };
}

#endif