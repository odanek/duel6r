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
#include "math/Math.h"
#include "Video.h"
#include "PlayerEventListener.h"
#include "Game.h"
namespace Duel6 {
    //TODO: This still needs further fine-tuning for good jumping experience
    static const float GRAVITATIONAL_ACCELERATION = -11.0f;
    static const float JUMP_FACTOR = -0.7f;
    static const float JUMP_ACCELERATION = JUMP_FACTOR * GRAVITATIONAL_ACCELERATION;

    // Very important fun aspect!
    static const float SHOT_FORCE_FACTOR = 0.05f;

    Player::Player(Game *game, Person &person, const PlayerSkin &skin, const PlayerSounds &sounds, const PlayerControls &controls,
                   Int32 id, Int32 team,
                   Int32 clientId,
                   Int32 clientLocalId,
                   size_t pos)
            : game(game),
              person(&person),
              skin(skin),
              animations(&skin.getAnimations()),
              sounds(&sounds),
              controls(&controls),
              orientation(Orientation::Left),
              id(id),
              team(team),
              clientId(clientId),
              clientLocalId(clientLocalId),
              pos(pos){
        flags = 0;
        camera.rotate(180.0, 0.0, 0.0);
        controllerState = 0;
        unconfirmedInputs.fill(0);
    }

    Player::~Player() {
    }

    void Player::startRound(World &world, Int32 startBlockX, Int32 startBlockY, Int32 ammo, const Weapon &weapon) {
        this->world = &world;
        unconfirmedInputs.fill(0);
        collider.initPosition(Float32(startBlockX), Float32(startBlockY) + 0.0001f);

        sprite = world.getSpriteList().add(animations->getStand().get(), skin.getTexture());
        sprite->setPosition(getSpritePosition(), 0.5f);
        this->weapon = weapon;
        gunSprite = weapon.makeSprite(world.getSpriteList());

        flags = FlagHasGun;
        orientation = Math::random(2) == 0 ? Orientation::Left : Orientation::Right;
        timeToReload = weapon.isChargeable() ? getReloadInterval() : 0;
        life = D6_MAX_LIFE;
        air = D6_MAX_AIR;
        this->ammo = ammo;
        timeSinceHit = 1.0f ;
        bonus = BonusType::INVULNERABILITY;
        bonusDuration = 2.0f;
        bonusRemainingTime = 2.0f;
        tempSkinDuration = 0;
        roundKills = 0;
        bodyAlpha = 1.0f;
        alpha = 1.0f;
        water.headUnderWater = Water::NONE;
        water.feetInWater = Water::NONE;

        indicators.hideAll(false);
        indicators.getName().show(400.0f);
        indicators.getBonus().show(bonusDuration);
        indicators.getBullets().show(4.0f);

        roundStartTime = clock();
        getPerson().addGames(1);
    }

    void Player::endRound() {
        if(isDeleted()){
            return;
        }
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
        return collider.isOnGround();
    }

    void Player::moveHorizontal(const Level &level, Float32 elapsedTime, Float32 speed) {
        if (hasFlag(FlagMoveLeft)) {
            if (this->collider.velocity.x > -D6_PLAYER_MAX_SPEED) {
                this->collider.velocity.x -= 2 * elapsedTime; // x 2 to compensate friction in CollidingEntity
            }
            if (this->collider.velocity.x < 0.0f) {
                orientation = Orientation::Left;
            }
        }

        if (hasFlag(FlagMoveRight)) {
            if (this->collider.velocity.x < D6_PLAYER_MAX_SPEED) {
                this->collider.velocity.x += 2 * elapsedTime;
            }

            if (this->collider.velocity.x > 0.0f) {
                orientation = Orientation::Right;
            }
        }
    }
     Player::Player(Player &&r):
          game(r.game),
          deleted(r.deleted),
          person(r.person),
          skin(r.skin),
          camera(r.camera),
          cameraFov(r.cameraFov),
          cameraTolerance(r.cameraTolerance),
          animations(r.animations),
          sounds(r.sounds),
          controls(r.controls),
          view(r.view),
          water(r.water),
          sprite(r.sprite),
          gunSprite(r.gunSprite),
          flags(r.flags),
          orientation(r.orientation),
          life(r.life),
          air(r.air),
          ammo(r.ammo),
          bonus(r.bonus),
          roundKills(r.roundKills),
          timeToReload(r.timeToReload),
          bonusRemainingTime(r.bonusRemainingTime),
          bonusDuration(r.bonusDuration),
          timeSinceHit(r.timeSinceHit),
          timeStuckInWall(r.timeStuckInWall),
          tempSkinDuration(r.tempSkinDuration),
          alpha(r.alpha),
          weapon(r.weapon),
          eventListener(r.eventListener),
          world(r.world),
          bodyAlpha(r.bodyAlpha),
          roundStartTime(r.roundStartTime),
          indicators(r.indicators),
          controllerState(r.controllerState),
          collider(r.collider),
          id(r.id),
          team(r.team),
          clientId(r.clientId),
          clientLocalId(r.clientLocalId),
          local(r.local),
          pos(r.pos),
          tick(r.tick),
          lastConfirmedTick(r.lastConfirmedTick),
          lateTicks(r.lateTicks),
          unconfirmedInputs(r.unconfirmedInputs),
          isCompensating(r.isCompensating) {
        }

