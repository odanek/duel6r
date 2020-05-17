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

#include "Explosion.h"
#include "Game.h"
namespace Duel6 {
    ExplosionList::ExplosionList(const GameResources &resources, Game &game, Float32 speed)
            : textures(resources.getExplosionTextures()), game(game), speed(speed) {
    }

    void ExplosionList::update(Float32 elapsedTime) {
        auto explIter = explosions.begin();
        while (explIter != explosions.end()) {
            explIter->now += speed * elapsedTime;
            if (explIter->now > explIter->max) {
                explIter = explosions.erase(explIter);
            } else {
                ++explIter;
            }
        }
    }

    void ExplosionList::render(Renderer &renderer) const {
        renderer.enableDepthTest(false);

        for (const Explosion &explosion : explosions) {
            Material material = Material::makeMaskedColoredTexture(textures, explosion.color);
            Vector position = explosion.centre - Vector(explosion.now, explosion.now);
            position.z = 0.6f;
            Vector size = Vector(2 * explosion.now, 2 * explosion.now);
            renderer.quadXY(position, size, Vector::ZERO, Vector(1, 1), material);
        }

        renderer.enableDepthTest(true);
    }

    void ExplosionList::add(const Vector &centre, Float32 startSize, Float32 maxSize, const Color &color) {
        game.spawnExplosion({centre, startSize, maxSize, color});
    }

    void ExplosionList::spawn(Explosion &&explosion) {
        explosions.push_back(std::move(explosion));
    }
}
