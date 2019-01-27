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

#include <string>
#include "PlayerAnimations.h"
#include "PlayerSkinColors.h"
#include "TextureManager.h"
#include "aseprite/animation.h"

namespace Duel6 {

    PlayerAnimations::PlayerAnimation::PlayerAnimation(const animation::Animation & animation, const std::string & name)
        : animation(animation.getAnimation(name)) {
    }

    Animation PlayerAnimations::PlayerAnimation::operator ()() const {
        return animation.data();
    }

    PlayerAnimations::PlayerAnimations(const animation::Animation & animation)
        : animation(animation),
          noAnim(animation, "Stand"),
          d6SAnim(animation, "Stand"),
          d6WAnim(animation, "Walk"),
          d6FallAnim(animation, "Fall"),
          d6JAnim(animation, "Jump"),
          d6DAnim(animation, "Duck"),
          d6DeadFallAnim(animation, "Dead_fall"),
          d6DeadHitAnim(animation, "Dead_hit"),
          d6DeadLyingAnim(animation, "Dead_lying"),
          d6LAnim(animation, "Death"),
          d6NAnim(noAnim),
          d6PAnim(animation, "Pick") {
    }

    Texture PlayerAnimations::generateAnimationTexture(const TextureManager & textureManager, const PlayerSkinColors &colors) const {
        animation::Palette substitution_table(animation.palette);
        int dst[] = { 4, 5, 8, 9, 12, 13, 16, 20, 24, 25, 36, 37 }; //indexes to palette colors used in the man.ase
        int src[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 9, 10 };
        auto & substitution = substitution_table.colors;
        for (int i = 0; i < 12; i++) {
            const auto & color = colors.get((PlayerSkinColors::BodyPart) src[i]);
            substitution[dst[i]].r = color.getRed();
            substitution[dst[i]].g = color.getGreen();
            substitution[dst[i]].b = color.getBlue();
            substitution[dst[i]].a = color.getAlpha();
        }

        auto view = animation.toView();

        view.setLayerVisibility("Hair", colors.getHair() == PlayerSkinColors::Hair::Normal);
        view.setLayerVisibility("Hair_Short", colors.getHair() == PlayerSkinColors::Hair::Short);
        view.setLayerVisibility("Headband", colors.hasHeadBand());

        return textureManager.generateSprite(animation, view, substitution_table, TextureFilter::Nearest, true);
    }
}
