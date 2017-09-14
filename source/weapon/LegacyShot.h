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

#ifndef DUEL6_LEGACYSHOT_H
#define DUEL6_LEGACYSHOT_H

#include "../Weapon.h"
#include "../Vector.h"
#include "../SpriteList.h"
#include "../Player.h"
#include "../ShotList.h"
#include "../GameSettings.h"
#include "LegacyWeapon.h"
#include "ShotBase.h"

namespace Duel6 {
    class LegacyShot : public ShotBase {
    private:
        const LegacyWeapon::Definition &definition;
        const LegacyWeapon::WeaponTextures &textures;
        const LegacyWeapon::WeaponSamples &samples;
        Rectangle collisionRect;
        Orientation orientation;
        Vector position;
        Vector velocity;
        SpriteList::Iterator sprite;
        bool powerful;
        Shot *hitByOtherShot;

    public:
        LegacyShot(Player &player, const LegacyWeapon &weapon, Orientation orientation, SpriteList::Iterator sprite,
                   const Rectangle &collisionRect);

        bool update(Float32 elapsedTime, World &world) override;

        Rectangle getCollisionRect() const override {
            return Rectangle::fromCornerAndSize(getPosition(), getDimensions());
        }

        bool requestCollision(const ShotHit &hit) override;

        void onHitPlayer(Player &player, bool directHit, const Vector &hitPoint, World &world) override;

        void onKillPlayer(Player &player, bool directHit, const Vector &hitPoint, World &world) override;

    protected:
        virtual void onExplode(const Vector &centre, Float32 range, World &world);

        const Vector &getPosition() const {
            return position;
        }

        Vector getDimensions() const {
            return collisionRect.getSize();
        }

        Vector getCentre() const {
            return getCollisionRect().getCentre();
        }

        Vector getSpritePosition() const {
            if (orientation == Orientation::Left) {
                return getPosition() - collisionRect.left;
            } else {
                return Vector(getPosition().x + collisionRect.right.x - 1.0f, getPosition().y - collisionRect.left.y);
            }
        }

        void move(Float32 elapsedTime);

        Float32 getExplosionRange() const;

        Float32 getExplosionPower() const;

        ShotHit evaluateShotHit(World &world);

        ShotHit checkPlayerCollision(std::vector<Player> &players);

        ShotHit checkWorldCollision(const Level &level);

        ShotHit checkShotCollision(ShotList &shotList, ShotCollisionSetting collisionSetting);

        void explode(ShotHit hit, World &world);

        void addPlayerExplosion(Player &player, World &world);

        void addPlayerBlood(const Player &player, const Vector &point, World &world);

    };
}

#endif
