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

#include "Sprite.h"
#include "Video.h"

namespace Duel6 {
    Sprite::Sprite() {
        frame = 0;
        delay = 0;
        speed = 1;
        looping = AnimationLooping::RepeatForever;
        orientation = Orientation::Left;
        visible = true;
        noDepth = false;
        finished = false;
        size = Vector(1.0f, 1.0f);
        grow = 0;
        alpha = 1.0f;
        zRotation = 0;
    }

    Sprite::Sprite(const Int16 *animation, const TextureList &textures)
            : Sprite() {
        this->animation = animation;
        this->textures = textures;
    }

    Sprite &Sprite::setAnimation(const Int16 *animation) {
        if (this->animation != animation) {
            this->animation = animation;
            delay = 0.0f;
            frame = 0;
            finished = false;
        }

        return *this;
    }

    Sprite &Sprite::setDraw(bool draw) {
        visible = draw;
        return *this;
    }

    Sprite &Sprite::setNoDepth(bool depth) {
        noDepth = depth;
        return *this;
    }

    void Sprite::update(float elapsedTime) {
        delay += elapsedTime;
        if (delay >= animation[frame + 1] * speed) {
            frame += 2;
            delay = 0;
            if (animation[frame] == -1) {
                finished = true;

                if (looping == AnimationLooping::RepeatForever) {
                    frame = 0;
                } else if (looping == AnimationLooping::OnceAndStop) {
                    frame -= 2;
                }
            }
        }

        if (grow > 0) {
            Vector growStep = Vector(grow, grow) * elapsedTime;
            position -= growStep;
            size += 2 * growStep;
        }
    }

    void Sprite::render() const {
        if (!visible) {
            return;
        }

        if (isNoDepth()) {
            globRenderer->enableDepthTest(false);
        }

        Int32 textureIndex = animation[frame];
        Texture texture = textures.at(textureIndex);
        Material material(texture, Color(255, 255, 255, Uint8(255 * alpha)), !isTransparent());

        bool rotated = zRotation != 0.0;
        if (rotated) {
            Vector translate = position + rotationCentre;

            Matrix shift = Matrix::translate(-translate);
            Matrix rotate = Matrix::rotate(zRotation, Vector::UNIT_Z);
            Matrix unshift = Matrix::translate(translate);

            globRenderer->setModelMatrix(unshift * rotate * shift);
        }

        bool reversed = (orientation == Orientation::Right);
        Vector texturePos = Vector(reversed ? 1.0f : 0.0f, 1.0f);
        Vector textureSize = Vector(reversed ? -1.0f : 1.0f, -1.0f);

        globRenderer->quadXY(Vector(position.x, position.y, z), size, texturePos, textureSize, material);

        if (rotated) {
            globRenderer->setModelMatrix(Matrix::IDENTITY);
        }

        if (isNoDepth()) {
            globRenderer->enableDepthTest(true);
        }
    }
}