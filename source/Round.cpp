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

#include "Round.h"
#include "Game.h"
#include "GameException.h"
#include "GameMode.h"
#include "Weapon.h"
#include "PersonProfile.h"

namespace Duel6 {
    Round::Round(Game &game, Int32 roundNumber, std::unique_ptr<Level> && level)
            : game(game), roundNumber(roundNumber), world(game, std::move(level)),
              suddenDeathMode(false), waterFillWait(0), showYouAreHere(D6_YOU_ARE_HERE_DURATION), gameOverWait(0),
              winner(false), scriptContext(world) {}

    void Round::start() {
        startTime = SDL_GetTicks();
        auto &players = world.getPlayers();
        game.getMode().initializePlayerPositions(game, players, world);
        setPlayerViews();
        game.getMode().initializeRound(game, players, world);
        scriptStart();
        game.getResources().getRoundStartSound().play();
    }

    void Round::end() {
        scriptEnd();

        auto &players = world.getPlayers();
        for (Player &player : players) {
            player.endRound();
        }
    }

    void Round::scriptStart() {
        auto &players = world.getPlayers();
        for (auto &player : players) {
            PersonProfile *profile = player.getPerson().getProfile();
            if (profile != nullptr) {
                auto &personScripts = profile->getScripts();
                for (auto &script : personScripts) {
                    script->roundStart(player, scriptContext);
                }
            }
        }
    }

    void Round::scriptUpdate(Player &player) {
        Uint32 roundTime = SDL_GetTicks() - startTime;
        PersonProfile *profile = player.getPerson().getProfile();
        if (profile != nullptr) {
            auto &personScripts = profile->getScripts();
            for (auto &script : personScripts) {
                script->roundUpdate(roundTime, player, scriptContext);
            }
        }
    }

    void Round::scriptEnd() {
        Uint32 roundTime = SDL_GetTicks() - startTime;
        auto &players = world.getPlayers();
        for (auto &player : players) {
            PersonProfile *profile = player.getPerson().getProfile();
            if (profile != nullptr) {
                auto &personScripts = profile->getScripts();
                for (auto &script : personScripts) {
                    script->roundEnd(roundTime, player, scriptContext);
                }
            }
        }
    }

    void Round::splitScreenView(Player &player, Int32 x, Int32 y) {
        const Video &video = game.getAppService().getVideo();
        PlayerView view(x, y, video.getScreen().getClientWidth() / 2 - 4, video.getScreen().getClientHeight() / 2 - 4);
        player.setView(view);
    }

    void Round::setPlayerViews() {
        const Video &video = game.getAppService().getVideo();
        std::vector<Player> &players = world.getPlayers();

        for (Player &player : players) {
            player.prepareCam(video, game.getSettings().getScreenMode(), game.getSettings().getScreenZoom(),
                              world.getLevel().getWidth(), world.getLevel().getHeight());
        }

        if (game.getSettings().getScreenMode() == ScreenMode::FullScreen) {
            for (Player &player : players) {
                player.setView(
                        PlayerView(0, 0, video.getScreen().getClientWidth(), video.getScreen().getClientHeight()));
            }

            return;
        } else {
            if (players.size() == 2) {
                splitScreenView(players[0], video.getScreen().getClientWidth() / 4 + 2, 2);
                splitScreenView(players[1], video.getScreen().getClientWidth() / 4 + 2,
                                video.getScreen().getClientHeight() / 2 + 2);
            }

            if (players.size() == 3) {
                splitScreenView(players[0], 2, 2);
                splitScreenView(players[1], video.getScreen().getClientWidth() / 2 + 2, 2);
                splitScreenView(players[2], video.getScreen().getClientWidth() / 4 + 2,
                                video.getScreen().getClientHeight() / 2 + 2);
            }

            if (players.size() == 4) {
                splitScreenView(players[0], 2, 2);
                splitScreenView(players[1], video.getScreen().getClientWidth() / 2 + 2, 2);
                splitScreenView(players[2], 2, video.getScreen().getClientHeight() / 2 + 2);
                splitScreenView(players[3], video.getScreen().getClientWidth() / 2 + 2,
                                video.getScreen().getClientHeight() / 2 + 2);
            }
        }
    }

