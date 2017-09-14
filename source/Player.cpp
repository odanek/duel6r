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

#include <algorithm>
#include <queue>
#include "Sound.h"
#include "TextureManager.h"
#include "InfoMessageQueue.h"
#include "World.h"
#include "Player.h"
#include "BonusList.h"
#include "Weapon.h"
#include "ElevatorList.h"
#include "Explosion.h"
#include "Math.h"
#include "Video.h"
#include "PlayerEventListener.h"

namespace Duel6 {
    static Int16 noAnim[4] = {0, 50, 0, -1};
    static Int16 d6SAnim[22] = {0, 200, 1, 30, 0, 30, 2, 30, 0, 90, 3, 15, 4, 15, 3, 15, 4, 15, 3, 30, -1, 0};
    static Int16 d6WAnim[10] = {6, 5, 5, 5, 6, 5, 7, 5, -1, 0};
    static Int16 d6JAnim[4] = {8, 50, -1, 0};
    static Int16 d6DAnim[18] = {9, 300, 10, 60, 9, 60, 11, 60, 9, 150, 12, 60, 9, 60, 12, 60, -1, 0};
    static Int16 d6LAnim[16] = {13, 10, 14, 10, 15, 10, 16, 10, 17, 10, 18, 10, 19, 10, -1, 0};
    static Int16 d6NAnim[4] = {25, 100, -1, 0};
    static Int16 d6PAnim[] = {0, 10, 20, 10, 21, 10, 22, 10, 23, 10, 24, 10, 23, 10, 22, 10, 21, 10, 0, 10, -1, 0};

    //TODO: This still needs further fine-tuning for good jumping experience
    static const float GRAVITATIONAL_ACCELERATION = -11.0f;
    static const float JUMP_FACTOR = -0.7f;
    static const float JUMP_ACCELERATION = JUMP_FACTOR * GRAVITATIONAL_ACCELERATION;

    // Very important fun aspect!
    static const float SHOT_FORCE_FACTOR = 0.05f;

    // collision detection
    static const float FLOOR_DISTANCE_THRESHOLD = 0.0001f;
    static const float DELTA = 0.30f;
    static const float VERTICAL_DELTA = 0.5f * DELTA;
    static const float HORIZONTAL_DELTA = 0.25f * DELTA;
    static const float DELTA_HEIGHT = 0.94f;

    Player::Player(Person &person, const PlayerSkin &skin, const PlayerSounds &sounds, const PlayerControls &controls)
            : person(person), skin(skin), sounds(sounds), controls(controls), bodyAlpha(1.0f) {
        camera.rotate(180.0, 0.0, 0.0);
    }

    Player::~Player() {
    }

    void Player::startRound(World &world, Int32 startBlockX, Int32 startBlockY, Int32 ammo, const Weapon &weapon) {
        this->world = &world;
        position = Vector(Float32(startBlockX), Float32(startBlockY) + 0.0001f);
        velocity = Vector(0, 0);
        acceleration = Vector(0, 0);
        Sprite manSprite(noAnim, skin.getTextureList());
        manSprite.setPosition(getSpritePosition(), 0.5f);
        sprite = world.getSpriteList().addSprite(manSprite);

        this->weapon = weapon;
        Sprite gunSprite;
        weapon.makeSprite(gunSprite);
        gunSprite.setPosition(getGunSpritePosition(), 0.5f);
        this->gunSprite = world.getSpriteList().addSprite(gunSprite);

        flags = FlagHasGun;
        orientation = Orientation::Left;
        timeToReload = 0;
        life = D6_MAX_LIFE;
        air = D6_MAX_AIR;
        this->ammo = ammo;
        elevator = nullptr;
        bonus = BonusType::INVULNERABILITY;
        bonusDuration = 2.0f;
        bonusRemainingTime = 2.0f;
        tempSkinDuration = 0;
        roundKills = 0;
        this->view = view;
        water.headUnderWater = Water::NONE;
        water.feetInWater = Water::NONE;
        externalForces = {0, 0};
        externalForcesSpeed = {0, 0};

        indicators.hideAll(false);
        indicators.getName().show(4.0f);
        indicators.getBonus().show(bonusDuration);
        indicators.getBullets().show(4.0f);

        roundStartTime = clock();
        getPerson().addGames(1);
    }

