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

#ifndef DUEL6_PLAYER_H
#define DUEL6_PLAYER_H

#include <memory>
#include <string>
#include <time.h>
#include "Camera.h"
#include "SpriteList.h"
#include "Person.h"
#include "PlayerSkin.h"
#include "PlayerControls.h"
#include "PlayerSounds.h"
#include "Orientation.h"
#include "ScreenMode.h"
#include "Bonus.h"
#include "Sound.h"
#include "Video.h"
#include "Water.h"
#include "Rectangle.h"
#include "Defines.h"
#include "Level.h"

namespace Duel6 {
    // Forward declarations
    class Elevator;

    class Shot;

    class Weapon;

    class World;

    class InfoMessageQueue;

    class PlayerEventListener;

    struct PlayerView {
    private:
        Int32 x;
        Int32 y;
        Int32 width;
        Int32 height;

    public:
        PlayerView() {}

        PlayerView(Int32 x, Int32 y, Int32 width, Int32 height)
                : x(x), y(y), width(width), height(height) {}

        Int32 getX() const {
            return x;
        }

        Int32 getY() const {
            return y;
        }

        Int32 getWidth() const {
            return width;
        }

        Int32 getHeight() const {
            return height;
        }
    };

    class Indicator {
    public:
        static constexpr Float32 DURATION = 2.0f;
        static constexpr Float32 FADE_DURATION = 0.5f;

    private:
        Float32 totalDuration;
        Float32 duration;

    public:
        Indicator()
                : totalDuration(0), duration(0) {}

        Indicator &setDuration(Float32 duration, bool keepFade = true) {
            Float32 startDuration = keepFade ? getAlpha() * FADE_DURATION : 0;
            this->totalDuration = startDuration + duration;
            this->duration = duration < FADE_DURATION ? 0 : startDuration;
            return *this;
        }

        Indicator &show(Float32 duration = DURATION) {
            return setDuration(duration);
        }

        Indicator &hide(bool keepFade = true) {
            return setDuration(0, keepFade);
        }

        bool isVisible() const {
            return totalDuration > 0.0f;
        }

        Float32 getAlpha() const {
            if (duration > totalDuration - FADE_DURATION) {
                return (totalDuration - duration) / FADE_DURATION;
            } else if (duration < FADE_DURATION) {
                return duration / FADE_DURATION;
            }
            return 1.0f;
        }

        Indicator &update(Float32 elapsedTime) {
            if (isVisible()) {
                duration += elapsedTime;
                if (duration > totalDuration) {
                    hide();
                }
            }
            return *this;
        }
    };

    struct PlayerIndicators {
    private:
        Indicator health;
        Indicator air;
        Indicator reload;
        Indicator name;
        Indicator bullets;
        Indicator bonus;

    public:
        PlayerIndicators &showAll(Float32 duration) {
            health.show(duration);
            air.show(duration);
            name.show(duration);
            bullets.show(duration);
            return *this;
        }

        PlayerIndicators &hideAll(bool keepFade) {
            health.hide(keepFade);
            air.hide(keepFade);
            reload.hide(keepFade);
            name.hide(keepFade);
            bullets.hide(keepFade);
            bonus.hide(keepFade);
            return *this;
        }

        PlayerIndicators &updateAll(Float32 elapsedTime) {
            health.update(elapsedTime);
            air.update(elapsedTime);
            reload.update(elapsedTime);
            name.update(elapsedTime);
            bullets.update(elapsedTime);
            bonus.update(elapsedTime);
            return *this;
        }

        Indicator &getHealth() {
            return health;
        }

        Indicator &getAir() {
            return air;
        }

        Indicator &getReload() {
            return reload;
        }

        Indicator &getName() {
            return name;
        }

        Indicator &getBullets() {
            return bullets;
        }

        Indicator &getBonus() {
            return bonus;
        }

        const Indicator &getHealth() const {
            return health;
        }

        const Indicator &getAir() const {
            return air;
        }

        const Indicator &getReload() const {
            return reload;
        }

        const Indicator &getName() const {
            return name;
        }

        const Indicator &getBullets() const {
            return bullets;
        }

        const Indicator &getBonus() const {
            return bonus;
        }
    };

    struct CameraPosition {
        Float32 Left;
        Float32 Right;
        Float32 Up;
        Float32 Down;
        Vector Pos;
        Float32 TolX;
        Float32 TolY;
    };

