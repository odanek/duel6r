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

#ifndef DUEL6_WATER_H
#define DUEL6_WATER_H

#include <vector>
#include <memory>
#include "Sound.h"
#include "TextureManager.h"
#include "Vector.h"

namespace Duel6 {
    class WaterImpl;

    class Player;

    class World;

    class Water final {
    private:
        typedef std::unique_ptr<WaterImpl> WaterImplPtr;

    public:
        static const Water NONE;
        static const Water BLUE;
        static const Water RED;
        static const Water GREEN;

    private:
        mutable WaterImpl *impl;
        static std::vector<Water> types;
        static std::vector<WaterImplPtr> implementations;

        void assign(WaterImplPtr &&impl) const;

    public:
        Water();

        void onEnter(Player &player, const Vector &location, World &world);

        void onUnder(Player &player, Float32 elapsedTime);

        bool operator==(const Water &water) const;

        bool operator!=(const Water &water) const;

    public:
        static const std::vector<Water> &values();

        static void initialize(Sound &sound, TextureManager &textureManager);
    };
}

#endif