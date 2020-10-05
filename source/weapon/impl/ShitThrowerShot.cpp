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

#include "ShitThrowerShot.h"

namespace Duel6 {
    namespace {
        const Rectangle collistionRectangle = Rectangle::fromCornerAndSize(Vector(0.28f, 0.72f), Vector(0.38f, 0.18f));
        const AnimationEntry shotAnimation[] = {0, 164, 1, 164, 2, 164, 1, 164, -1, 0};
        const AnimationEntry boomAnimation[] = {0, 164, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    }
    ShitThrowerShot::ShitThrowerShot(Player &owner, World &world, const LegacyWeapon &legacyWeapon, Orientation shotOrientation,
                                     PlayerSkin &brownSkin,
                                     const Weapon &weapon,
                                     Uint32 shotId,
                                     bool powerful,
                                     Int32 power,
                                     Float32 bulletSpeed,
                                     Vector &position,
                                     Vector &velocity)
        : LegacyShot(owner, world, legacyWeapon, shotAnimation, boomAnimation, shotOrientation, collistionRectangle,
              weapon, shotId, powerful, power,
              bulletSpeed, position, velocity),
          brownSkin(brownSkin) {
    }
    ShitThrowerShot::ShitThrowerShot(Player &player, World &world, const LegacyWeapon &weapon, Orientation orientation,
                                     PlayerSkin &brownSkin)
            : LegacyShot(player, world, weapon, shotAnimation, boomAnimation, orientation, collistionRectangle), brownSkin(brownSkin) {
    }

    bool ShitThrowerShot::isColliding() const {
        return true;
    }

    bool ShitThrowerShot::hasBlood() const {
        return false;
    }

    Float32 ShitThrowerShot::getExplosionRange() const {
        return 2.0f;
    }

    void ShitThrowerShot::onExplode(const Vector &centre, Float32 range, World &world) {
    }

    void ShitThrowerShot::onHitPlayer(Player &player, bool directHit, const Vector &point, World &world) {
        player.useTemporarySkin(brownSkin);
    }

    SpriteList::Iterator ShitThrowerShot::makeBoomSprite(SpriteList &spriteList) {
        auto sprite = LegacyShot::makeBoomSprite(spriteList);
        sprite->setNoDepth(true).setGrow(2.44f * getPowerFactor());
        return sprite;
    }
}