    void Player::endRound() {
        Int32 gameTime = Int32((clock() - roundStartTime) / CLOCKS_PER_SEC);
        getPerson().addTotalGameTime(gameTime);
        if (isAlive()) {
            getPerson().addTimeAlive(gameTime);
        }
    }

    void Player::setView(const PlayerView &view) {
        this->view = view;
    }

    bool Player::isOnGround() const {
        // enables jumping sideways out of places with no space above player's head
        Float32 delta = 0.8f * VERTICAL_DELTA;
        Float32 down = getPosition().y - FLOOR_DISTANCE_THRESHOLD;
        Float32 left = getPosition().x + delta;
        Float32 right = getPosition().x + (1 - delta);

        return world->getLevel().isWall(left, down, true) || world->getLevel().isWall(right, down, true);
    }

    void Player::moveHorizontal(const Level &level, Float32 elapsedTime, Float32 speed) {
        if (hasFlag(FlagMoveLeft)) {
            if (this->velocity.x > -D6_PLAYER_MAX_SPEED) {
                this->velocity.x -= elapsedTime;
            }
            if (this->velocity.x < 0.0f) {
                orientation = Orientation::Left;
            }
        } else if (this->velocity.x < 0.0f) {
            this->velocity.x = std::min(this->velocity.x + elapsedTime, 0.0f);
        }

        if (hasFlag(FlagMoveRight)) {
            if (this->velocity.x < D6_PLAYER_MAX_SPEED) {
                this->velocity.x += elapsedTime;
            }

            if (this->velocity.x > 0.0f) {
                orientation = Orientation::Right;
            }
        } else if (this->velocity.x > 0.0f) {
            this->velocity.x = std::max(this->velocity.x - elapsedTime, 0.0f);
        }
    }

    void Player::moveVertical(const Level &level, Float32 elapsedTime, Float32 speed) {
        checkElevator(speed);
        if (hasFlag(FlagMoveUp) && this->velocity.y <= 0 && (isOnGround() || isOnElevator())) {
            const Float32 delta = VERTICAL_DELTA;
            Float32 up = getPosition().y + 0.94f;
            Float32 left = getPosition().x + delta;
            Float32 right = getPosition().x + (1.0f - delta);

            if (!level.isWall(left, up, true) && !level.isWall(right, up, true)) {
                acceleration.y += JUMP_ACCELERATION;
            }

            if (isOnElevator()) { //leaving elevator
                acceleration.y += elevator->getAcceleratedVelocity().y;
            }

        }
        if (hasFlag(FlagDoubleJump) && getBonus() == BonusType::FAST_MOVEMENT) {
            if (this->velocity.y > 0.0) {
                acceleration.y += JUMP_ACCELERATION;
            }
        }

        if (!isOnElevator()) {
            this->velocity.y += GRAVITATIONAL_ACCELERATION * elapsedTime;    // gravity
        }

        if (!isOnGround()) {
            if (this->velocity.y < -D6_PLAYER_JUMP_SPEED) {
                this->velocity.y = -D6_PLAYER_JUMP_SPEED;
            }
        }
    }

    void Player::fall() {
        if (isOnGround() || isOnElevator()) {
            setFlag(FlagKnee);
        } else {
            if (isRising()) {
                velocity.y = 0.0f; //TODO Accelerate
            }
        }
    }

    void Player::pick() {
        if (isOnGround() && !isMoving() && !isOnElevator()) {
            world->getBonusList().checkWeapon(*this);
        }
    }

    void Player::shoot() {
        if (!getAmmo() || isReloading() || !hasGun())
            return;

        timeToReload = getReloadInterval();

        indicators.getBullets().show();
        indicators.getReload().show(timeToReload + Indicator::FADE_DURATION);

        ammo--;
        gunSprite->setFrame(0);
        getPerson().addShots(1);
        Orientation originalOrientation = getOrientation();

        getWeapon().shoot(*this, originalOrientation, *world);

        if (getBonus() == BonusType::SPLIT_FIRE && getAmmo() > 0) {
            ammo--;
            getPerson().addShots(1);
            Orientation secondaryOrientation =
                    originalOrientation == Orientation::Left ? Orientation::Right : Orientation::Left;
            getWeapon().shoot(*this, secondaryOrientation, *world);
        }
    }

