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
        auto filterType = NEAREST_FILTER_BOOM.find(index) != NEAREST_FILTER_BOOM.end() ? TextureFilter::NEAREST
                                                                                       : TextureFilter::LINEAR;
        textures.boom = textureManager.load(Format("{0}/boom/") << wpnPath, filterType, true);
        textures.gun = textureManager.load(Format("{0}/gun/") << wpnPath, TextureFilter::NEAREST, true);
        textures.shot = textureManager.load(Format("{0}/shot/") << wpnPath, TextureFilter::NEAREST, true);

        if (!definition.shotSound.empty()) {
            samples.shot = sound.loadSample(std::string(D6_FILE_WEAPON_SOUNDS) + definition.shotSound);
        }
        if (!definition.boomSound.empty()) {
            samples.boom = sound.loadSample(std::string(D6_FILE_WEAPON_SOUNDS) + definition.boomSound);
        }
    }

    void LegacyWeapon::shoot(Player &player, Orientation orientation, World &world) const {
        Sprite shotSprite(definition.shotAnimation, textures.shot);
        auto spriteIterator = world.getSpriteList().addSprite(shotSprite);
        world.getShotList().addShot(makeShot(player, orientation, spriteIterator));
        samples.shot.play();
    }

    std::unique_ptr<Shot>
    LegacyWeapon::makeShot(Player &player, Orientation orientation, SpriteList::Iterator spriteIterator) const {
        return std::make_unique<LegacyShot>(player, *this, orientation, spriteIterator, getShotCollisionRectangle());
    }

    Sprite &LegacyWeapon::makeSprite(Sprite &sprite) const {
        return sprite.setAnimation(definition.animation).setTextures(textures.gun).setFrame(6).setLooping(
                AnimationLooping::OnceAndStop);
    }

    Texture LegacyWeapon::getBonusTexture() const {
        return textures.gun.at(definition.animation[12]);
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
}