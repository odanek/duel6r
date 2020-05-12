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
            Int32 power;
            Float32 reloadSpeed;
            std::string name;
            std::string shotSound;
            std::string boomSound;
            AnimationEntry animation[16];
        };

        struct WeaponTextures {
            Texture boom;
            Texture gun;
            Texture shot;
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

        virtual Float32 getShotSpeed(Float32 coefficient) const;

        virtual Int32 getShotPower(Float32 coefficient) const;

        std::unique_ptr<Shot> shoot(Player &player, Orientation orientation, World &world,
                                   const Weapon &weapon,
                                   Uint32 shotId,
                                   bool powerful,
                                   Int32 power, Float32 bulletSpeed,
                                   Vector &position,
                                   Vector &velocity) const override;

        void shoot(Player &player, Orientation orientation, World &world) const override;

        SpriteList::Iterator makeSprite(SpriteList &spriteList) const override;

        Texture getBonusTexture() const override;

        Int32 getBonusTextureIndex() const override;

        const Definition &getDefinition() const;

        const WeaponTextures &getTextures() const;

        const WeaponSamples &getSamples() const;

        bool isChargeable() const override;

        virtual Float32 getBulletSpeed() const = 0;

    protected:
        virtual std::unique_ptr<Shot> makeShot(Player &player, World &world, Orientation orientation,
                                               const Weapon &weapon,
                                               Uint32 shotId,
                                               bool powerful,
                                               Int32 power, Float32 bulletSpeed,
                                               Vector &position,
                                               Vector &velocity) const = 0;
        virtual std::unique_ptr<Shot> makeShot(Player &player, World &world, Orientation orientation) const = 0;
    };
}

#endif
