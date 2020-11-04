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

#ifndef DUEL6_PLAYERANIMATIONS_H
#define DUEL6_PLAYERANIMATIONS_H

#include <string>
#include "PlayerSkinColors.h"
#include "TextureManager.h"
#include "aseprite/animation.h"

namespace Duel6 {

    class PlayerAnimation {
    private:
        const std::vector<AnimationEntry> animation;

    public:
        PlayerAnimation(const animation::Animation &animation, const std::string &name);

        Animation get() const;
    };


    class AuxAnimation {
    private:
        const std::vector<AnimationEntry> animationEntry;

    public:
        AuxAnimation(const animation::Animation &animation, const std::string &name);
        Animation get() const;
    };

    class AuxAnimations {
    private:
        const animation::Animation animation;
        const animation::Animation doubleJumpAnimation;
        const AuxAnimation chat;
        const AuxAnimation console;
        const AuxAnimation unfocused;
        const AuxAnimation doubleJump;
    public:
        AuxAnimations(animation::Animation &&animation, animation::Animation &&doubleJumpAnimation);
        Texture generateAnimationTexture(const TextureManager &textureManager) const;
        Texture generateDoubleJumpAnimationTexture(const TextureManager &textureManager) const;

        const AuxAnimation & getChat() const;
        const AuxAnimation & getConsole() const;
        const AuxAnimation & getUnfocused() const;
        const AuxAnimation & getDoubleJump() const;
    };

    class PlayerAnimations {
    private:
        const animation::Animation animation;
        const PlayerAnimation stand;
        const PlayerAnimation hitStand;
        const PlayerAnimation walk;
        const PlayerAnimation fall;
        const PlayerAnimation jump;
        const PlayerAnimation duck;
        const PlayerAnimation hitDuck;
        const PlayerAnimation deadFall;
        const PlayerAnimation deadHit;
        const PlayerAnimation deadLying;
        const PlayerAnimation dying;
        const PlayerAnimation pick;

    public:
        PlayerAnimations(animation::Animation &&animation);

        Texture generateAnimationTexture(const TextureManager &textureManager, const PlayerSkinColors &colors) const;

        const PlayerAnimation &getStand() const;

        const PlayerAnimation &getHitStand() const;

        const PlayerAnimation &getWalk() const;

        const PlayerAnimation &getFall() const;

        const PlayerAnimation &getJump() const;

        const PlayerAnimation &getDuck() const;

        const PlayerAnimation &getHitDuck() const;

        const PlayerAnimation &getDeadFall() const;

        const PlayerAnimation &getDeadHit() const;

        const PlayerAnimation &getDeadLying() const;

        const PlayerAnimation &getDying() const;

        const PlayerAnimation &getPick() const;
    };

}

#endif
