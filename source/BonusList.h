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

#ifndef DUEL6_BONUSLIST_H
#define DUEL6_BONUSLIST_H

#include <list>
#include "Type.h"
#include "Bonus.h"
#include "Player.h"
#include "PlayerSounds.h"
#include "TextureManager.h"
#include "Level.h"
#include "GameSettings.h"
#include "GameResources.h"

namespace Duel6 {
    class BonusList {
    private:
        const GameSettings &settings;
        Texture randomTexture;
        World &world;
        std::list<Bonus> bonuses;
        std::list<LyingWeapon> weapons;

    private:
        static const Int32 RANDOM_BONUS_FREQUENCY = 6;
        static const Int32 MAX_BONUS_ATTEMPTS = 50;

        bool isValidPosition(const Int32 x, const Int32 y, bool weapon);

    public:
        BonusList(const GameSettings &settings, const GameResources &resources, World &world);

        void addRandomBonus();

        void render() const;

        void addPlayerGun(Player &player, const Vector &position);

        void checkBonus(Player &player);

        void checkWeapon(Player &player);
    };

}

#endif