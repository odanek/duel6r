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

#include "Bonus.h"
#include "bonus/PlusLife.h"
#include "bonus/MinusLife.h"
#include "bonus/PowerfulShots.h"
#include "bonus/Invulnerability.h"
#include "bonus/Bullets.h"
#include "bonus/FastMovement.h"
#include "bonus/Invisibility.h"
#include "bonus/SplitFire.h"
#include "bonus/VampireShots.h"
#include "bonus/FullLife.h"
#include "bonus/FastReload.h"
#include "Video.h"

namespace Duel6 {
    namespace {
        class NoneBonus : public BonusTypeImpl {
        public:
            Texture getTexture() const override {
                return Texture();
            }

            bool isOneTime() const override {
                return true;
            }

            bool isApplicable(Player &player, World &world) const override {
                return false;
            }

            void onApply(Player &player, World &world, Int32 duration) const override {}

            void onExpire(Player &player, World &world) const override {}
        };

        static NoneBonus NONE_BONUS;
    }

    const BonusType BonusType::NONE;
    const BonusType BonusType::PLUS_LIFE;
    const BonusType BonusType::MINUS_LIFE;
    const BonusType BonusType::FULL_LIFE;
    const BonusType BonusType::FAST_RELOAD;
    const BonusType BonusType::POWERFUL_SHOTS;
    const BonusType BonusType::INVULNERABILITY;
    const BonusType BonusType::BULLETS;
    const BonusType BonusType::FAST_MOVEMENT;
    const BonusType BonusType::INVISIBILITY;
    const BonusType BonusType::SPLIT_FIRE;
    const BonusType BonusType::VAMPIRE_SHOTS;
    std::vector<BonusType> BonusType::types;
    std::vector<BonusType::BonusTypeImplPtr> BonusType::implementations;

    BonusType::BonusType()
            : impl(&NONE_BONUS) {}

    BonusType::BonusType(const BonusType &bonusType)
            : impl(bonusType.impl) {}

    void BonusType::assign(BonusTypeImplPtr &&impl) const {
        this->impl = impl.get();
        implementations.push_back(std::forward<BonusTypeImplPtr>(impl));
        types.push_back(*this);
    }

    Texture BonusType::getTexture() const {
        return impl->getTexture();
    }

    bool BonusType::isOneTime() const {
        return impl->isOneTime();
    }

    bool BonusType::isApplicable(Player &player, World &world) const {
        return impl->isApplicable(player, world);
    }

    void BonusType::onApply(Player &player, World &world, Int32 duration) const {
        impl->onApply(player, world, duration);
    }

    void BonusType::onExpire(Player &player, World &world) const {
        impl->onExpire(player, world);
    }

    bool BonusType::operator==(const BonusType &bonus) const {
        return impl == bonus.impl;
    }

    bool BonusType::operator!=(const BonusType &bonus) const {
        return impl != bonus.impl;
    }

    const std::vector<BonusType> &BonusType::values() {
        return types;
    }

    void BonusType::initialize(const TextureList &textures) {
        PLUS_LIFE.assign(std::make_unique<Bonuses::PlusLife>(textures.at(1)));
        MINUS_LIFE.assign(std::make_unique<Bonuses::MinusLife>(textures.at(2)));
        FULL_LIFE.assign(std::make_unique<Bonuses::FullLife>(textures.at(3)));
        FAST_RELOAD.assign(std::make_unique<Bonuses::FastReload>(textures.at(4)));
        POWERFUL_SHOTS.assign(std::make_unique<Bonuses::PowerfulShots>(textures.at(5)));
        INVULNERABILITY.assign(std::make_unique<Bonuses::Invulnerability>(textures.at(6)));
        BULLETS.assign(std::make_unique<Bonuses::Bullets>(textures.at(7)));
        FAST_MOVEMENT.assign(std::make_unique<Bonuses::FastMovement>(textures.at(8)));
        INVISIBILITY.assign(std::make_unique<Bonuses::Invisibility>(textures.at(9)));
        SPLIT_FIRE.assign(std::make_unique<Bonuses::SplitFire>(textures.at(10)));
        VAMPIRE_SHOTS.assign(std::make_unique<Bonuses::VampireShots>(textures.at(11)));
    }

    Bonus::Bonus(BonusType type, Int32 duration, const Vector &position, Texture texture)
            : bonus(type), duration(duration), position(position), texture(texture) {
        this->position.z = 0.5f;
    }

    void Bonus::render() const {
        Vector pos = getSpritePosition();
        Material material = Material::makeMaskedTexture(texture);
        globRenderer->quadXY(pos, Vector(1.0f, 1.0f), Vector(0.1f, 0.9f), Vector(0.8f, -0.8f), material);
    }

    LyingWeapon::LyingWeapon(Weapon weapon, Int32 bullets, const Vector &position)
            : weapon(weapon), position(position), bullets(bullets) {
        this->position.z = 0.5f;
    }

    void LyingWeapon::render() const {
        Vector pos = getSpritePosition();
        Material material = Material::makeMaskedTexture(weapon.getBonusTexture());
        globRenderer->quadXY(pos, Vector(1.0f, 1.0f), Vector(0.1f, 0.9f), Vector(0.8f, -0.8f), material);
    }
}