    Player &Player::pickWeapon(Weapon weapon, Int32 bullets) {
        setFlag(FlagPick);
        this->weapon = weapon;
        ammo = bullets;
        timeToReload = 0;
        indicators.getReload().hide();
        indicators.getBullets().show();
        weapon.makeSprite(*gunSprite);
        return *this;
    }

    void Player::makeMove(const Level &level, Float32 elapsedTime) {
        static bool bleft = false, bright = false, bup = false, bdown = false;
        Float32 speed = getSpeed() * elapsedTime;

        bool onElevator = isOnElevator();
        if (onElevator && level.isWall(Int32(getCentre().x), Int32(getCentre().y), false)) {
            this->velocity.x = 0;
        }
        elevator = nullptr;
        acceleration.x = 0;
        acceleration.y = 0;

        moveVertical(level, elapsedTime, speed);
        moveHorizontal(level, elapsedTime, speed);

        externalForcesSpeed += externalForces;
        externalForces.x = 0;
        externalForces.y = 0;
        if (externalForcesSpeed.length() < 0.01) {
            externalForcesSpeed.x = 0;
            externalForcesSpeed.y = 0;
        }
        //do not multiply by speed or elapsedTime !!!
        this->velocity += acceleration;

        // horizontal speed clamping
        if (fabs(this->velocity.x * speed) > 0.5f) {
            this->velocity.x = std::copysign(0.5f, this->velocity.x) / speed;
        }
        if (fabs(this->externalForcesSpeed.x * speed) > 0.5f) {
            this->externalForcesSpeed.x = std::copysign(0.5f, this->externalForcesSpeed.x) / speed;
        }
        Vector totalSpeed = this->velocity + externalForcesSpeed;
        bleft = false, bright = false, bup = false, bdown = false;

        //collision detection here we go

        {
            Float32 delta = VERTICAL_DELTA;
            Float32 up;
            Float32 down;
            Float32 left;
            Float32 right;

            /**
             * Down
             */
            up = getPosition().y + DELTA_HEIGHT + totalSpeed.y * speed;
            down = getPosition().y + totalSpeed.y * speed;
            left = getPosition().x + delta + totalSpeed.x * speed;
            right = getPosition().x + (1.0f - delta) + totalSpeed.x * speed;
            if (level.isWall(right, down, true) || level.isWall(left, down, true)) {
                bdown = true;
                this->velocity.y = 0;
                totalSpeed.y = 0;
                if (!(level.isWall(right, up, true) || level.isWall(left, down, true))) {
                    this->velocity.y = -0.001;
                    totalSpeed.y = -0.001;
                }
            }

            /**
             * Up
             */
            up = getPosition().y + DELTA_HEIGHT + totalSpeed.y * speed;
            down = getPosition().y + totalSpeed.y * speed;
            left = getPosition().x + delta + totalSpeed.x * speed;
            right = getPosition().x + (1.0f - delta) + totalSpeed.x * speed;

            if (totalSpeed.y > 0.0f && (level.isWall(right, up, true) || level.isWall(left, up, true))) {
                bup = true;
                totalSpeed.y = 0;
                this->velocity.y = 0;
            }

            /**
             * Right
             */
            delta = HORIZONTAL_DELTA;
            left = getPosition().x + delta + totalSpeed.x * speed;
            down = getPosition().y + 0.1f;
            if (totalSpeed.y > 0) {
                up = getPosition().y + DELTA_HEIGHT + totalSpeed.y * speed;
            } else {
                up = getPosition().y + DELTA_HEIGHT;
            }
            right = getPosition().x + (1.0f - delta) + totalSpeed.x * speed;

            if (totalSpeed.x > 0 && (floorf(right) > floorf(getPosition().x)) &&
                ((level.isWall(right, up, true) || level.isWall(right, down, true)))) {
                bright = true;
            }

            /**
             * Left
             */
            up = getPosition().y + DELTA_HEIGHT;
            down = getPosition().y + 0.1f;
            left = getPosition().x + delta + totalSpeed.x * speed;
            right = getPosition().x + (1.0f - delta) + totalSpeed.x * speed;
            if (totalSpeed.y > 0) {
                up = getPosition().y + DELTA_HEIGHT + totalSpeed.y * speed;
            } else {
                up = getPosition().y + DELTA_HEIGHT;
            }
            if (level.isWall(left, up, true) || level.isWall(left, down, true) || (left < 0)) {
                bleft = true;
            }

            /**
             * Put it all together
             */
            if (!onElevator) {
                if (bdown) {
                    this->position.y = ceil(this->position.y - 0.5f);
                } else if (bup) {
                    this->position.y = floor(this->position.y + 0.5f);
                }

                if (bleft) {
                    this->position.x = ceil(this->position.x + totalSpeed.x * speed) - delta;
                } else if (bright) {
                    this->position.x = floorf(this->position.x) + delta;
                }
            }
            if (bleft || bright) {
                externalForcesSpeed.x *= 0.5;
            }
            if (bup || bdown) {
                externalForcesSpeed.y *= 0.5;
            }
            externalForcesSpeed.x *= 0.9;
            externalForcesSpeed.y *= 0.9;
            position.x = std::max(-0.1f, std::min(position.x, level.getWidth() - 0.9f));
        }

        if (isOnElevator()) {
            position += elevator->getAcceleratedVelocity() * elapsedTime;
        }

        if (isPickingGun() && sprite->getAnimation() == d6PAnim && sprite->isFinished()) {
            unsetFlag(FlagPick);
        }
        if (isKneeling()) {
            unsetFlag(FlagMoveLeft | FlagMoveRight);
        }

        position.y += totalSpeed.y * speed; // the speed has the elapsedTime already factored in
        position.x += totalSpeed.x * D6_PLAYER_ACCEL * speed; // the speed has the elapsedTime already factored in
    }