    Player & Player::operator=(Player &&r){
        game = r.game;
        deleted = r.deleted;
        person = r.person;
        skin = r.skin;
        camera = r.camera;
        cameraFov = r.cameraFov;
        cameraTolerance = r.cameraTolerance;
        animations = r.animations;
        sounds = r.sounds;
        controls = r.controls;
        view = r.view;
        water = r.water;
        sprite = r.sprite;
        gunSprite = r.gunSprite;
        flags = r.flags;
        orientation = r.orientation;
        life = r.life;
        air = r.air;
        ammo = r.ammo;
        bonus = r.bonus;
        roundKills = r.roundKills;
        timeToReload = r.timeToReload;
        bonusRemainingTime = r.bonusRemainingTime;
        bonusDuration = r.bonusDuration;
        timeSinceHit = r.timeSinceHit;
        timeStuckInWall = r.timeStuckInWall;
        tempSkinDuration = r.tempSkinDuration;
        alpha = r.alpha;
        weapon = r.weapon;
        eventListener = r.eventListener;
        world = r.world;
        bodyAlpha = r.bodyAlpha;
        roundStartTime = r.roundStartTime;
        indicators = r.indicators;
        controllerState = r.controllerState;
        collider = r.collider;
        id = r.id;
        team = r.team;
        clientId = r.clientId;
        clientLocalId = r.clientLocalId;
        local = r.local;
        pos = r.pos;
        tick = r.tick;
        lastConfirmedTick = r.lastConfirmedTick;
        lateTicks = r.lateTicks;
        unconfirmedInputs = r.unconfirmedInputs;
        isCompensating = r.isCompensating;
        return *this;
    }
    void Player::moveVertical(const Level &level, Float32 elapsedTime, Float32 speed) {
        if (hasFlag(FlagMoveUp) && this->collider.velocity.y <= 0 && (collider.isOnHardSurface())) {
            if (!collider.isUnderHardSurface()) {
                collider.acceleration.y += JUMP_ACCELERATION;
            }

            if (isOnElevator()) { //leaving elevator
                collider.acceleration.y += collider.elevator->getVelocity().y;
            }

        }
        // player can perform a double jump
        // fast movement bonus unlocks super double jump
        if (hasFlag(FlagDoubleJump)) {
            if (collider.velocity.y > 0 && getBonus() == BonusType::FAST_MOVEMENT) {
                collider.acceleration.y += JUMP_ACCELERATION;
            } else {
                collider.velocity.y = JUMP_ACCELERATION;
            }
            unsetFlag(FlagDoubleJump);
            unsetFlag(FlagDoubleJumpReset);
        }
    }

    void Player::fall() {
        if (isOnGround() || isOnElevator()) {
            setFlag(FlagKnee);
        } else {
            if (isRising()) {
                collider.velocity.y = 0.0f;
            }
        }
    }

    void Player::pick() {
        if ((isOnGround() || isOnElevator()) && !isMoving()) {
            world->getBonusList().checkWeapon(*this);
        }
    }

    bool Player::isReloading() {
        if (weapon.isChargeable()) {
            return (hasFlag(FlagShoot) && !hasFlag(FlagShootDebounce)) || getChargeLevel() < 0.5f;
        }
        return timeToReload > 0;
    }