    void Round::checkWinner() {
        std::vector<Player> &allPlayers = world.getPlayers();

        alivePlayers.clear();

        // todo: rewrite to copy_if if it is possible to do it that way without billion lines of compile errors:-)
        for (Player &player : allPlayers) {
            if (player.isAlive() && !player.isDeleted()) {
                alivePlayers.push_back(&player);
            }
        }

        if (!game.networkGame || game.isServer){
            if (!suddenDeathMode && game.getMode().checkForSuddenDeathMode(world, alivePlayers)) {
                suddenDeathMode = true;
            }

            if (game.getMode().checkRoundOver(world, alivePlayers)) {
                roundOver();
            }
        }
    }
    void Round::roundOver() {
        winner = true;
        gameOverWait = D6_GAME_OVER_WAIT;

        game.getResources().getGameOverSound().play();
        onRoundEnd();
    }
    void Round::compensateLag(uint16_t gameTick, uint16_t confirmedTick) {
        static const Float64 elapsedTime = 1.0f / D6_UPDATE_FREQUENCY; // TODO
        isCompensatingLag = true;
//        world.confirmElevators(ticks, elapsedTime);
        for (Player &player : world.getPlayers()) {
            if(player.isDeleted()){
                continue;
            }
            player.compensateLag(world, gameTick, confirmedTick, elapsedTime);
        }

        isCompensatingLag = false;
    }

    void Round::update(Float32 elapsedTime) {
        // Check if there's a winner
        if (game.isServer || !game.networkGame) {
            if (!hasWinner()) {
                checkWinner();
            } else {
                gameOverWait = std::max(gameOverWait - elapsedTime, 0.0f);
                if (gameOverWait < (D6_GAME_OVER_WAIT - D6_ROUND_OVER_WAIT)) {
                    for (Player &player : world.getPlayers()) {
                        if (player.isDeleted()) {
                            continue;
                        }
                        player.tick++;
                    }
                    return;
                }
            }
        }

        for (Player &player : world.getPlayers()) {
            if(player.isDeleted()){
                continue;
            }
            if(player.local){
                player.updateControllerStatus();
                scriptUpdate(player);

                player.update(world, game.getSettings().getScreenMode(), elapsedTime);
                if (game.getSettings().isGhostEnabled() && !player.isInGame() && !player.isGhost()) {
                    player.makeGhost();
                }
            } else {
              //  auto tmp = player.getControllerState();
                bool runElevators = player.lastConfirmedTick == player.tick;
                //Converted to Uint16 to deal with the counter wrap-around at 65535
                while((Uint16)(player.lastConfirmedTick - player.tick) > 0){
                    scriptUpdate(player);
                    player.setControllerState(player.unconfirmedInputs[player.tick % 128]);
                    player.unconfirmedInputs[player.tick % 128] = 0;
                    player.update(world, game.getSettings().getScreenMode(), elapsedTime);
                    if (game.getSettings().isGhostEnabled() && !player.isInGame() && !player.isGhost()) {
                        player.makeGhost();
                    }
                }
                if(runElevators){
                    player.getCollider().collideWithElevator(0.0, 1.0);
                }
            //    player.setControllerState(tmp); // put it back to not replay inputs // commented out - this stops propagating controls between clients from server
            }
        }

        world.update(elapsedTime);
        game.getAppService().getVideo().getRenderer().setGlobalTime(world.getTime());

        if (suddenDeathMode) {
            waterFillWait += elapsedTime;
            if (waterFillWait > D6_RAISE_WATER_WAIT) {
                waterFillWait = 0;
                game.raiseWater();
            }
        }

        showYouAreHere = std::max(showYouAreHere - 3 * elapsedTime, 0.0f);
    }

    void Round::keyEvent(const KeyPressEvent &event) {
        // Switch between fullscreen and split screen mode
        if (event.getCode() == SDLK_F2 && world.getPlayers().size() < 5) {
            switchScreenMode();
        }

        // Turn on/off player statistics
        if (event.getCode() == SDLK_F4) {
            game.getSettings().setShowRanking(!game.getSettings().isShowRanking());
        }

        // Save screenshot
        if (event.getCode() == SDLK_F10) {
            Image image = game.getAppService().getVideo().getRenderer().makeScreenshot();
            std::string name = image.saveScreenshot();
            game.getAppService().getConsole().printLine(Format("Screenshot saved to {0}") << name);
        }
    }

    void Round::switchScreenMode() {
        GameSettings &settings = game.getSettings();
        settings.setScreenMode((settings.getScreenMode() == ScreenMode::FullScreen) ? ScreenMode::SplitScreen
                                                                                    : ScreenMode::FullScreen);
        setPlayerViews();
    }

    bool Round::isOver() const {
        return hasWinner() && gameOverWait <= 0;
    }

    bool Round::isLast() const {
        return game.getSettings().isRoundLimit() && roundNumber + 1 == game.getSettings().getMaxRounds();
    }
}
