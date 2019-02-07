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

#ifndef DUEL6_PLAYERINDICATORS_H
#define DUEL6_PLAYERINDICATORS_H

#include "Type.h"

namespace Duel6 {
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
}

#endif
