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

#include "../World.h"
#include "../collision/Collision.h"
#include "LegacyShot.h"

namespace Duel6 {
    LegacyShot::LegacyShot(Player &player, const LegacyWeapon &weapon, Orientation orientation,
                           SpriteList::Iterator sprite, const Rectangle &collisionRect)
            : ShotBase(player.getWeapon(), player), definition(weapon.getDefinition()), textures(weapon.getTextures()),
              samples(weapon.getSamples()), collisionRect(collisionRect), orientation(orientation), sprite(sprite),
              hitByOtherShot(nullptr) {
        const Vector dim = getDimensions();
        const Rectangle playerRect = player.getCollisionRect();
        if (orientation == Orientation::Left) {
            position = Vector(playerRect.left.x - dim.x, player.getGunVerticalPosition() - dim.y / 2.0f);
            velocity = Vector(-1.0f, 0.0f);
        } else {
            position = Vector(playerRect.right.x, player.getGunVerticalPosition() - dim.y / 2.0f);
            velocity = Vector(1.0f, 0.0f);
        }
        this->sprite->setPosition(getSpritePosition(), 0.6f).setOrientation(this->orientation);
        powerful = getPlayer().hasPowerfulShots();
    }

    void LegacyShot::move(Float32 elapsedTime) {
        position += velocity * definition.bulletSpeed * elapsedTime;
        sprite->setPosition(getSpritePosition());
    }

    Float32 LegacyShot::getExplosionPower() const {
        Float32 coef = (powerful ? 2.0f : 1.0f);
        return definition.power * coef;
    }

    Float32 LegacyShot::getExplosionRange() const {
        Float32 coef = (powerful ? 2.0f : 1.0f);
        return definition.boom * coef;
    }

    bool LegacyShot::update(Float32 elapsedTime, World &world) {
        move(elapsedTime);

        ShotHit hit = evaluateShotHit(world);

        if (hit.hit) {
            explode(hit, world);

            Sprite boom(definition.boomAnimation, textures.boom);
            boom.setPosition(getCentre() - Vector(0.5f, 0.5f), 0.6f)
                    .setSpeed(2.0f)
                    .setLooping(AnimationLooping::OnceAndRemove)
                    .setOrientation(orientation)
                    .setAlpha(0.6f)
                    .setGrow(definition.expGrow * (powerful ? 1.2f : 1.0f));

            SpriteList::Iterator boomSprite = world.getSpriteList().addSprite(boom);
            samples.boom.play();
            if (definition.boom > 0) {
                boomSprite->setNoDepth(true);
            }

            world.getSpriteList().removeSprite(sprite);
            return false;
        }
        return true;
    }

    bool LegacyShot::requestCollision(const ShotHit &hit) {
        if (hit.shot != nullptr) {
            hitByOtherShot = hit.shot;
            return true;
        }
        return false;
    }

    void LegacyShot::explode(ShotHit hit, World &world) {
        std::vector<Player *> killedPlayers;
        std::vector<Player *> hittedPlayers;

        Player &author = getPlayer();

        Float32 range = getExplosionRange();
        Float32 power = getExplosionPower();

        const Vector shotCentre = getCentre();
        onExplode(shotCentre, range, world);

        for (Player &player : world.getPlayers()) {
            bool directHit = (hit.player != nullptr && player.is(*hit.player));
            Vector playerCentre = player.getCentre();
            Float32 dist = directHit ? 0 : (playerCentre - shotCentre).length();

            if (directHit || dist < range) {
                hittedPlayers.push_back(&player);
                if (player.hitByShot(directHit ? power : ((range - dist) * power) / range, *this, directHit,
                                     getCentre(), velocity)) {
                    killedPlayers.push_back(&player);
                }
            }
        }

        author.processShot(*this, hittedPlayers, killedPlayers);
    }

    void LegacyShot::onExplode(const Vector &centre, Float32 range, World &world) {
        world.getFireList().check(centre, range);
    }

    void LegacyShot::onHitPlayer(Player &player, bool directHit, const Vector &hitPoint, World &world) {
        if (directHit && definition.blood) {
            addPlayerBlood(player, hitPoint, world);
        }

        if (directHit && definition.explodes && !player.isAlive()) {
            addPlayerExplosion(player, world);
        }
    }

    void LegacyShot::onKillPlayer(Player &player, bool directHit, const Vector &hitPoint, World &world) {
        if (directHit && definition.blood) {
            addPlayerBlood(player, hitPoint, world);
        }

        if (directHit && definition.explodes) {
            addPlayerExplosion(player, world);
        }
    }

    ShotHit LegacyShot::evaluateShotHit(World &world) {
        if (hitByOtherShot != nullptr) {
            return {true, nullptr, hitByOtherShot};
        }

        ShotHit hit = checkPlayerCollision(world.getPlayers());
        if (!hit.hit) {
            hit = checkWorldCollision(world.getLevel());
        }
        if (!hit.hit) {
            hit = checkShotCollision(world.getShotList(), world.getGameSettings().getShotCollision());
        }
        return hit;
    }

    ShotHit LegacyShot::checkPlayerCollision(std::vector<Player> &players) {
        const Rectangle shotBox = getCollisionRect();

        for (Player &player : players) {
            if (!player.isInGame() || player.getBonus() == BonusType::INVISIBILITY || player.is(getPlayer())) {
                continue;
            }

            if (Collision::rectangles(player.getCollisionRect(), shotBox)) {
                return {true, &player, nullptr};
            }
        }

        return {false, nullptr, nullptr};
    }

    ShotHit LegacyShot::checkWorldCollision(const Level &level) {
        const Rectangle box = getCollisionRect();
        Float32 up = box.right.y;
        Float32 down = box.left.y;
        Float32 left = box.left.x;
        Float32 right = box.right.x;

        bool hitsWall = level.isWall(left, up, true) ||
                        level.isWall(left, down, true) ||
                        level.isWall(right, up, true) ||
                        level.isWall(right, down, true);

        return {hitsWall, nullptr, nullptr};
    }

    ShotHit LegacyShot::checkShotCollision(ShotList &shotList, ShotCollisionSetting collisionSetting) {
        ShotHit hit = {false, nullptr, nullptr};

        if (collisionSetting == ShotCollisionSetting::All ||
            (collisionSetting == ShotCollisionSetting::Large && definition.collides)) {
            shotList.forEach([this, &hit](Shot &otherShot) -> bool {
                if (this != &otherShot &&
                    Collision::rectangles(this->getCollisionRect(), otherShot.getCollisionRect())) {
                    if (otherShot.requestCollision({true, nullptr, this})) {
                        hit.hit = true;
                        hit.shot = &otherShot;
                        return false;
                    }
                }
                return true;
            });
        }
        return hit;
    }

    void LegacyShot::addPlayerExplosion(Player &player, World &world) {
        player.removeBody();
        world.getExplosionList().add(player.getCentre(), 0.5f, 1.2f, definition.explosionColor);
    }

    void LegacyShot::addPlayerBlood(const Player &player, const Vector &point, World &world) {
        Rectangle rect = player.getCollisionRect();
        world.getExplosionList().add(
                Vector(rect.left.x + (0.3f + (Math::random(40)) * 0.01f) * rect.getSize().x, point.y), 0.2f, 0.5f,
                Color::RED);
    }
}