    class Player {
    private:
        enum Flags {
            FlagDead = 0x01,
            FlagPick = 0x02,
            FlagKnee = 0x04,
            FlagLying = 0x08,
            FlagMoveDown = 0x10,
            FlagMoveLeft = 0x40,
            FlagMoveRight = 0x80,
            FlagMoveUp = 0x100,
            FlagHasGun = 0x200,
            FlagGhost = 0x400,
            FlagDoubleJumpDebounce = 0x800,
            FlagDoubleJump = 0x1000,
        };

        struct WaterState {
            Water headUnderWater;
            Water feetInWater;
        };

    private:
        Person &person;
        PlayerSkin skin;
        Camera camera;
        CameraPosition cameraPos;
        const PlayerSounds &sounds;
        const PlayerControls &controls;
        PlayerView view;
        WaterState water;
        SpriteList::Iterator sprite;
        SpriteList::Iterator gunSprite;
        Uint32 flags;
        Orientation orientation;
        Vector position;
        Vector acceleration;
        Vector externalForces;
        Vector externalForcesSpeed;
        Vector velocity;
        Float32 life;
        Float32 air;
        Int32 ammo;
        BonusType bonus;
        Int32 roundKills;
        Float32 timeToReload;
        Float32 bonusRemainingTime;
        Float32 bonusDuration;
        Float32 timeSinceHit;
        Float32 timeStuckInWall;
        Float32 tempSkinDuration;
        Weapon weapon;
        const Elevator *elevator;
        PlayerEventListener *eventListener;
        World *world; // TODO: Remove
        Float32 bodyAlpha;
        clock_t roundStartTime;
        PlayerIndicators indicators;

    public:
        Player(Person &person, const PlayerSkin &skin, const PlayerSounds &sounds, const PlayerControls &controls);

        ~Player();

        bool is(const Player &player) const {
            return (this == &player);
        }

        Vector getVelocity() const;

        void startRound(World &world, Int32 startBlockX, Int32 startBlockY, Int32 ammo, const Weapon &weapon);

        void endRound();

        void setView(const PlayerView &view);

        void update(World &world, ScreenMode screenMode, Float32 elapsedTime);

        void prepareCam(const Video &video, ScreenMode screenMode, Int32 zoom, Int32 levelSizeX, Int32 levelSizeY);

        bool hit(Float32 pw); // Returns true if the shot caused the player to die
        bool hitByShot(Float32 pw, Shot &s, bool directHit, const Vector &hitPoint, const Vector &shotVector);

        bool airHit(Float32 amount);

        void processShot(Shot &shot, std::vector<Player *> &playersHit, std::vector<Player *> &playersKilled);

        void setEventListener(PlayerEventListener &listener) {
            eventListener = &listener;
        }

        const Vector &getPosition() const {
            return position;
        }

        Vector getDimensions() const {
            if (isKneeling()) {
                return Vector(1.0f, 0.8f);
            } else if (isLying()) {
                return Vector(1.0f, 0.45f);
            }
            return Vector(1.0f, 1.0f);
        }

        Vector getCentre() const {
            return getCollisionRect().getCentre();
        }

        Rectangle getCollisionRect() const {
            return Rectangle::fromCornerAndSize(getPosition(), getDimensions());
        }

        Vector getSpritePosition() const {
            return getPosition();
        }

        Float32 getGunVerticalPosition() const {
            return getPosition().y + getDimensions().y - 0.5f;
        }

        Vector getGunSpritePosition() const {
            return isKneeling() ? getSpritePosition() - Vector(0.0f, 0.15f) : getSpritePosition();
        }

        PlayerIndicators &getIndicators() {
            return indicators;
        }

        const PlayerIndicators &getIndicators() const {
            return indicators;
        }

        const PlayerView &getView() const {
            return view;
        }

        Person &getPerson() {
            return person;
        }

        const Person &getPerson() const {
            return person;
        }

        const Camera &getCamera() const {
            return camera;
        }

        const Weapon &getWeapon() const {
            return weapon;
        }

        Int32 getAmmo() const {
            return ammo;
        }

        Int32 getRoundKills() const {
            return roundKills;
        }

        void addRoundKills(Int32 kills) {
            roundKills += kills;
        }

        Orientation getOrientation() const {
            return orientation;
        }

