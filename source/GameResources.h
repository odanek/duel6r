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

#ifndef DUEL6_GAMERESOURCES_H
#define DUEL6_GAMERESOURCES_H

#include <unordered_map>
#include "Water.h"
#include "Block.h"
#include "AppService.h"

namespace Duel6 {
    class GameResources {
    private:
        Block::Meta blockMeta;
        Sound::Sample gameOverSound;
        Sound::Sample roundStartSound;
        TextureList blockTextures;
        TextureList bcgTextures;
        TextureList explosionTextures;
        TextureList bonusTextures;
        TextureList elevatorTextures;
        std::unordered_map<Size, TextureList> fireTextures;

    public:
        GameResources(AppService &appService);

        const Block::Meta &getBlockMeta() const {
            return blockMeta;
        }

        const Sound::Sample &getGameOverSound() const {
            return gameOverSound;
        }

        const Sound::Sample &getRoundStartSound() const {
            return roundStartSound;
        }

        const TextureList &getBlockTextures() const {
            return blockTextures;
        }

        const TextureList &getBcgTextures() const {
            return bcgTextures;
        }

        const TextureList &getExplosionTextures() const {
            return explosionTextures;
        }

        const TextureList &getBonuseTextures() const {
            return bonusTextures;
        }

        const TextureList &getElevatorTextures() const {
            return elevatorTextures;
        }

        const std::unordered_map<Size, TextureList> &getFireTextures() const {
            return fireTextures;
        }
    };
}

#endif
