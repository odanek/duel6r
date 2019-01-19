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

#include "math/Math.h"
#include "Fire.h"

namespace Duel6 {
    const FireType FireType::CONIFEROUS_TREE(0, 7);
    const FireType FireType::BROAD_LEAVED_TREE(1, 8);
    const std::vector<FireType> FireType::types = {CONIFEROUS_TREE, BROAD_LEAVED_TREE};

    namespace {
        AnimationEntry nonFireAnimation[] = {0, 328, -1, 0};
        AnimationEntry burnedAnimation[] = {1, 328, -1, 0};
        std::vector<AnimationEntry> burningAnimation;
    }

    Fire::Fire(const FireType &type, SpriteList::Iterator sprite, const Vector &position)
            : type(type), sprite(sprite), position(position), burned(false) {}

    FireList::FireList(const GameResources &resources, SpriteList &spriteList)
            : spriteList(spriteList), burningTexture(resources.getBurningTexture()),
              textures(resources.getFireTextures()) {}

    void FireList::find(const Level &level) {
        for (Int32 y = 0; y < level.getHeight(); y++) {
            for (Int32 x = 0; x < level.getWidth(); x++) {
                const Block &block = level.getBlockMeta(x, y);

                if (block.isBurning()) {
                    for (const FireType &type : FireType::values()) {
                        if (block.getIndex() == type.getBlock()) {
                            Vector position(x, y);

                            auto sprite = spriteList.add(nonFireAnimation, textures.at(type.getId()));
                            sprite->setPosition(position, 0.75f).setLooping(AnimationLooping::OnceAndStop);

                            fires.emplace_back(type, sprite, position);
                        }
                    }
                }
            }
        }
    }

    void FireList::check(const Vector &explCentre, Float32 d) {
        for (Fire &fire : fires) {
            if (fire.isBurned()) {
                continue;
            }

            Float32 distance = (explCentre - fire.getCentre()).length();
            if (distance >= d) {
                continue;
            }

            fire.setBurned(true);

            auto sprite = spriteList.add(burningAnimation.data(), burningTexture);
            sprite->setPosition(fire.getPosition() - Vector(0.3f, 0.2f), 0.78f)
                    .setSize(Vector(1.6f, 1.6f))
                    .setLooping(AnimationLooping::OnceAndRemove)
                    .setBlendFunc(BlendFunc::SrcColor)
                    .setOnFinished([&fire]() {
                        fire.getSprite()->setAnimation(burnedAnimation);
                    });
        }
    }

    void FireList::initialize() {
        for (Int32 j = 0; j < 3; j++) {
            for (Int32 i = 0; i < 49; i++) {
                burningAnimation.push_back(i);
                burningAnimation.push_back(1);
            }
        }
        burningAnimation.push_back(-1);
        burningAnimation.push_back(0);
    }
}