        Player &setAlpha(Float32 alpha) {
            sprite->setAlpha(bodyAlpha * alpha);
            gunSprite->setAlpha(alpha);
            return *this;
        }

        Player &setBonus(BonusType type, Int32 duration) {
            if (type == bonus) {
                bonusRemainingTime += Float32(duration) / 2;
                bonusDuration += Float32(duration) / 2;
            } else {
                bonus.onExpire(*this, *world);
                bonus = type;
                bonusRemainingTime = Float32(duration);
                bonusDuration = Float32(duration);
            }
            bonus.onApply(*this, *world, Int32(bonusDuration));
            indicators.getBonus().show(bonusDuration);
            return *this;
        }

        Float32 getLife() const {
            return life;
        }

        Float32 getAir() const {
            return air;
        }

        Float32 getReloadInterval() const;

        Float32 getReloadTime() const {
            return timeToReload;
        }

        BonusType getBonus() const {
            return bonus;
        }

        Float32 getBonusRemainingTime() const {
            return bonusRemainingTime;
        }

        Float32 getBonusDuration() const {
            return bonusDuration;
        }

        Player &addLife(Float32 life, bool showHpBar = true);

        Player &setFullLife() {
            addLife(D6_MAX_LIFE);
            return *this;
        }

        Player &pickAmmo(Int32 ammo) {
            this->ammo += ammo;
            indicators.getBullets().show();
            return *this;
        }

        void useTemporarySkin(PlayerSkin &tempSkin);

        Player &pickWeapon(Weapon weapon, Int32 bullets);

        bool isReloading() {
            return timeToReload > 0;
        }

        bool isKneeling() const {
            return hasFlag(FlagKnee);
        }

        bool isLying() const {
            return hasFlag(FlagLying);
        }

        bool isAlive() const {
            return !hasFlag(FlagDead);
        }

        bool isGhost() const {
            return hasFlag(FlagGhost);
        }

        void makeGhost() {
            setFlag(FlagGhost);
            setAlpha(0.1f);
        }

        bool isUnderWater() const {
            return water.headUnderWater != Water::NONE;
        }

        bool isPickingGun() const {
            return hasFlag(FlagPick);
        }

        bool hasGun() const {
            return hasFlag(FlagHasGun);
        }

        bool isInGame() const {
            return isAlive() || isLying();
        }

        bool isInvulnerable() const {
            return (getBonus() == BonusType::INVULNERABILITY);
        }

        bool isRising() const {
            return velocity.y > 0;
        }

        bool isFalling() const {
            return velocity.y < 0;
        }

        bool isOnGround() const;

        bool isMoving() const {
            return (velocity.x != 0.0f);
        }

        bool hasPowerfulShots() const {
            return (getBonus() == BonusType::POWERFUL_SHOTS);
        }

        bool hasFastReload() const {
            return (getBonus() == BonusType::FAST_RELOAD);
        }

        bool isOnElevator() const {
            return (elevator != nullptr);
        }

        const PlayerSkin &getSkin() const {
            return skin;
        }

        const WaterState &getWaterState() const {
            return water;
        }

        void playSound(PlayerSounds::Type type) const {
            sounds.getRandomSample(type).play();
        }

        void setBodyAlpha(Float32 alpha) {
            bodyAlpha = alpha;
            setAlpha(1.0f);
        }

        void removeBody() {
            unsetFlag(FlagLying);
        }

        void die();

    private:
        void makeMove(const Level &level, Float32 elapsedTime);

        void moveHorizontal(const Level &level, Float32 elapsedTime, Float32 speed);

        void moveVertical(const Level &level, Float32 elapsedTime, Float32 speed);

        void checkKeys();

        void checkWater(World &world, Float32 elapsedTime);

        void fall();

        void pick();

        void shoot();

        void setAnm();

        void unstuck();

        void updateCam(Int32 levelSizeX, Int32 levelSizeY);

        void switchToOriginalSkin();

        void dropWeapon(const Level &level);

        Float32 getSpeed() const;

        void checkElevator(Float32 speedFactor);

        void checkStuck(const Level &level, Float32 elapsedTime);

        bool hasFlag(Uint32 flag) const {
            return (flags & flag) == flag;
        }

        void setFlag(Uint32 flag) {
            flags |= flag;
        }

        void unsetFlag(Uint32 flag) {
            flags &= ~flag;
        }
    };
}

#endif