    void Player::shoot() {
        if ((!getAmmo() && getBonus() != BonusType::INFINITE_AMMO) || !hasGun())
            return;


        indicators.getReload().show(timeToReload + Indicator::FADE_DURATION);

        if (isReloading())
            return;

        if (weapon.isChargeable()) {
            if (!hasFlag(FlagShootDebounce) || !hasFlag(FlagShoot)) {
                return;
            }
        }

        if (getBonus() != BonusType::INFINITE_AMMO) {
            indicators.getBullets().show();
            ammo--;
        }
        gunSprite->setFrame(0);
        getPerson().addShots(1);
        Orientation originalOrientation = getOrientation();

        getWeapon().shoot(*this, originalOrientation, *world);

        if (getBonus() == BonusType::SPLIT_FIRE) {
            getPerson().addShots(1);
            Orientation secondaryOrientation =
                    originalOrientation == Orientation::Left ? Orientation::Right : Orientation::Left;
            getWeapon().shoot(*this, secondaryOrientation, *world);
        }

        if (weapon.isChargeable()) {
            if (!hasFlag(FlagShootDebounce) || !hasFlag(FlagShoot)) {
                return;
            }
            timeToReload = getReloadInterval();
        } else {
            timeToReload = getReloadInterval();
        }
    }

    Player &Player::pickWeapon(Weapon weapon, Int32 bullets, Float32 remainingReloadTime) {
        setFlag(FlagPick);
        unsetFlag(FlagMoveLeft | FlagMoveRight);

        replaceWeapon(weapon);
        ammo = bullets;
        if (weapon.isChargeable()) {
            timeToReload = getReloadInterval();
        } else {
            timeToReload = remainingReloadTime;
        }
        indicators.getReload().show(timeToReload + Indicator::FADE_DURATION);
        indicators.getBullets().show();

        return *this;
    }

    void Player::replaceWeapon(Weapon weapon){
        this->weapon = weapon;
        world->getSpriteList().remove(gunSprite);
        gunSprite = weapon.makeSprite(world->getSpriteList());
    }

    void Player::makeMove(const Level &level, ElevatorList &elevators, Float32 elapsedTime) {
        Float32 speed = getSpeed() * elapsedTime;

        moveVertical(level, elapsedTime, speed);
        moveHorizontal(level, elapsedTime, speed);
        collider.collideWithElevators(elevators, elapsedTime, speed);
        collider.collideWithLevel(level, elapsedTime, speed);

        if (isPickingGun() && sprite->isFinished()) {
            unsetFlag(FlagPick);
            setFlag(FlagHasGun);
        }
        if (isKneeling()) {
            unsetFlag(FlagMoveLeft | FlagMoveRight);
        }
    }

    Vector Player::getVelocity() const {
        return collider.velocity;
    }

