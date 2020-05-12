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

#include <unordered_set>
#include "../World.h"
#include "LegacyWeapon.h"
#include "LegacyShot.h"

namespace Duel6 {
    namespace {
        const std::unordered_set<Size> NEAREST_FILTER_BOOM = {5, 6, 9, 13, 14, 15, 16};
    }

    LegacyWeapon::LegacyWeapon(Sound &sound, TextureManager &textureManager, const Definition &definition, Size index)
            : WeaponBase(definition.name, definition.reloadSpeed), definition(definition) {
        const std::string wpnPath = Format("{0}{1,3|0}") << D6_TEXTURE_WPN_PATH << index;
        auto filterType = NEAREST_FILTER_BOOM.find(index) != NEAREST_FILTER_BOOM.end() ? TextureFilter::Nearest
                                                                                       : TextureFilter::Linear;
        textures.boom = textureManager.loadStack(Format("{0}/boom/") << wpnPath, filterType, true);
        textures.gun = textureManager.loadStack(Format("{0}/gun/") << wpnPath, TextureFilter::Nearest, true);
        textures.shot = textureManager.loadStack(Format("{0}/shot/") << wpnPath, TextureFilter::Nearest, true);

        if (!definition.shotSound.empty()) {
            samples.shot = sound.loadSample(std::string(D6_FILE_WEAPON_SOUNDS) + definition.shotSound);
        }
        if (!definition.boomSound.empty()) {
            samples.boom = sound.loadSample(std::string(D6_FILE_WEAPON_SOUNDS) + definition.boomSound);
        }
    }

    Float32 LegacyWeapon::getShotSpeed(Float32 coefficient) const {
        return isChargeable() ? getBulletSpeed() * coefficient : getBulletSpeed();
    }

    Int32 LegacyWeapon::getShotPower(Float32 coefficient) const {
        if (isChargeable()) {
            return coefficient * definition.power;
        } else {
            return definition.power;
        }
    }
    std::unique_ptr<Shot> LegacyWeapon::shoot(Player &player, Orientation orientation, World &world,
                             const Weapon &weapon,
                             Uint32 shotId,
                             bool powerful,
                             Int32 power, Float32 bulletSpeed,
                             Vector &position,
                             Vector &velocity) const {
       // samples.shot.play();
        return makeShot(player, world, orientation, weapon, shotId,
            powerful, power, bulletSpeed, position, velocity);

    }
    void LegacyWeapon::shoot(Player &player, Orientation orientation, World &world) const {
        world.getShotList().addShot(makeShot(player, world, orientation));
        samples.shot.play();
    }

    SpriteList::Iterator LegacyWeapon::makeSprite(SpriteList &spriteList) const {
        auto sprite = spriteList.add(definition.animation, textures.gun);
        sprite->setFrame(0).setLooping(AnimationLooping::OnceAndStop);
        return sprite;
    }

    Texture LegacyWeapon::getBonusTexture() const {
        return textures.gun;
    }

    Int32 LegacyWeapon::getBonusTextureIndex() const {
        return definition.animation[12];
    }

    const LegacyWeapon::Definition &LegacyWeapon::getDefinition() const {
        return definition;
    }

    const LegacyWeapon::WeaponTextures &LegacyWeapon::getTextures() const {
        return textures;
    }

    const LegacyWeapon::WeaponSamples &LegacyWeapon::getSamples() const {
        return samples;
    }

    bool LegacyWeapon::isChargeable() const {
        return false;
    }
}