    Vector Player::getVelocity() const {
        return velocity;
    }

    void Player::checkStuck(const Level &level, Float32 elapsedTime) {
        if (level.isWall(Int32(getCentre().x), Int32(getCentre().y), true)) {
            timeStuckInWall += elapsedTime;
            if (timeStuckInWall > D6_PLAYER_STUCK) {
                unstuck();
            }
        } else {
            timeStuckInWall = 0;
        }
    }

    void Player::unstuck() {
        Level::StartingPositionList startingPositions;
        world->getLevel().findStartingPositions(startingPositions);

        Level::StartingPosition &newPosition = startingPositions.front();
        position.x = Float32(newPosition.first);
        position.y = Float32(newPosition.second) + 0.0001f;
        timeStuckInWall = 0;
    }

    Float32 Player::getSpeed() const {
        Float32 spd = 1.0f;

        if (isUnderWater()) {
            spd *= 0.67f;
        }

        if (tempSkinDuration) {
            spd *= 0.5f;
        }

        if (getBonus() == BonusType::FAST_MOVEMENT) {
            spd *= 1.43f;
        } else {
            spd *= (1.4f - (getLife() / 250));
        }

        return spd;
    }

    Float32 Player::getReloadInterval() const {
        Float32 coef = hasFastReload() ? 2.0f : 1.0f;
        return getWeapon().getReloadInterval() / coef;
    }

    void Player::checkKeys() {
        if (!isAlive() && !isGhost()) {
            return;
        }

        if (controls.getStatus().isPressed()) {
            indicators.showAll(5.0f);
        }

        if (isPickingGun()) {
            return;
        }

        if (!isKneeling()) {
            if (controls.getLeft().isPressed()) {
                setFlag(FlagMoveLeft);
            } else {
                unsetFlag(FlagMoveLeft);
            }

            if (controls.getRight().isPressed()) {
                setFlag(FlagMoveRight);
            } else {
                unsetFlag(FlagMoveRight);
            }

            if (controls.getUp().isPressed()) {
                unsetFlag(FlagDoubleJump);
                if (hasFlag(FlagDoubleJumpDebounce) && !isOnGround() && !isOnElevator() && !hasFlag(FlagMoveUp)) {
                    setFlag(FlagDoubleJump);
                    unsetFlag(FlagDoubleJumpDebounce);
                }
                setFlag(FlagMoveUp);
            } else {
                if (isOnGround() || isOnElevator()) {
                    setFlag(FlagDoubleJumpDebounce);
                }
                unsetFlag(FlagDoubleJump);
                unsetFlag(FlagMoveUp);
            }

            if (controls.getPick().isPressed()) {
                pick();
            }
        }

        if (!isGhost() && controls.getShoot().isPressed()) {
            shoot();
        }

        unsetFlag(FlagKnee);

        if (controls.getDown().isPressed()) {
            fall();
        }
    }

