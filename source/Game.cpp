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

#include "Sound.h"
#include "WorldRenderer.h"
#include "Game.h"
#include "Menu.h"
#include "GameMode.h"

namespace Duel6 {
#define MAX_PLAYERS 32 // because resizing vectors screwes up pointers to players gamemodes

    Game::Game(AppService &appService, GameResources &resources, GameSettings &settings)
        : appService(appService),
          resources(resources),
          settings(settings),
          worldRenderer(appService, *this),
          playedRounds(0) {
        players.reserve(MAX_PLAYERS);
    }

    void Game::beforeStart(Context *prevContext) {
        SDL_ShowCursor(SDL_DISABLE);
    }

    void Game::beforeClose(Context *nextContext) {
        endRound();
    }

    void Game::render() const {
        worldRenderer.render();
    }

    void Game::update(Float32 elapsedTime) {
        if (getRound().isOver()) {
            if (!getRound().isLast()) {
                nextRound();

            }
        } else {
            getRound().update(elapsedTime);
            gameProxy->sendGameState(*this);
        }
    }

    void Game::keyEvent(const KeyPressEvent &event) {
        if (event.getCode() == SDLK_ESCAPE && (isOver() || event.withShift())) {
            close();
            return;
        }
        if (event.getCode() == SDLK_TAB && (!getRound().hasWinner())) {
            displayScoreTab = !displayScoreTab;
        }

        if (!getRound().isLast()) {
            if (event.getCode() == SDLK_F1 && (getRound().hasWinner() || event.withShift())) {
                nextRound();
                return;
            }
            if (getRound().hasWinner() && ((D6_GAME_OVER_WAIT - getRound().getRemainingGameOverWait()) > 3.0f)) {
                nextRound();
                return;
            }
        }

        getRound().keyEvent(event);
    }

    void Game::textInputEvent(const TextInputEvent &event) {
    }

    void Game::mouseButtonEvent(const MouseButtonEvent &event) {
    }

    void Game::mouseMotionEvent(const MouseMotionEvent &event) {
    }

    void Game::mouseWheelEvent(const MouseWheelEvent &event) {
    }

    void Game::joyDeviceAddedEvent(const JoyDeviceAddedEvent &event) {
    }

    void Game::joyDeviceRemovedEvent(const JoyDeviceRemovedEvent &event) {
    }

    void Game::start(std::vector<PlayerDefinition> &playerDefinitions, const std::vector<std::string> &levels,
                     const std::vector<Size> &backgrounds,
                     ScreenMode screenMode, Int32 screenZoom,
                     GameMode &gameMode) {
        Console &console = appService.getConsole();
        console.printLine("\n=== Starting new game ===");
        console.printLine(Format("...Rounds: {0}") << settings.getMaxRounds());
        TextureManager &textureManager = appService.getTextureManager();
        players.clear();

        for (auto &skin : skins) {
            textureManager.dispose(skin.getTexture());
        }
        skins.clear();
        gameMode.initializePlayers(playerDefinitions);
        maxPlayerId = 0;
        //players.reserve(playerDefinitions.size());
        playerAnimations = std::make_unique<PlayerAnimations>(resources.getPlayerAnimation());
        for (const PlayerDefinition &playerDef : playerDefinitions) {
            console.printLine(Format("...Generating player for person: {0}") << playerDef.getPerson().getName());
            skins.push_back(PlayerSkin(playerDef.getColors(), textureManager, *playerAnimations));
            Player & p = players.emplace_back(
                playerDef.getPerson(),
                skins.back(),
                playerDef.getSounds(),
                playerDef.getControls(),
                maxPlayerId++,
                playerDef.getTeam());
            p.local = true;
        }

        this->levels = levels;
        std::shuffle(this->levels.begin(), this->levels.end(), Math::randomEngine);

        this->backgrounds = backgrounds;
        this->gameMode = &gameMode;
        settings.setScreenMode(screenMode);
        settings.setScreenZoom(screenZoom);
        gameMode.initializeGame(*this, players, settings.isQuickLiquid(), settings.isGlobalAssistances());
        startRound();
    }
    void Game::joinPlayers(std::vector<PlayerDefinition> &playerDefinitions) {
        gameMode->initializePlayers(playerDefinitions);
        for(auto & d : playerDefinitions){
            joinPlayer(d);
        }
        gameMode->initializePlayersMidGame(players);
        gameMode->initializePlayerPositions(*this, players, this->round->getWorld() );
    }

    void Game::joinPlayer(PlayerDefinition &playerDefinition) { // TODO private

        Console &console = appService.getConsole();
        TextureManager &textureManager = appService.getTextureManager();
        console.printLine(Format("...Player joined ... Generating player for person: {0}") << playerDefinition.getPerson().getName());
        Int32 pos = 0;
        for (const auto &player : players) {
            if (player.isDeleted()) {
                skins[pos] = PlayerSkin(playerDefinition.getColors(), textureManager, *playerAnimations);
                players[pos] = {playerDefinition.getPerson(),
                    skins[pos],
                    playerDefinition.getSounds(),
                    playerDefinition.getControls(),
                    maxPlayerId++,
                    playerDefinition.getTeam()};
                return;
            }
            pos++;
        }
        skins.push_back(PlayerSkin(playerDefinition.getColors(), textureManager, *playerAnimations));
        players.emplace_back(playerDefinition.getPerson(),
            skins.back(),
            playerDefinition.getSounds(),
            playerDefinition.getControls(),
            maxPlayerId++,
            playerDefinition.getTeam());

    }

    void Game::disconnectPlayers(std::vector<Int32> ids) {
        for(auto & id: ids) {
            for (auto &player : players) {
                if(player.getId() == id) {
                    player.setDeleted(true);
                }
            }
        }
    }

    void Game::startRound() {
        currentRound = playedRounds;
        displayScoreTab = false;

        bool shuffle = settings.getLevelSelectionMode() == LevelSelectionMode::Shuffle;
        Int32 level = shuffle ? playedRounds % Int32(levels.size()) : Math::random(Int32(levels.size()));
        const std::string levelPath = levels[level];
        bool mirror = Math::random(2) == 0;

        Console &console = appService.getConsole();
        console.printLine(Format("\n===Loading level {0}===") << levelPath);
        console.printLine(Format("...Parameters: mirror: {0}") << mirror);

        std::unique_ptr<Level> levelData;
        levelData = std::make_unique<Level>(levelPath, mirror, resources.getBlockMeta());
 // todo netgame

        round = std::make_unique<Round>(*this, playedRounds, std::move(levelData));
        round->setOnRoundEnd([this]() {
            onRoundEnd();
        });
        round->start();
        //TODO NET send roundstart (players pos)
        worldRenderer.prerender();
    }

    void Game::endRound() {
        round->end();
    }

    void Game::onRoundEnd() {
        playedRounds++;
        if (round->isLast()) {
            getMode().updateElo(players);
        }
        menu->savePersonData();
    }
    void Game::onNextRound() {
        endRound();
        startRound();
    }

    void Game::nextRound() {
        if (isServer) {
            gameProxy->nextRound();
            onNextRound();
        }
    }

    Int32 Game::getCurrentRound() const {
        return currentRound;
    }
}
