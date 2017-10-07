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

#ifndef DUEL6_SPRITE_H
#define DUEL6_SPRITE_H

#include <vector>
#include "Type.h"
#include "AnimationLooping.h"
#include "Orientation.h"
#include "TextureManager.h"
#include "Vector.h"

namespace Duel6 {
    class Sprite {
    private:
        const Int16 *animation;    // Source array of animations and delays
        TextureList textures;   // Texture array
        Size frame;    // Current animation frame
        Float32 delay;    // Delay to next animation frame
        Float32 speed;     // Speed of animation
        AnimationLooping looping;   // Type of looping
        Orientation orientation;   // Current orientation
        Vector position;
        Float32 z;
        Vector size;
        Float32 grow;   // Grow factor for explosions
        Float32 alpha;  // Transparency ratio
        bool visible;
        bool noDepth;
        bool finished;
        Float32 zRotation;
        Vector rotationCentre;

    public:
        Sprite();

        Sprite(const Int16 *animation, const TextureList &textures);

        Sprite &setPosition(const Vector &position, Float32 z) {
            this->position = position;
            this->z = z;
            return *this;
        }

        Sprite &setPosition(const Vector &position) {
            this->position = position;
            return *this;
        }

        Sprite &setAnimation(const Int16 *animation);

        const Int16 *getAnimation() const {
            return animation;
        }

        Sprite &setTextures(const TextureList &textures) {
            this->textures = textures;
            return *this;
        }

        Sprite &setFrame(Size frame) {
            this->frame = frame;
            this->delay = 0;
            return *this;
        }

        Sprite &setOrientation(Orientation orientation) {
            this->orientation = orientation;
            return *this;
        }

        Sprite &setGrow(Float32 grow) {
            this->grow = grow;
            return *this;
        }

        Sprite &setAlpha(Float32 alpha) {
            this->alpha = alpha;
            return *this;
        }

        Sprite &setSpeed(Float32 speed) {
            this->speed = speed;
            return *this;
        }

        Sprite &setLooping(AnimationLooping looping) {
            this->looping = looping;
            return *this;
        }

        Sprite &setDraw(bool draw);

        Sprite &setNoDepth(bool depth);

        Sprite &setZRotation(Float32 angle, Vector centre = Vector()) {
            zRotation = angle;
            rotationCentre = centre;
            return *this;
        }

        Size getFrame() const {
            return frame;
        }

        AnimationLooping getLooping() const {
            return looping;
        }

        bool isTransparent() const {
            return alpha < 1.0f;
        }

        bool isFinished() const {
            return finished;
        }

        bool isNoDepth() const {
            return noDepth;
        }

        void update(Float32 elapsedTime);

        void render() const;
    };
}

#endif