    void Player::checkStuck(const Level &level, Float32 elapsedTime) {
        if (collider.isInWall()) {
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
        collider.initPosition((Float32) newPosition.first, Float32(newPosition.second) + 0.0001f);

        timeStuckInWall = 0;
    }

    Float32 Player::getSpeed() const {
        Float32 spd = 1.0f;

        if (isUnderWater() && !hasSnorkel()) {
            spd *= 0.67f;
        }

        if (tempSkinDuration > 0) {
            spd *= 0.5f;
        }

        if (getBonus() == BonusType::FAST_MOVEMENT) {
            spd *= 1.43f;
        } else if (!hasGun()) {
            spd *= 1.4f;
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
        if (isDeleted()){
            return;
        }

        if (!isAlive() && !isGhost()) {
            return;
        }

        if (controllerState & ButtonStatus) {
            indicators.showAll(5.0f);
        }

        if (isPickingGun()) {
            return;
        }

        if (!isKneeling()) {
            if (controllerState & ButtonLeft) {
                setFlag(FlagMoveLeft);
            } else {
                unsetFlag(FlagMoveLeft);
            }

            if (controllerState & ButtonRight) {
                setFlag(FlagMoveRight);
            } else {
                unsetFlag(FlagMoveRight);
            }
            if (controllerState & ButtonUp) {
                if (!hasFlag(FlagMoveUp) && !collider.isOnHardSurface() && hasFlag(FlagDoubleJumpReset)) {
                    setFlag(FlagDoubleJump);
                }
                setFlag(FlagMoveUp);
            } else {
                unsetFlag(FlagMoveUp);
            }
            if (collider.isOnHardSurface()) {
                setFlag(FlagDoubleJumpReset);
            }
            if (controllerState & ButtonPick) {
                dropWeapon(world->getLevel());
                pick();
            }
        }

        if (!isGhost()) {
            if (controllerState & ButtonShoot) {
                shoot();
                unsetFlag(FlagShootDebounce);
                setFlag(FlagShoot);
            } else {
                setFlag(FlagShootDebounce);
                if (hasFlag(FlagShoot)) {
                    shoot();
                    unsetFlag(FlagShoot);
                }
            }
        }

        unsetFlag(FlagKnee);
        if (controllerState & ButtonDown) {
            fall();
        }
    }

    void Player::updateControllerStatus() {
        if(local){
            controllerState = 0;
        } else {
            return;
        }
        const PlayerControls & controls = *this->controls;
        if (controls.getLeft().isPressed()) {
            controllerState |= ButtonLeft;
        }
        if (controls.getRight().isPressed()) {
            controllerState |= ButtonRight;
        }
        if (controls.getUp().isPressed()) {
            controllerState |= ButtonUp;
        }
        if (controls.getDown().isPressed()) {
            controllerState |= ButtonDown;
        }
        if (controls.getShoot().isPressed()) {
            controllerState |= ButtonShoot;
        }
        if (controls.getPick().isPressed()) {
            controllerState |= ButtonPick;
        }
        if (controls.getStatus().isPressed()) {
            controllerState |= ButtonStatus;
        }
    }

    void Player::updateLoop(World &world, ElevatorList &elevators, Float32 elapsedTime) {
        checkKeys();
        updateDimensions();
        makeMove(world.getLevel(), elevators, elapsedTime);
        setAnm();
    }

    void Player::compensateLag(World &world,  uint16_t gameTick, uint16_t confirmedTick, Float32 elapsedTime) {
//        if(!local) {
//            return;
//        }
        uint16_t ticks = gameTick - confirmedTick;
        //collider.setPosition(lastConfirmedPos);
        auto tmp = controllerState;
        isCompensating = true;
        for (auto i = 1; i < ticks; i++) {
            controllerState = unconfirmedInputs[(confirmedTick + i) % 128];
//            updateLoop(world, world.getUnconfirmedElevatorList(), elapsedTime, true);
            updateLoop(world, world.getElevatorList(), elapsedTime);
        }
        isCompensating = false;
        controllerState = tmp;
    }

    void Player::update(World &world, ScreenMode screenMode, Float32 elapsedTime) {
        tick++;
        checkWater(world, elapsedTime);
        if (isAlive()) {
            /* TODO: NETCODE */
            world.getBonusList().checkBonus(*this);
        }

        updateLoop(world, world.getElevatorList(), elapsedTime);

        // Drop gun if still has it and died
        if (isLying() && hasGun()) {
            setBonus(BonusType::NONE, 0);
            dropWeapon(world.getLevel());
        }

        // Move intervals
        indicators.updateAll(elapsedTime);
        if (isReloading()) {
            if (!weapon.isChargeable() || hasFlag(FlagShoot) || getChargeLevel() < 0.5) {
                timeToReload = std::max(0.0f, getReloadTime() - elapsedTime);
            }
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
        Animation animation;
        const PlayerAnimations & animations = *this->animations;
        sprite->setSpeed(1.0f);
        if (!isAlive() && !isGhost()) {
            if (isLying()) {
                if (isDying()) {
                    if (sprite->getAnimation() == animations.getDying().get() && sprite->isFinished()) {
                        unsetFlag(FlagDying);
                        setFlag(FlagDead);
                    }
                    animation = animations.getDying().get();
                } else { //dead
                    animation = animations.getDying().get();
                    if (!collider.isOnHardSurface()) {
                        animation = animations.getDeadFall().get();
                    } else {
                        if (sprite->getAnimation() == animations.getDeadFall().get()) {
                            animation = animations.getDeadHit().get();
                            sprite->setLooping(AnimationLooping::OnceAndStop);
                        } else {
                            animation = animations.getDeadLying().get();
                        }
                    }
                }
            } else {
                sprite->setDraw(false);
                animation = animations.getStand().get();
            }
        } else if (isPickingGun()) {
            animation = animations.getPick().get();
        } else if (isKneeling()) {
            animation = timeSinceHit < 0.5f ? animations.getHitDuck().get() : animations.getDuck().get();
            sprite->setLooping(AnimationLooping::RepeatForever);
        } else if (isOnElevator()) {
            if (!isMoving()) {
                animation = animations.getStand().get();
            } else {
                sprite->setSpeed(getVelocity().length());
                animation = animations.getWalk().get();
                sprite->setLooping(AnimationLooping::RepeatForever);
            }
        } else if (!isOnGround()) {
            if (isRising()) {
                animation = animations.getJump().get();
                sprite->setLooping(AnimationLooping::RepeatForever);
            } else {
                animation = animations.getFall().get();
                sprite->setLooping(AnimationLooping::RepeatForever);
            }
            sprite->setSpeed(getVelocity().length());
        } else if (!isMoving()) {
            animation = timeSinceHit < 0.5f ? animations.getHitStand().get() : animations.getStand().get();
        } else {
            sprite->setSpeed(getVelocity().length());
            animation = animations.getWalk().get();
            sprite->setLooping(AnimationLooping::RepeatForever);
        }

        sprite->setPosition(getSpritePosition())
                .setOrientation(getOrientation())
                .setAnimation(animation)
                .setAlpha(bodyAlpha * alpha);

        gunSprite->setPosition(getGunSpritePosition(), 0.5f)
                .setOrientation(getOrientation())
                .setDraw(hasGun() && isAlive() && !isPickingGun())
                .setAlpha(alpha);
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

        Vector position;
        position.x = levelSizeX / 2.0f;
        position.y = levelSizeY / 2.0f;
        position.z = mZ + 1.0f;

        cameraFov.x = dX / 2.0f;
        cameraFov.y = dY / 2.0f;
        cameraTolerance.x = (dX * D6_CAM_TOLPER_X) / 200.0f;
        cameraTolerance.y = (dY * D6_CAM_TOLPER_Y) / 200.0f;

        camera.setPosition(position);

        if (screenMode == ScreenMode::SplitScreen) {
            updateCam(levelSizeX, levelSizeY);
        }
    }

    void Player::updateCam(Int32 levelSizeX, Int32 levelSizeY) {
        Float32 mX = 0.0, mY = 0.0;
        Vector centre = getCentre();
        Vector position = camera.getPosition();
        Vector tolerance = cameraTolerance;
        Vector fov = cameraFov;

        if (centre.x < position.x - tolerance.x) {
            mX = centre.x - (position.x - tolerance.x);
            if (position.x - fov.x + mX < 0.0f)
                mX = fov.x - position.x;
        } else if (centre.x > position.x + tolerance.x) {
            mX = centre.x - (position.x + tolerance.x);
            if (position.x + fov.x + mX > (Float32) levelSizeX)
                mX = (Float32) levelSizeX - (position.x + fov.x);
        }
        if (centre.y < position.y - tolerance.y) {
            mY = centre.y - (position.y - tolerance.y);
            if (position.y - fov.y + mY < 0.0f)
                mY = fov.y - position.y;
        } else if (centre.y > position.y + tolerance.y) {
            mY = centre.y - (position.y + tolerance.y);
            if (position.y + fov.y + mY > (Float32) levelSizeY)
                mY = (Float32) levelSizeY - (position.y + fov.y);
        }

        if (mX != 0.0) {
            position.x += mX;
        }
        if (mY != 0.0) {
            position.y += mY;
        }
        if (mX != 0.0 || mY != 0.0) {
            camera.setPosition(position);
        }
    }

    bool
    Player::hitByShot(Float32 amount, Shot &shot, bool directHit, const Vector &hitPoint, const Vector &shotVector) {
        Vector estimatedShotVector = getCentre() - hitPoint;
        if (directHit) {
            estimatedShotVector.x = shotVector.x;    //makes sure that close-up shots have the right effect
        }
        collider.externalForces += estimatedShotVector.unit() * amount * SHOT_FORCE_FACTOR;
        if (isInvulnerable() || isDeleted()) {
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
        if (isInvulnerable() || !isAlive() || isDeleted()) {
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
        if(isCompensating) {
            return;
        }

        if (!hasGun()) {
            return;
        }
        world->getBonusList().addPlayerGun(*this, collider);
        unsetFlag(FlagHasGun);
        indicators.getReload().hide();
        indicators.getBullets().hide();
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

        const Water *headWater = world.getLevel().getWaterType(Int32(centerX), Int32(headY));
        water.headUnderWater = headWater;

        if (headWater != Water::NONE) {
            water.feetInWater = headWater;
            headWater->onUnder(*this, elapsedTime);
        } else {
            airHit(-2 * D6_AIR_RECHARGE_SPEED * elapsedTime);

            const Water *feetWater = world.getLevel().getWaterType(Int32(centerX), Int32(feetY));
            if (feetWater != Water::NONE && water.feetInWater == Water::NONE) {
                feetWater->onEnter(*this, Vector(centerX, bottomY), world);
            }
            water.feetInWater = feetWater;
        }
    }

    void Player::useTemporarySkin(PlayerSkin &tempSkin) {
        tempSkinDuration = Float32(10 + Math::random(5));
        sprite->setTexture(tempSkin.getTexture());
    }

    void Player::switchToOriginalSkin() {
        tempSkinDuration = 0;
        sprite->setTexture(skin.getTexture());
    }

    void Player::processShot(Shot &shot, std::vector<Player *> &playersHit, std::vector<Player *> &playersKilled) {
        bool suicide = (std::find(playersKilled.begin(), playersKilled.end(), this) != playersKilled.end());

        if (suicide) {
            playSound(PlayerSounds::Type::Suicide);
            eventListener->onSuicide(*this, playersKilled);
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

    void Player::updateDimensions() {
        if (isKneeling()) {
            collider.dimensions = Vector(1.0f, 0.8f);
        } else if (isLying()) {
            collider.dimensions = Vector(1.0f, 0.45f);
        } else {
            collider.dimensions = Vector(1.0f, 1.0f);
        }
    }

    void Player::updateBonus(const BonusType *type, Float32 duration, Float32 remainingTime) {
        bonus = type;
        bonusRemainingTime = remainingTime;
        bonusDuration = duration;
        indicators.getBonus().show(bonusDuration);
    }

    Player &Player::setBonus(const BonusType *type, Int32 duration) {
        if (type == bonus) {
            bonusRemainingTime += Float32(duration) / 2;
            bonusDuration += Float32(duration) / 2;
        } else {
            if (bonus != BonusType::NONE) {
                bonus->onExpire(*this, *world);
            }
            bonus = type;
            bonusRemainingTime = Float32(duration);
            bonusDuration = Float32(duration);
        }
        if (bonus != BonusType::NONE) {
            bonus->onApply(*this, *world, Int32(bonusDuration));
        }
        indicators.getBonus().show(bonusDuration);
        return *this;
    }
    void Player::setPosition(float x, float y, float z){
        collider.setPosition(x,y,z);
      //  lastConfirmedPos = collider.getPosition(); //todo add acceleration, velocity etc.
    }
    void Player::die() {
        setFlag(FlagDying | FlagLying);
        unsetFlag(FlagMoveUp | FlagMoveDown | FlagMoveLeft | FlagMoveRight | FlagKnee | FlagPick);
        sprite->setPosition(getSpritePosition()).setLooping(AnimationLooping::OnceAndStop);
        gunSprite->setDraw(false);

        Int32 timeAlive = Int32((clock() - roundStartTime) / CLOCKS_PER_SEC);
        getPerson().addTimeAlive(timeAlive);
    }

    const CollidingEntity &Player::getCollider() const {
        return collider;
    }

    CollidingEntity &Player::getCollider() {
        return collider;
    }

    Int32 Player::getTeam() const{
        return team;
    }

    void Player::setTeam(Int32 team) {
        this->team = team;
    }

    Int32 Player::getClientId() const{
        return clientId;
    }

    void Player::setClientId(Int32 clientId) {
        this->clientId = clientId;
    }

    Int32 Player::getClientLocalId() const{
        return clientLocalId;
    }

    void Player::setClientLocalId(Int32 clientLocalId) {
        this->clientLocalId = clientLocalId;
    }

    void Player::playSound(PlayerSounds::Type type) const {
        game->playSample(*this, type);
    }

    Int32 Player::getId() const
    {
        return id;
    }

    void Player::setId(Int32 id) {
        this->id = id;
    }

    bool Player::isDeleted() const {
        return deleted;
    }

    void Player::setDeleted(bool value) {
        die();
        sprite->setDraw(false);
        this->deleted = value;
    }


    Float32 Player::getAlpha() const {
        return alpha;
    }

    Float32 Player::getBodyAlpha() const {
        return bodyAlpha;
    }

}
