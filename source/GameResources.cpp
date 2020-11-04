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

#include "GameResources.h"
#include "Defines.h"
#include "Fire.h"
#include "Weapon.h"
#include "Bonus.h"
#include "PlayerAnimations.h"
namespace Duel6 {
    void GameResources::load(Console &console, Sound &sound, TextureManager &textureManager) {
        console.printLine("\n===Initializing game resources===\n");
        console.printLine("...Animations initialization");
        std::string animationBasePath(D6_TEXTURE_MAN_PATH);
        playerAnimations = std::make_unique<PlayerAnimations>(std::move(textureManager.loadAnimation(animationBasePath + "man.ase")));
        auxAnimations = std::make_unique<AuxAnimations>(std::move(textureManager.loadAnimation(animationBasePath + "chat.ase")), std::move(textureManager.loadAnimation(animationBasePath + "djump.ase")));
        console.printLine("...Weapon initialization");
        Weapon::initialize(sound, textureManager, *playerAnimations, *auxAnimations);
        console.printLine("...Building water-list");
        Water::initialize(sound, textureManager);
        console.printLine("...Loading game sounds");
        roundStartSound = sound.loadSample("sound/game/round-start.wav");
        gameOverSound = sound.loadSample("sound/game/game-over.wav");
        chatMessageSound = sound.loadSample("sound/game/chatmsg.ogg");
        console.printLine(Format("...Loading block meta data: {0}") << D6_FILE_BLOCK_META);
        blockMeta = Block::loadMeta(D6_FILE_BLOCK_META);
        console.printLine(Format("...Loading block textures: {0}") << D6_TEXTURE_BLOCK_PATH);
        blockTextures = textureManager.loadStack(D6_TEXTURE_BLOCK_PATH, TextureFilter::Linear, true);
        console.printLine(Format("...Loading explosion textures: {0}") << D6_TEXTURE_EXPL_PATH);
        explosionTextures = textureManager.loadStack(D6_TEXTURE_EXPL_PATH, TextureFilter::Nearest, true);
        console.printLine(Format("...Loading bonus textures: {0}") << D6_TEXTURE_EXPL_PATH);
        bonusTextures = textureManager.loadStack(D6_TEXTURE_BONUS_PATH, TextureFilter::Linear, true);
        console.printLine(Format("...Loading elevator textures: {0}") << D6_TEXTURE_ELEVATOR_PATH);
        elevatorTextures = textureManager.loadStack(D6_TEXTURE_ELEVATOR_PATH, TextureFilter::Linear, true);

        console.printLine(Format("...Loading background textures: {0}") << D6_TEXTURE_BCG_PATH);
        bcgTextures = textureManager.loadDict(D6_TEXTURE_BCG_PATH, TextureFilter::Linear, true);

        console.printLine(Format("...Loading fire textures: {0}") << D6_TEXTURE_FIRE_PATH);
        for (const FireType &fireType : FireType::values()) {
            Texture texture = textureManager.loadStack(Format("{0}{1,3|0}/") << D6_TEXTURE_FIRE_PATH << fireType.getId(),
                                                       TextureFilter::Nearest, true);
            fireTextures[fireType.getId()] = texture;
        }

        Texture burn = textureManager.loadStack("textures/fire/burn/", TextureFilter::Linear, true);
        burningTexture = burn;
    }

    const PlayerAnimations& GameResources::getPlayerAnimations() const {
        return *playerAnimations;
    }

    const AuxAnimations& GameResources::getAuxAnimations() const {
        return *auxAnimations;
    }
}
