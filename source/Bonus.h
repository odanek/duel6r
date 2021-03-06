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

#ifndef DUEL6_BONUS_H
#define DUEL6_BONUS_H

#include "Type.h"
#include "TextureManager.h"
#include "math/Vector.h"
#include "Rectangle.h"
#include "Weapon.h"
#include "collision/WorldCollision.h"

namespace Duel6 {
    class Player;

    class World;

    class BonusType {
    public:
        virtual ~BonusType() = default;

        virtual std::string getName() const = 0;

        virtual Int32 getTextureIndex() const = 0;

        virtual bool isOneTime() const = 0;

        virtual bool isApplicable(Player &player, World &world) const = 0;

        virtual void onApply(Player &player, World &world, Int32 duration) const = 0;

        virtual void onExpire(Player &player, World &world) const = 0;

    public:
        static const BonusType *const NONE;
        static const BonusType *const PLUS_LIFE;
        static const BonusType *const MINUS_LIFE;
        static const BonusType *const FULL_LIFE;
        static const BonusType *const FAST_RELOAD;
        static const BonusType *const POWERFUL_SHOTS;
        static const BonusType *const INVULNERABILITY;
        static const BonusType *const BULLETS;
        static const BonusType *const FAST_MOVEMENT;
        static const BonusType *const INVISIBILITY;
        static const BonusType *const SPLIT_FIRE;
        static const BonusType *const VAMPIRE_SHOTS;
        static const BonusType *const INFINITE_AMMO;
        static const BonusType *const SNORKEL;

    public:
        static const std::vector<const BonusType *> ALL;
    };

    class Bonus {
    private:
        const BonusType *type;
        Int32 duration;
        Vector position;
        Int32 textureIndex;

    public:
        Bonus(const BonusType *type, Int32 duration, const Vector &position, Int32 textureIndex);

        void render(Renderer &renderer, Texture texture) const;

        const BonusType *getType() const {
            return type;
        }

        Int32 getDuration() const {
            return duration;
        }

        const Vector &getPosition() const {
            return position;
        }

        Vector getDimensions() const {
            return Vector(0.6f, 0.6f);
        }

        Rectangle getCollisionRect() const {
            return Rectangle::fromCornerAndSize(getPosition(), getDimensions());
        }

        Vector getSpritePosition() const;
    };

    class LyingWeapon {
        Weapon weapon;
        Int32 bullets;
    public:
        CollidingEntity collider;
        Float32 pickTimeout = 0.5f;
        Float32 remainingReloadTime = 0.0f;
    public:
        LyingWeapon(Weapon weapon, Int32 bullets, Float32 remainingReloadTime, const CollidingEntity &playerCollider);

        LyingWeapon(Weapon weapon, Int32 bullets, const Vector &position);

        void render(Renderer &renderer) const;

        const Weapon &getWeapon() const {
            return weapon;
        }

        Int32 getBullets() const {
            return bullets;
        }

        const Vector &getPosition() const {
            return collider.position;
        }

        Vector getDimensions() const {
            return Vector(1.0f, 1.0f);
        }

        Rectangle getCollisionRect() const {
            return Rectangle::fromCornerAndSize(getPosition(), getDimensions());
        }

        Vector getSpritePosition() const;
    };
}

#endif