    void Player::update(World &world, ScreenMode screenMode, Float32 elapsedTime) {
        checkWater(world, elapsedTime);
        if (isAlive()) {
            world.getBonusList().checkBonus(*this);
        }

        checkKeys();
        makeMove(world.getLevel(), elapsedTime);
        setAnm();

        // Drop gun if still has it and died
        if (isLying() && hasGun() && isOnGround()) {
            setBonus(BonusType::NONE, 0);
            dropWeapon(world.getLevel());
            unsetFlag(FlagHasGun);
        }

        // Move intervals
        indicators.updateAll(elapsedTime);
        if (isReloading()) {
            timeToReload = std::max(0.0f, getReloadTime() - elapsedTime);
        }

        if (getBonusRemainingTime() > 0) {
            if ((bonusRemainingTime -= elapsedTime) <= 0) {
                setBonus(BonusType::NONE, 0);
            }
        }

        if (getLife() < D6_MAX_LIFE && getLife() > 1 && timeSinceHit > D6_PLAYER_HPREGEN_DELAY) {
            addLife(elapsedTime * getRoundKills() * 0.8f, false);
        }

        if (tempSkinDuration > 0) {
            if ((tempSkinDuration -= elapsedTime) <= 0) {
                switchToOriginalSkin();
            }
        }

        if (screenMode == ScreenMode::SplitScreen) {
            updateCam(world.getLevel().getWidth(), world.getLevel().getHeight());
        }

        timeSinceHit += elapsedTime;

        checkStuck(world.getLevel(), elapsedTime);
    }

    void Player::setAnm() {
        Int16 *animation;

        if (!isAlive() && !isGhost()) {
            if (isLying()) {
                animation = d6LAnim;
            } else {
                animation = d6NAnim;
            }
        } else if (isPickingGun()) {
            animation = d6PAnim;
        } else if (isKneeling()) {
            animation = d6DAnim;
        } else if (isOnElevator()) {
            if (!isMoving()) {
                animation = d6SAnim;
            } else {
                animation = d6WAnim;
            }
        } else if (!isOnGround()) {
            animation = d6JAnim;
        } else if (!isMoving()) {
            animation = d6SAnim;
        } else {
            animation = d6WAnim;
        }

        sprite->setPosition(getSpritePosition())
                .setOrientation(getOrientation())
                .setAnimation(animation);

        gunSprite->setPosition(getGunSpritePosition())
                .setOrientation(getOrientation())
                .setDraw(isAlive() && !isPickingGun());
    }

    void Player::prepareCam(const Video &video, ScreenMode screenMode, Int32 zoom, Int32 levelSizeX, Int32 levelSizeY) {
        Float32 fovX, fovY, mZ, dX = 0.0, dY = 0.0;
        fovY = Math::degTan(video.getView().getFieldOfView() / 2.0f);
        fovX = video.getScreen().getAspect() * fovY;

        if (screenMode == ScreenMode::FullScreen) {
            if (levelSizeX > video.getScreen().getAspect() * levelSizeY)
                dX = (Float32) levelSizeX;
            else
                dY = (Float32) levelSizeY;
        } else {
            dX = (Float32) std::min(zoom, std::max(levelSizeX, levelSizeY));
        }

        if (dX == 0.0) {
            mZ = dY / (2.0f * fovY);
            dX = 2.0f * fovX * mZ;
        } else {
            mZ = dX / (2.0f * fovX);
            dY = 2.0f * fovY * mZ;
        }

        cameraPos.Pos.x = levelSizeX / 2.0f;
        cameraPos.Pos.y = levelSizeY / 2.0f;
        cameraPos.Pos.z = mZ + 1.0f;
        cameraPos.Left = cameraPos.Pos.x - dX / 2.0f;
        cameraPos.Right = cameraPos.Pos.x + dX / 2.0f;
        cameraPos.Down = cameraPos.Pos.y - dY / 2.0f;
        cameraPos.Up = cameraPos.Pos.y + dY / 2.0f;
        cameraPos.TolX = (dX * D6_CAM_TOLPER_X) / 200.0f;
        cameraPos.TolY = (dY * D6_CAM_TOLPER_Y) / 200.0f;

        camera.setPosition(cameraPos.Pos);

        if (screenMode == ScreenMode::SplitScreen) {
            updateCam(levelSizeX, levelSizeY);
        }
    }

