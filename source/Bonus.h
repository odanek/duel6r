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
#include "Vector.h"
#include "Rectangle.h"
#include "Weapon.h"

namespace Duel6 {
    class Player;

    class World;

    class BonusTypeImpl {
    public:
        virtual Texture getTexture() const = 0;

        virtual bool isOneTime() const = 0;

        virtual bool isApplicable(Player &player, World &world) const = 0;

        virtual void onApply(Player &player, World &world, Int32 duration) const = 0;

        virtual void onExpire(Player &player, World &world) const = 0;
    };

    class BonusType final {
    private:
        typedef std::unique_ptr<BonusTypeImpl> BonusTypeImplPtr;

    public:
        static const BonusType NONE;
        static const BonusType PLUS_LIFE;
        static const BonusType MINUS_LIFE;
        static const BonusType FULL_LIFE;
        static const BonusType FAST_RELOAD;
        static const BonusType POWERFUL_SHOTS;
        static const BonusType INVULNERABILITY;
        static const BonusType BULLETS;
        static const BonusType FAST_MOVEMENT;
        static const BonusType INVISIBILITY;
        static const BonusType SPLIT_FIRE;
        static const BonusType VAMPIRE_SHOTS;

    private:
        mutable BonusTypeImpl *impl;

        void assign(BonusTypeImplPtr &&impl) const;

    private:
        static std::vector<BonusType> types;
        static std::vector<BonusTypeImplPtr> implementations;

    public:
        BonusType();

        BonusType(const BonusType &bonusType);

        Texture getTexture() const;

        bool isOneTime() const;

        bool isApplicable(Player &player, World &world) const;

        void onApply(Player &player, World &world, Int32 duration) const;

        void onExpire(Player &player, World &world) const;

        bool operator==(const BonusType &bonus) const;

        bool operator!=(const BonusType &bonus) const;

    public:
        static const std::vector<BonusType> &values();

        static void initialize(const TextureList &textures);
    };

    class Bonus {
    private:
        BonusType bonus;
        Int32 duration;
        Vector position;
        Texture texture;

    public:
        Bonus(BonusType type, Int32 duration, const Vector &position, Texture texture);

        void render() const;

        const BonusType &getType() const {
            return bonus;
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

        Vector getSpritePosition() const {
            return position - Vector(0.2f, 0.2f);
        }
    };

    class LyingWeapon {
        Weapon weapon;
        Vector position;
        Int32 bullets;

    public:
        LyingWeapon(Weapon weapon, Int32 bullets, const Vector &position);

        void render() const;

        const Weapon &getWeapon() const {
            return weapon;
        }

        Int32 getBullets() const {
            return bullets;
        }

        const Vector &getPosition() const {
            return position;
        }

        Vector getDimensions() const {
            return Vector(1.0f, 1.0f);
        }

        Rectangle getCollisionRect() const {
            return Rectangle::fromCornerAndSize(getPosition(), getDimensions());
        }

        Vector getSpritePosition() const {
            return position;
        }
    };
}

#endif