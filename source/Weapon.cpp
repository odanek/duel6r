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

#include <iterator>
#include <stdlib.h>
#include "Sound.h"
#include "Weapon.h"
#include "GameSettings.h"
#include "weapon/LegacyWeapon.h"
#include "weapon/impl/Pistol.h"
#include "weapon/impl/Bazooka.h"
#include "weapon/impl/Lightning.h"
#include "weapon/impl/Shotgun.h"
#include "weapon/impl/Plasma.h"
#include "weapon/impl/Laser.h"
#include "weapon/impl/MachineGun.h"
#include "weapon/impl/Triton.h"
#include "weapon/impl/Uzi.h"
#include "weapon/impl/Bow.h"
#include "weapon/impl/Slime.h"
#include "weapon/impl/DoubleLaser.h"
#include "weapon/impl/ShitThrower.h"
#include "weapon/impl/KissOfDeath.h"
#include "weapon/impl/Spray.h"
#include "weapon/impl/Sling.h"
#include "weapon/impl/StopperGun.h"

namespace Duel6 {
    namespace {
        class NoneWeapon : public WeaponImpl {
        public:
            std::string getName() const override { return ""; }

            Float32 getReloadInterval() const override { return 0.0f; }

            std::unique_ptr<Shot> shoot(Player &player, Orientation orientation, World &world,
                                       const Weapon &weapon,
                                       Uint32 shotId,
                                       bool powerful,
                                       Int32 power, Float32 bulletSpeed,
                                       Vector &position,
                                       Vector &velocity) const override {return std::unique_ptr<Shot>();};

            void shoot(Player &player, Orientation orientation, World &world) const override {}

            SpriteList::Iterator makeSprite(SpriteList &spriteList) const override { return SpriteList::Iterator(); }

            Texture getBonusTexture() const override { return Texture(); }

            Int32 getBonusTextureIndex() const override {
                return 0;
            }

            bool isChargeable() const override {
                return false;
            }
        };

        NoneWeapon NO_WEAPON;
    }

    std::vector<Weapon::WeaponImplPtr> Weapon::implementations;
    std::vector<Weapon> Weapon::weapons;

    Weapon::Weapon()
            : impl(&NO_WEAPON) {}

    Weapon::Weapon(WeaponImpl *impl)
            : impl(impl) {}

    Uint8 Weapon::getId() const {
        return impl->getId();
    }

    Weapon Weapon::add(WeaponImplPtr &&impl) {
        Weapon weapon(impl.get());
        weapon.impl->setId(implementations.size());
        implementations.push_back(std::forward<WeaponImplPtr>(impl));
        weapons.push_back(weapon);
        return weapon;
    }

    std::string Weapon::getName() const {
        return impl->getName();
    }

    Float32 Weapon::getReloadInterval() const {
        return impl->getReloadInterval();
    }
    std::unique_ptr<Shot> Weapon::shoot(Player &player, Orientation orientation, World &world,
                       Uint32 shotId,
                       bool powerful,
                       Int32 power, Float32 bulletSpeed,
                       Vector &position,
                       Vector &velocity) const {
        return impl->shoot(player, orientation, world,
            *this, shotId, powerful, power, bulletSpeed, position, velocity);
    }

    void Weapon::shoot(Player &player, Orientation orientation, World &world) const {
        impl->shoot(player, orientation, world);
    }

    SpriteList::Iterator Weapon::makeSprite(SpriteList &spriteList) const {
        return impl->makeSprite(spriteList);
    }

    Texture Weapon::getBonusTexture() const {
        return impl->getBonusTexture();
    }

    Int32 Weapon::getBonusTextureIndex() const {
        return impl->getBonusTextureIndex();
    }

    bool Weapon::operator==(const Weapon &weapon) const {
        return impl == weapon.impl;
    }

    bool Weapon::operator!=(const Weapon &weapon) const {
        return impl != weapon.impl;
    }

    bool Weapon::isChargeable() const {
        return impl->isChargeable();
    }

    void Weapon::playShotSample(float panning) const {
        impl->playShotSample();
    }

    void Weapon::initialize(Sound &sound, TextureManager &textureManager, const PlayerAnimations & playerAnimations, const AuxAnimations & auxAnimations) {
        add(std::make_unique<Pistol>(sound, textureManager));
        add(std::make_unique<Bazooka>(sound, textureManager));
        add(std::make_unique<Lightning>(sound, textureManager));
        add(std::make_unique<Shotgun>(sound, textureManager));
        add(std::make_unique<Plasma>(sound, textureManager));
        add(std::make_unique<Laser>(sound, textureManager));
        add(std::make_unique<MachineGun>(sound, textureManager));
        add(std::make_unique<Triton>(sound, textureManager));
        add(std::make_unique<Uzi>(sound, textureManager));
        add(std::make_unique<Bow>(sound, textureManager));
        add(std::make_unique<Slime>(sound, textureManager));
        add(std::make_unique<DoubleLaser>(sound, textureManager));
        add(std::make_unique<KissOfDeath>(sound, textureManager));
        add(std::make_unique<Spray>(sound, textureManager));
        add(std::make_unique<Sling>(sound, textureManager));
        add(std::make_unique<StopperGun>(sound, textureManager));
        add(std::make_unique<ShitThrower>(sound, textureManager, playerAnimations, auxAnimations));
    }

    const std::vector<Weapon> &Weapon::values() {
        return weapons;
    }

    const Weapon &Weapon::getRandomEnabled(const GameSettings &settings) {
        auto &enabledWeapons = settings.getEnabledWeapons();
        Size randomIndex = Math::random(enabledWeapons.size());
        auto randomWeaponId = enabledWeapons.cbegin();
        std::advance(randomWeaponId, randomIndex);
        return Weapon::getById(*randomWeaponId);
    }

    const Weapon &Weapon::getById(Uint8 id) {
        if(id < weapons.size()) {
            return weapons[id];
        }
        return weapons[0];
    }
}
