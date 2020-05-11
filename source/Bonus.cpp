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
#include "bonus/InfiniteAmmo.h"
#include "bonus/Snorkel.h"
#include "Video.h"

namespace Duel6 {
    std::vector<const BonusType *> BonusType::ALL = {};
    const BonusType *const BonusType::NONE = new Bonuses::None();
    const BonusType *const BonusType::PLUS_LIFE = new Bonuses::PlusLife();
    const BonusType *const BonusType::MINUS_LIFE = new Bonuses::MinusLife();
    const BonusType *const BonusType::FULL_LIFE = new Bonuses::FullLife();
    const BonusType *const BonusType::FAST_RELOAD = new Bonuses::FastReload();
    const BonusType *const BonusType::POWERFUL_SHOTS = new Bonuses::PowerfulShots();
    const BonusType *const BonusType::INVULNERABILITY = new Bonuses::Invulnerability();
    const BonusType *const BonusType::BULLETS = new Bonuses::Bullets();
    const BonusType *const BonusType::FAST_MOVEMENT = new Bonuses::FastMovement();
    const BonusType *const BonusType::INVISIBILITY = new Bonuses::Invisibility();
    const BonusType *const BonusType::SPLIT_FIRE = new Bonuses::SplitFire();
    const BonusType *const BonusType::VAMPIRE_SHOTS = new Bonuses::VampireShots();
    const BonusType *const BonusType::INFINITE_AMMO = new Bonuses::InfiniteAmmo();
    const BonusType *const BonusType::SNORKEL = new Bonuses::Snorkel();


    const BonusType * BonusType::getById(unsigned int id) {
        if(id >= BonusType::counter){
            return nullptr;
        }
        return BonusType::ALL[id];
    }
    const BonusType * BonusType::getRandom() {
        return BonusType::ALL[1 + Math::random(BonusType::ALL.size() - 1)]; // exclude none
    }
    unsigned int BonusType::counter = 0;
    unsigned int Bonus::counter = 0;
    unsigned int LyingWeapon::counter = 0;

    BonusType::BonusType() {
        BonusType::ALL.push_back((const BonusType *)this);
        this->id = BonusType::counter;
        BonusType::counter++;
    }
    unsigned int BonusType::getId() const {
        return id;
    }

    Bonus::Bonus(const BonusType *type, Int32 duration, const Vector &position, Int32 textureIndex)
            : type(type), duration(duration), position(position), textureIndex(textureIndex), id(counter++) {
        this->position.z = 0.5f;
    }

    unsigned int Bonus::getId() const {
        return id;
    }

    Bonus::Bonus(unsigned int id, unsigned int bonusTypeId, Int32 duration, const Vector &position, Int32 textureIndex)
    : type(BonusType::getById(bonusTypeId)), duration(duration), position(position), textureIndex(textureIndex), id(id){
        this->position.z = 0.5f;
    }

    void Bonus::render(Renderer &renderer, Texture texture) const {
        Vector pos = getSpritePosition();
        Material material = Material::makeMaskedTexture(texture);
        renderer.quadXY(pos, Vector(1.0f, 1.0f), Vector(0.1f, 0.9f, Float32(textureIndex)), Vector(0.8f, -0.8f),
                        material);
    }

    Int32 Bonus::getTextureIndex() const {
        return textureIndex;
    }

    Vector Bonus::getSpritePosition() const {
        return Vector(position.x - 0.2f, position.y - 0.2f, 0.47f);
    }

    LyingWeapon::LyingWeapon(Weapon weapon, Int32 bullets, const Vector &position) :
            weapon(weapon), bullets(bullets), id(counter++), collider(position) {
        collider.position.z = 0.5f;
    }

    LyingWeapon::LyingWeapon(Weapon weapon, Int32 bullets, Float32 remainingReloadTime,
                             const CollidingEntity &playerCollider)
            : weapon(weapon), bullets(bullets), id(counter++), collider(playerCollider), remainingReloadTime(remainingReloadTime) {
        collider.position.z = 0.5f;
        collider.velocity.x *= 2;
        collider.velocity.y *= 2;
    }

    LyingWeapon::LyingWeapon(unsigned int weaponId, unsigned int id, Int32 bullets, Float32 remainingReloadTime, const CollidingEntity &collider)
        : weapon(Weapon::getById(weaponId)),
          bullets(bullets),
          id(id),
          collider(collider),
          remainingReloadTime(remainingReloadTime){
        this->collider.position.z = 0.5f;
    }

    void LyingWeapon::render(Renderer &renderer) const {
        Vector pos = getSpritePosition();
        Material material = Material::makeMaskedTexture(weapon.getBonusTexture());
        renderer.quadXY(pos, Vector(1.0f, 1.0f), Vector(0.1f, 0.9f, Float32(weapon.getBonusTextureIndex())),
                        Vector(0.8f, -0.8f), material);
    }

    unsigned int LyingWeapon::getId() const {
        return id;
    }

    Vector LyingWeapon::getSpritePosition() const {
        auto position = collider.position;
        return Vector(position.x, position.y, 0.47f);
    }
}
