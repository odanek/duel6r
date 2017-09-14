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

#ifndef DUEL6_LEGACYWEAPON_H
#define DUEL6_LEGACYWEAPON_H

#include <string>
#include "WeaponBase.h"
#include "../Shot.h"
#include "../SpriteList.h"

namespace Duel6 {
    class LegacyShot;

    class LegacyWeapon : public WeaponBase {
    public:
        struct Definition {
            Float32 bulletSpeed;
            bool blood;
            bool explodes;
            bool collides;
            Color explosionColor;
            Int32 boom;
            Int32 power;
            Float32 reloadSpeed;
            std::string name;
            std::string shotSound;
            std::string boomSound;
            Float32 expGrow;
            Int16 animation[16];
            Int16 shotAnimation[18];
            Int16 boomAnimation[14];
        };

        struct WeaponTextures {
            TextureList boom;
            TextureList gun;
            TextureList shot;
        };

        struct WeaponSamples {
            Sound::Sample shot;
            Sound::Sample boom;
        };

    protected:
        const Definition &definition;
        WeaponTextures textures;
        WeaponSamples samples;

    public:
        LegacyWeapon(Sound &sound, TextureManager &textureManager, const Definition &definition, Size index);

        void shoot(Player &player, Orientation orientation, World &world) const override;

        Sprite &makeSprite(Sprite &sprite) const override;

        Texture getBonusTexture() const override;

        const Definition &getDefinition() const;

        const WeaponTextures &getTextures() const;

        const WeaponSamples &getSamples() const;

    protected:
        virtual std::unique_ptr<Shot>
        makeShot(Player &player, Orientation orientation, SpriteList::Iterator spriteIterator) const;

        virtual Rectangle getShotCollisionRectangle() const = 0;
    };
}

#endif