    void Player::updateCam(Int32 levelSizeX, Int32 levelSizeY) {
        Float32 mX = 0.0, mY = 0.0;
        Vector pos = getCentre();

        if (pos.x < cameraPos.Pos.x - cameraPos.TolX) {
            mX = pos.x - (cameraPos.Pos.x - cameraPos.TolX);
            if (cameraPos.Left + mX < 0.0f)
                mX = -cameraPos.Left;
        } else if (pos.x > cameraPos.Pos.x + cameraPos.TolX) {
            mX = pos.x - (cameraPos.Pos.x + cameraPos.TolX);
            if (cameraPos.Right + mX > (Float32) levelSizeX)
                mX = (Float32) levelSizeX - cameraPos.Right;
        }
        if (pos.y < cameraPos.Pos.y - cameraPos.TolY) {
            mY = pos.y - (cameraPos.Pos.y - cameraPos.TolY);
            if (cameraPos.Down + mY < 0.0f)
                mY = -cameraPos.Down;
        } else if (pos.y > cameraPos.Pos.y + cameraPos.TolY) {
            mY = pos.y - (cameraPos.Pos.y + cameraPos.TolY);
            if (cameraPos.Up + mY > (Float32) levelSizeY)
                mY = (Float32) levelSizeY - cameraPos.Up;
        }

        if (mX != 0.0) {
            cameraPos.Left += mX;
            cameraPos.Right += mX;
            cameraPos.Pos.x += mX;
        }
        if (mY != 0.0) {
            cameraPos.Up += mY;
            cameraPos.Down += mY;
            cameraPos.Pos.y += mY;
        }
        if (mX != 0.0 || mY != 0.0) {
            camera.setPosition(cameraPos.Pos);
        }
    }

    bool
    Player::hitByShot(Float32 amount, Shot &shot, bool directHit, const Vector &hitPoint, const Vector &shotVector) {
        Vector estimatedShotVector = getCentre() - hitPoint;
        if (directHit) {
            estimatedShotVector.x = shotVector.x;    //makes sure that close-up shots have the right effect
        }
        externalForces += estimatedShotVector.unit() * amount * SHOT_FORCE_FACTOR;
        if (isInvulnerable() || !isInGame()) {
            return false;
        } else if (!isAlive()) {
            shot.onHitPlayer(*this, directHit, hitPoint, *world);
            return false;
        }

        Player &shootingPlayer = shot.getPlayer();
        Person &shootingPerson = shootingPlayer.getPerson();

        if (!eventListener->onDamageByShot(*this, shootingPlayer, amount, shot, directHit)) {
            return false;
        }

        timeSinceHit = 0;

        if (directHit) {
            playSound(PlayerSounds::Type::GotHit);
            shootingPerson.addHits(1);
            if (shootingPlayer.getBonus() == BonusType::VAMPIRE_SHOTS) {
                shootingPlayer.addLife(amount);
            }
        }

        if (life <= 0.0f) {
            die();
            orientation = (hitPoint.x < getCentre().x) ? Orientation::Left : Orientation::Right;
            shot.onKillPlayer(*this, directHit, hitPoint, *world);
            return true;
        } else {
            shot.onHitPlayer(*this, directHit, hitPoint, *world);
        }

        return false;
    }

    bool Player::hit(Float32 amount) {
        if (isInvulnerable() || !isAlive()) {
            return false;
        }

        if (!eventListener->onDamageByEnv(*this, amount)) {
            return false;
        }

        timeSinceHit = 0;

        if (life <= 0.0f) {
            die();
            eventListener->onKillByEnv(*this);

            return true;
        }

        return false;
    }

    bool Player::airHit(Float32 amount) {
        Float32 oldAir = air;
        air -= amount;
        bool outOfAir = false;

        if (air < 0) {
            air = 0;
            outOfAir = true;
        } else if (air > D6_MAX_AIR) {
            air = D6_MAX_AIR;
        }

        if (air != oldAir) {
            if (air == D6_MAX_AIR) {
                indicators.getAir().hide();
            } else {
                indicators.getAir().show();
            }
        }

        if (outOfAir && hit(amount)) {
            playSound(PlayerSounds::Type::Drowned);
            return true;
        }

        return false;
    }

