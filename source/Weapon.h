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

#ifndef DUEL6_WEAPON_H
#define DUEL6_WEAPON_H

#include <memory>
#include <string>
#include <vector>
#include "Type.h"
#include "Sound.h"
#include "Sprite.h"
#include "TextureManager.h"

namespace Duel6 {
    class World;
    class GameSettings;
    class Player;

    class WeaponImpl {
    public:
        virtual ~WeaponImpl() {}

        virtual std::string getName() const = 0;

        virtual Float32 getReloadInterval() const = 0;

        virtual void shoot(Player &player, Orientation orientation, World &world) const = 0;

        virtual Sprite &makeSprite(Sprite &sprite) const = 0;

        virtual Texture getBonusTexture() const = 0;
    };

    class Weapon final {
    public:
        struct Hash {
            std::size_t operator()(const Weapon &val) const {
                std::hash<WeaponImpl *> referenceHash;
                return referenceHash(val.impl);
            }
        };

    private:
        typedef std::unique_ptr<WeaponImpl> WeaponImplPtr;

    private:
        WeaponImpl *impl;

    private:
        static std::vector<WeaponImplPtr> implementations;
        static std::vector<Weapon> weapons;

        explicit Weapon(WeaponImpl *impl);

        static Weapon add(WeaponImplPtr &&impl);

    public:
        Weapon();

        std::string getName() const;

        Float32 getReloadInterval() const;

        void shoot(Player &player, Orientation orientation, World &world) const;

        Sprite &makeSprite(Sprite &sprite) const;

        Texture getBonusTexture() const;

        bool operator==(const Weapon &weapon) const;

        bool operator!=(const Weapon &weapon) const;

    public:
        static const std::vector<Weapon> &values();

        static void initialize(Sound &sound, TextureManager &textureManager);

        static const Weapon &getRandomEnabled(const GameSettings &settings);
    };
}

#endif