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

#include "BazookaShot.h"

namespace Duel6 {
    namespace {
        const Rectangle collistionRectangle = Rectangle::fromCornerAndSize(Vector(0.035f, 0.65f), Vector(0.59f, 0.40f));
        const AnimationEntry shotAnimation[] = {0, 10, 1, 10, -1, 0};
        const AnimationEntry boomAnimation[] = {0, 1, 2, 1, 3, 1, 4, 1, 5, 1, 6, 1, 7, 1, 8, 1, 9, 1, 10, 1, 11,
                                                  1, 12, 1, 13, 1, 14, 1, 15, 1, 16, 1, 17, 1, 18, 1, 19, 1, 20, 1,
                                                  21, 1, 22, 1, 23, 1, 24, 1, 25, 1, 26, 1, 27, 1, 28, 1, 29, 1, 30, 1,
                                                  31, 1, 32, 1, 33, 1, 34, 1, 35, 1, -1, 0};
    }

    BazookaShot::BazookaShot(Player &player, World &world, const LegacyWeapon &weapon, Orientation orientation)
            : LegacyShot(player, world, weapon, shotAnimation, boomAnimation, orientation, collistionRectangle) {
    }

    bool BazookaShot::isColliding() const {
        return true;
    }

    bool BazookaShot::hasBlood() const {
        return false;
    }

    bool BazookaShot::hasPlayerExplosion() const {
        return true;
    }

    Color BazookaShot::getPlayerExplosionColor() const {
        return Color::RED;
    }

    Float32 BazookaShot::getExplosionRange() const {
        return 3.0f;
    }

    SpriteList::Iterator BazookaShot::makeBoomSprite(SpriteList &spriteList) {
        auto sprite = LegacyShot::makeBoomSprite(spriteList);
        sprite->setAlpha(1.0f).setBlendFunc(BlendFunc::SrcColor).setNoDepth(true);
        sprite->setGrow(0.03f * getPowerFactor());
        return sprite;
    }
}