    void Player::dropWeapon(const Level &level) {
        Rectangle bbox = getCollisionRect();
        Int32 x1 = Int32(bbox.left.x), x2 = Int32(bbox.right.x);
        Int32 y = Int32(bbox.left.y + 0.5f);

        if (level.isWall(x1, y - 1, true) && !level.isWall(x1, y, true)) {
            world->getBonusList().addPlayerGun(*this, Vector(x1, y));
        } else {
            if (level.isWall(x2, y - 1, true) && !level.isWall(x2, y, true)) {
                world->getBonusList().addPlayerGun(*this, Vector(x2, y));
            }
        }
    }

    Player &Player::addLife(Float32 change, bool showHpBar) {
        Float32 oldLife = life;
        life = std::max(0.0f, std::min(Float32(D6_MAX_LIFE), life + change));
        if (showHpBar && oldLife != life) {
            indicators.getHealth().show();
        }
        return *this;
    }

    void Player::checkWater(World &world, Float32 elapsedTime) {
        const Rectangle rect = getCollisionRect();
        Float32 centerX = rect.getCentre().x;
        Float32 headY = rect.left.y + rect.getSize().y * 0.8f;
        Float32 feetY = rect.left.y + rect.getSize().y * 0.1f;
        Float32 bottomY = rect.left.y;

        Water headWater = world.getLevel().getWaterType(Int32(centerX), Int32(headY));
        water.headUnderWater = headWater;

        if (headWater != Water::NONE) {
            water.feetInWater = headWater;
            headWater.onUnder(*this, elapsedTime);
        } else {
            airHit(-2 * D6_AIR_RECHARGE_SPEED * elapsedTime);

            Water feetWater = world.getLevel().getWaterType(Int32(centerX), Int32(feetY));
            if (feetWater != Water::NONE && water.feetInWater == Water::NONE) {
                feetWater.onEnter(*this, Vector(centerX, bottomY), world);
            }
            water.feetInWater = feetWater;
        }
    }

    void Player::checkElevator(Float32 speedFactor) {
        const Elevator *elevator = world->getElevatorList().checkPlayer(*this, speedFactor);

        if (elevator != nullptr) {
            this->elevator = elevator;
            position.y = elevator->getPosition().y;
            velocity.y = 0.0f;
        }
    }

    void Player::useTemporarySkin(PlayerSkin &tempSkin) {
        tempSkinDuration = Float32(10 + Math::random(5));
        sprite->setTextures(tempSkin.getTextureList());
    }

    void Player::switchToOriginalSkin() {
        tempSkinDuration = 0;
        sprite->setTextures(skin.getTextureList());
    }

    void Player::processShot(Shot &shot, std::vector<Player *> &playersHit, std::vector<Player *> &playersKilled) {
        bool suicide = (std::find(playersKilled.begin(), playersKilled.end(), this) != playersKilled.end());

        if (suicide) {
            playSound(PlayerSounds::Type::Suicide);
            eventListener->onSuicide(*this, playersKilled.size() - 1);
        } else if (!playersKilled.empty()) {
            playSound(PlayerSounds::Type::KilledOther);
        }

        for (Player *player : playersKilled) {
            if (!player->is(*this)) {
                player->playSound(PlayerSounds::Type::WasKilled);
                player->eventListener->onKillByPlayer(*player, *this, shot, suicide);
            }
        }

        bool hittedSelf = (std::find(playersHit.begin(), playersHit.end(), this) != playersHit.end());
        if (!hittedSelf && !playersHit.empty()) {
            playSound(PlayerSounds::Type::HitOther);
        }
    }

    void Player::die() {
        setFlag(FlagDead | FlagLying);
        unsetFlag(FlagMoveUp | FlagMoveDown | FlagMoveLeft | FlagMoveRight | FlagKnee | FlagPick);
        sprite->setPosition(getSpritePosition()).setLooping(AnimationLooping::OnceAndStop);
        gunSprite->setDraw(false);

        Int32 timeAlive = Int32((clock() - roundStartTime) / CLOCKS_PER_SEC);
        getPerson().addTimeAlive(timeAlive);
    }
}
