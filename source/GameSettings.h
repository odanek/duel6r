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

#ifndef DUEL6_GAMESETTINGS_H
#define DUEL6_GAMESETTINGS_H

#include <unordered_set>
#include <utility>
#include "Type.h"
#include "ScreenMode.h"
#include "Weapon.h"

namespace Duel6 {
    enum class ShotCollisionSetting {
        None,
        Large,
        All
    };

    enum class LevelSelectionMode {
        Random,
        Shuffle
    };

    class GameSettings {
    public:
        typedef std::unordered_set<Weapon, Weapon::Hash> EnabledWeapons;

    private:
        std::pair<Int32, Int32> ammoRange;
        Int32 maxRounds;
        ScreenMode screenMode;
        Int32 screenZoom;
        bool wireframe;
        bool showFps;
        bool showRanking;
        bool ghostMode;
        ShotCollisionSetting shotCollision;
        EnabledWeapons enabledWeapons;
        LevelSelectionMode levelSelectionMode;

    public:
        GameSettings();

        ScreenMode getScreenMode() const {
            return screenMode;
        }

        GameSettings &setScreenMode(const ScreenMode &screenMode) {
            this->screenMode = screenMode;
            return *this;
        }

        Int32 getScreenZoom() const {
            return screenZoom;
        }

        GameSettings &setScreenZoom(Int32 screenZoom) {
            this->screenZoom = screenZoom;
            return *this;
        }

        const std::pair<Int32, Int32> &getAmmoRange() const {
            return ammoRange;
        }

        GameSettings &setAmmoRange(const std::pair<Int32, Int32> &range) {
            ammoRange = range;
            return *this;
        }

        Int32 getMaxRounds() const {
            return maxRounds;
        }

        bool isRoundLimit() const {
            return maxRounds > 0;
        }

        GameSettings &setMaxRounds(Int32 maxRounds) {
            this->maxRounds = maxRounds;
            return *this;
        }

        GameSettings &setGhostEnabled(bool enabled) {
            ghostMode = enabled;
            return *this;
        }

        bool isGhostEnabled() {
            return this->ghostMode;
        }

        GameSettings &setShotCollision(ShotCollisionSetting value) {
            shotCollision = value;
            return *this;
        }

        ShotCollisionSetting getShotCollision() const {
            return shotCollision;
        }

        bool isWireframe() const {
            return wireframe;
        }

        GameSettings &setWireframe(bool wireframe) {
            this->wireframe = wireframe;
            return *this;
        }

        bool isShowFps() const {
            return showFps;
        }

        GameSettings &setShowFps(bool showFps) {
            this->showFps = showFps;
            return *this;
        }

        bool isShowRanking() const {
            return showRanking;
        }

        GameSettings &setShowRanking(bool showRanking) {
            this->showRanking = showRanking;
            return *this;
        }

        LevelSelectionMode getLevelSelectionMode() const {
            return levelSelectionMode;
        }

        GameSettings &setLevelSelectionMode(LevelSelectionMode mode) {
            levelSelectionMode = mode;
            return *this;
        }

        GameSettings &enableWeapon(const Weapon &weapon, bool enable);

        bool isWeaponEnabled(const Weapon &weapon) const;

        const EnabledWeapons &getEnabledWeapons() const;
    };
}


#endif
