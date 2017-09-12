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

#include "Defines.h"
#include "Water.h"
#include "Player.h"
#include "World.h"

namespace Duel6 {
    class WaterImpl {
    public:
        virtual ~WaterImpl() {}

        virtual void onEnter(Player &player, const Vector &location, World &world) const = 0;

        virtual void onUnder(Player &player, Float32 elapsedTime) const = 0;
    };

    namespace {
        Int16 wtAnim[24] = {0, 5, 1, 5, 2, 5, 3, 5, 4, 5, 5, 5, 6, 5, 7, 5, 8, 5, 9, 5, -1, 0};

        class EmptyImpl : public WaterImpl {
            void onEnter(Player &player, const Vector &location, World &world) const {}

            void onUnder(Player &player, Float32 elapsedTime) const {}
        };

        static EmptyImpl NONE_WATER;

        class WaterBase : public WaterImpl {
        private:
            Sound::Sample splashSample;
            TextureList textures;

        public:
            WaterBase(Sound &sound, TextureManager &textureManager, const std::string &sample, const Color &color) {
                splashSample = sound.loadSample(sample);

                TextureManager::SubstitutionTable subst;
                subst[Color(0, 182, 255)] = color;
                textures = textureManager.load(D6_TEXTURE_WATER_PATH, TextureFilter::NEAREST, true, subst);
            }

            void onEnter(Player &player, const Vector &location, World &world) const override {
                addSplash(world.getSpriteList(), location);
                getSplashSound().play();
            }

            void onUnder(Player &player, Float32 elapsedTime) const override {
                Float32 hitAmount = getAirHit() * elapsedTime;
                player.airHit(hitAmount);
            }

        protected:
            Sound::Sample getSplashSound() const {
                return splashSample;
            }

            void addSplash(SpriteList &spriteList, const Vector &position) const {
                Sprite waterSplash(wtAnim, textures);
                waterSplash.setPosition(position - Vector(0.5f, 0.0f), 0.5f).setLooping(
                        AnimationLooping::OnceAndRemove);
                spriteList.addSprite(waterSplash);
            }

            virtual Float32 getAirHit() const = 0;
        };

        class BlueWater : public WaterBase {
        public:
            BlueWater(Sound &sound, TextureManager &textureManager)
                    : WaterBase(sound, textureManager, D6_FILE_WATER_BLUE, Color(0, 182, 255)) {}

            Float32 getAirHit() const override {
                return 60.0f;
            }
        };

        class RedWater : public WaterBase {
        public:
            RedWater(Sound &sound, TextureManager &textureManager)
                    : WaterBase(sound, textureManager, D6_FILE_WATER_RED, Color(197, 0, 0)) {}

            Float32 getAirHit() const override {
                return 120.0f;
            }
        };

        class GreenWater : public WaterBase {
        public:
            GreenWater(Sound &sound, TextureManager &textureManager)
                    : WaterBase(sound, textureManager, D6_FILE_WATER_GREEN, Color(0, 197, 0)) {}

            Float32 getAirHit() const override {
                return 180.0f;
            }
        };
    }

    const Water Water::NONE;
    const Water Water::BLUE;
    const Water Water::RED;
    const Water Water::GREEN;
    std::vector<Water> Water::types;
    std::vector<Water::WaterImplPtr> Water::implementations;

    Water::Water()
            : impl(&NONE_WATER) {}

    const std::vector<Water> &Water::values() {
        return types;
    }

    void Water::onEnter(Player &player, const Vector &location, World &world) {
        impl->onEnter(player, location, world);
    }

    void Water::onUnder(Player &player, Float32 elapsedTime) {
        impl->onUnder(player, elapsedTime);
    }

    void Water::assign(WaterImplPtr &&impl) const {
        this->impl = impl.get();
        implementations.push_back(std::forward<WaterImplPtr>(impl));
        types.push_back(*this);
    }

    void Water::initialize(Sound &sound, TextureManager &textureManager) {
        BLUE.assign(std::make_unique<BlueWater>(sound, textureManager));
        RED.assign(std::make_unique<RedWater>(sound, textureManager));
        GREEN.assign(std::make_unique<GreenWater>(sound, textureManager));
    }

    bool Water::operator==(const Water &water) const {
        return impl == water.impl;
    }

    bool Water::operator!=(const Water &water) const {
        return impl != water.impl;
    }
}