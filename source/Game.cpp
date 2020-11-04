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

#include <sstream>
#include "Sound.h"
#include "WorldRenderer.h"
#include "Game.h"
#include "Menu.h"
#include "GameMode.h"
#include "json/JsonParser.h"
#include "json/JsonWriter.h"

namespace Duel6 {
#define INITIAL_PLAYERS_POSITIONS 32 // initial size (we can actually serve much more players (>90))

    Game::Game(AppService &appService, GameResources &resources, GameSettings &settings)
        : appService(appService),
          resources(resources),
          settings(settings),
          worldRenderer(appService, *this),
          playedRounds(0),
          playerAnimations(resources.getPlayerAnimations()),
          playerAuxAnimations(resources.getAuxAnimations()),
          chatMessageQueue(10),
          chatInput(appService.getInput()){
        players.reserve(INITIAL_PLAYERS_POSITIONS);
        skins.reserve(INITIAL_PLAYERS_POSITIONS);
    }

    void Game::beforeStart(Context *prevContext) {
        SDL_ShowCursor(SDL_DISABLE);
    }

    void Game::beforeClose(Context *nextContext) {
        endRound();
        isRunning = false;
        if(networkGame){
            if(isServer){
                gameProxy->gameEnded();
            }
            appService.getNetHost().requestStop();
            appService.getNetClient().requestStop();
        }
    }

    void Game::render() const {
        if(!isRunning) return;
        worldRenderer.render();
    }

    void Game::update(Float32 elapsedTime) {
        if(!isRunning) return;
        tick++;

        if ((isServer || !networkGame) && getRound().isOver()) {
            if (!getRound().isLast()) {
                nextRound();
            }
        } else {
            getRound().update(elapsedTime);
        }
        gameProxy->sendGameStateUpdate(*this);
        chatMessageQueue.update(elapsedTime);
    }

    void Game::announcePeerIsChatting(bool chatOpened) {
        if(!networkGame) {
            return;
        }
        gameProxy->chat(chatOpened);
        if(isServer){
            for(auto & player: players){
                if(player.local){
                    player.setChatting(chatOpened);
                }
            }
        }
    }

    void Game::announcePeerIsInConsole(bool consoleOpened) {
        if(!networkGame) {
            return;
        }
        gameProxy->console(consoleOpened);
        if(isServer){
            for(auto & player: players){
                if(player.local){
                    player.setInConsole(consoleOpened);
                }
            }
        }
    }

    void Game::announcePeerFocusChanged(bool focusGained) {
        if (!networkGame) {
            return;
        }
        gameProxy->focus(focusGained);

        if (isServer) {
            for (auto &player : players) {
                if (player.local) {
                    player.setFocused(focusGained);
                }
            }
        }
    }

    void Game::keyEvent(const KeyPressEvent &event) {
        if(!isRunning) return;
        if(chatInput.active() && event.getCode() == SDLK_ESCAPE){
            chatInput.toggle();
            announcePeerIsChatting(false);
            return;
        }
        if (event.getCode() == SDLK_ESCAPE && (isOver() || event.withShift())) {
            close();
            return;
        }
        if (event.getCode() == SDLK_TAB && (!getRound().hasWinner())) {
            displayScoreTab = !displayScoreTab;
        }

        if (event.getCode() == SDLK_F5){
            if(chatInput.toggle()){
                SDL_StartTextInput();
                chatInput.clear();
                announcePeerIsChatting(true);
            } else {
                SDL_StopTextInput();
                announcePeerIsChatting(false);
            }
        }

        if (chatInput.active() && event.getCode() == SDLK_RETURN) {
            broadcastChatMessage(chatInput.getText());
            chatInput.toggle();
            announcePeerIsChatting(false);
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

        if(chatInput.active()){
            chatInput.keyEvent(event);
        }

        getRound().keyEvent(event);
    }

    void Game::textInputEvent(const TextInputEvent &event) {
        if(chatInput.active()){
            chatInput.textInputEvent(event.getText());
        }
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

    void Game::windowFocusEvent(const WindowFocusEvent & event){
        if(isRunning && networkGame){
            announcePeerFocusChanged(event.hasFocus());
        }
    }

    void Game::start(std::vector<PlayerDefinition> &playerDefinitions, const std::vector<std::string> &levels,
                     const std::vector<Size> &backgrounds,
                     ScreenMode screenMode, Int32 screenZoom,
                     GameMode &gameMode,
                     bool networkGame) {
        localChatName = "";
        if(playerDefinitions.size() > 0){
            localChatName = playerDefinitions[0].getPerson().getName();
            if(playerDefinitions.size() > 1){
                std::stringstream ss;
                ss << " (+" << (playerDefinitions.size() - 1) << ")";
                localChatName += ss.str();
            }
        }
        chatMessageQueue.clear();
        tick = 65000; // debug
        this->networkGame = networkGame;
        Console &console = appService.getConsole();
        console.printLine("\n=== Starting new game ===");
        console.printLine(Format("...Rounds: {0}") << settings.getMaxRounds());
        TextureManager &textureManager = appService.getTextureManager();
        players.clear();

        for (auto &skin : skins) {
            textureManager.dispose(skin.getTexture());
            textureManager.dispose(skin.getAuxTexture());
        }
        skins.clear();
        gameMode.initializePlayers(playerDefinitions);
        maxPlayerId = 0;
        //players.reserve(playerDefinitions.size());

        for (const PlayerDefinition &playerDef : playerDefinitions) {
            console.printLine(Format("...Generating player for person: {0}") << playerDef.getPerson().getName());
            skins.push_back(PlayerSkin(playerDef.getColors(), textureManager, playerAnimations, playerAuxAnimations));
            Player & p = players.emplace_back(this,
                playerDef.getPerson(),
                skins.back(),
                playerDef.getSounds(),
                playerDef.getControls(),
                maxPlayerId,
                playerDef.getTeam(), 0, maxPlayerId, maxPlayerId);
            maxPlayerId++;

            p.local = true;
        }

        this->levels = levels;
        std::shuffle(this->levels.begin(), this->levels.end(), Math::randomEngine);

        this->backgrounds = backgrounds;
        this->gameMode = &gameMode;
        settings.setScreenMode(screenMode);
        settings.setScreenZoom(screenZoom);
        gameMode.initializeGame(*this, players, settings.isQuickLiquid(), settings.isGlobalAssistances());
        if(isServer || !networkGame){
            startRound();
        }
        if(networkGame && !isServer){
            appService.getConsole().printLine("Awaiting server to accept connection.");
        }
    }
    void Game::joinPlayers(std::vector<PlayerDefinition> &playerDefinitions) {
        gameMode->initializePlayers(playerDefinitions);
        std::vector<size_t> added;
        added.reserve(playerDefinitions.size());
        for(auto & d : playerDefinitions){
            added.push_back(joinPlayer(d));
        }
        gameMode->joinPlayers(*this, players, added, this->round->getWorld());
     //   gameMode->initializePlayerPositions(*this, players, this->round->getWorld() );
        if(isServer){
            gameProxy->playersJoined(playerDefinitions);
        }
    }

    size_t Game::joinPlayer(PlayerDefinition &playerDefinition) { // TODO private
        Console &console = appService.getConsole();
        TextureManager &textureManager = appService.getTextureManager();
        console.printLine(Format("...Player joined ... Generating player for person: {0}") << playerDefinition.getPerson().getName());
        size_t pos = 0;
        bool authoritative = isServer || !networkGame;
        Int32 playerId = authoritative ? maxPlayerId++ : playerDefinition.getPlayerId();
        for (const auto &player : players) {
            if (player.isDeleted()) {
                textureManager.dispose(skins[pos].getTexture());
                textureManager.dispose(skins[pos].getAuxTexture());
                skins[pos] = PlayerSkin(playerDefinition.getColors(), textureManager, playerAnimations,  playerAuxAnimations);
                players[pos] = {this, playerDefinition.getPerson(),
                    skins[pos],
                    playerDefinition.getSounds(),
                    playerDefinition.getControls(),
                    playerId,
                    playerDefinition.getTeam(),
                    playerDefinition.getClientId(),
                    playerDefinition.getClientLocalId(),
                    pos
                };
                if(authoritative){
                    playerDefinition.setPlayerId(playerId);
                }
                playerDefinition.playerPos = pos;
                return pos;
            }
            pos++;
        }
        skins.push_back(PlayerSkin(playerDefinition.getColors(), textureManager, playerAnimations, playerAuxAnimations));
        players.emplace_back(this,
            playerDefinition.getPerson(),
            skins.back(),
            playerDefinition.getSounds(),
            playerDefinition.getControls(),
            playerId,
            playerDefinition.getTeam(),
            playerDefinition.getClientId(),
            playerDefinition.getClientLocalId(),
            pos);

        if(authoritative){
            playerDefinition.setPlayerId(playerId);
        }
        playerDefinition.playerPos = pos;

        return pos;

    }

    void Game::disconnectPlayers(const std::vector<Int32> & ids) {
        for(auto & id: ids) {
            for (auto &player : players) {
                if(player.getId() == id) {
                    player.setDeleted(true);
                }
            }
        }
        if(isServer){
            gameProxy->playersDisconnected(ids);
        }
    }

    //client
    void Game::onStartRound(std::unique_ptr<Level> && levelData) {
        levelData->setBlockMeta(resources.getBlockMeta());
        startRound(std::move(levelData), [](){});
    }

    void Game::startRound(std::unique_ptr<Level> && levelData, std::function<void()> callback){
        currentRound = playedRounds;
        displayScoreTab = false;
        round = std::make_unique<Round>(*this, playedRounds, std::move(levelData));
        round->setOnRoundEnd(callback);
        round->start();
        isRunning = true;
        //TODO NET send roundstart (players pos)
        worldRenderer.prerender();
    }
    //server
    void Game::startRound() {
        bool shuffle = settings.getLevelSelectionMode() == LevelSelectionMode::Shuffle;
        Int32 level = shuffle ? playedRounds % Int32(levels.size()) : Math::random(Int32(levels.size()));
        const std::string levelPath = levels[level];
        bool mirror = Math::random(2) == 0;

        Console &console = appService.getConsole();
        console.printLine(Format("\n===Loading level {0}===") << levelPath);
        console.printLine(Format("...Parameters: mirror: {0}") << mirror);

        std::unique_ptr<Level> levelData;
        levelData = std::make_unique<Level>(levelPath, mirror, resources.getBlockMeta());

        if(isServer){
            gameProxy->startRound(playedRounds, *levelData);
        }
        startRound(std::move(levelData), [this]() {
            onRoundEnd();
        });
        // todo netgame
    }

    void Game::endRound() {
        if(!isRunning) return;
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
        if (isServer || !networkGame) {
            startRound();
        }
    }

    void Game::nextRound() {
        if (isServer) {
            gameProxy->nextRound();
        } else {
            gameProxy->requestNextRound(currentRound);
        }
        onNextRound();
    }

    Int32 Game::getCurrentRound() const {
        return currentRound;
    }

    void Game::spawnBonus(Bonus &&bonus) {
        if (isServer) {
            gameProxy->spawnBonus(bonus);
        }
        round->getWorld().getBonusList().spawnBonus(std::move(bonus));
    }
    void Game::spawnRemoteShot(std::unique_ptr<Shot> && shot) {
        auto shotCentre = shot->getCentre();
        float panning = shotCentre.x / round->getWorld().getLevel().getWidth() * 2.0f - 1.0f;
        shot->getWeapon().playShotSample(panning);
        round->getWorld().getShotList().spawnShot(std::move(shot));

    }
    void Game::spawnShot(std::unique_ptr<Shot> && shot) {
        if (isServer) {
            gameProxy->spawnShot(shot);
        }
        round->getWorld().getShotList().spawnShot(std::move(shot));
    }
    void Game::spawnWeapon(LyingWeapon &&weapon) {
        if (isServer) {
            gameProxy->spawnWeapon(weapon);
        }
        round->getWorld().getBonusList().spawnWeapon(std::move(weapon));
    }

    void Game::pickWeapon(Player &player, unsigned int weaponId) {
        if (isServer) {
            gameProxy->pickWeapon(player, weaponId);
        }
        round->getWorld().getBonusList().pickWeapon(player, weaponId);
    }

    void Game::pickBonus(Player &player, unsigned int bonusId) {
        if (isServer) {
            gameProxy->pickBonus(player, bonusId);
        }
        round->getWorld().getBonusList().pickBonus(player, bonusId);
    }

    void Game::raiseWater() {
        if (isServer) {
            gameProxy->raiseWater();
        }
        round->getWorld().raiseWater();
    }

    void Game::eraseShot(Uint16 id) {
        if(isServer){
            gameProxy->eraseShot(id);
        }
        round->getWorld().getShotList().eraseShot(id, round->getWorld());
    }

    void Game::spawnExplosion(Explosion &&explosion) {
        if (isServer) {
            gameProxy->spawnExplosion(explosion);
        }
        round->getWorld().getExplosionList().spawn(std::move(explosion));
    }

    void Game::playSample(const Player &player, PlayerSounds::Type type) {
        if (isServer) {
            gameProxy->playSample(player.getId(), type);
        }
        player.playSample(type);
    }

    void Game::doubleJumpEffect(Player &player, Float32 x, Float32 y, Float32 angle) {
        if (isServer) {
            gameProxy->doubleJumpEffect(player.getId(), x, y, angle);
        }
        player.onDoubleJumpEffect(x, y, angle);
    }

    void Game::onDoubleJumpEffect(Player &player, Float32 x, Float32 y, Float32 angle) {
        if(!player.local || (player.compensatedUntilTick - player.lastDJumpTick > 10)){
            player.onDoubleJumpEffect(x, y, angle);
        }
    }

    void Game::broadcastMessage(const Player &player, const std::string & msg, bool display) {
        if (isServer) {
            gameProxy->broadcastMessage(player.getId(), msg);
        }
        if(isServer || !networkGame || display){
            this->infoMessageQueue->onBroadcast(player, msg);
        }
    }

    void Game::broadcastChatMessage(const std::string &msg, bool display, bool system, const std::string & origin) {
        if (!display || isServer) {
            gameProxy->broadcastChatMessage(msg, system, (display || system) ? origin : localChatName);
        }
        if(isServer || !networkGame || display){
            if(!system){
                resources.getChatMsgSound().play();
            }
            appService.getConsole().printLine(((display || system) ? origin : localChatName) + ":" + msg);
            this->chatMessageQueue.add(system, (display || system) ? origin : localChatName, msg);
        }
    }

    void Game::setMessageQueue(InfoMessageQueue & queue) {
        this->infoMessageQueue = &queue;
    }


    std::string Game::getGameState(){
        Json::Value json = Json::Value::makeObject();
        json.set("state", Json::Value::makeString("GAME"));
        json.set("round", Json::Value::makeNumber(currentRound));
        json.set("maxRounds", Json::Value::makeNumber(settings.getMaxRounds()));
        json.set("roundLimit", Json::Value::makeBoolean(settings.isRoundLimit()));
        json.set("waterRising", Json::Value::makeBoolean(round  && round->getWorld().getLevel().isRaisingWater()));

        Json::Value playersJson = Json::Value::makeArray();
        for(const auto & player: players){
            if(player.isDeleted()){
                continue;
            }

            Json::Value playerJson = Json::Value::makeObject();

            playerJson.set("name", Json::Value::makeString(player.getPerson().getName()));
            playerJson.set("team", Json::Value::makeNumber(player.getTeam()));
            playerJson.set("ping", Json::Value::makeNumber((Int32) player.rtt));
            playerJson.set("reloadTime", Json::Value::makeNumber(player.getReloadTime()));
            playerJson.set("reloadInterval", Json::Value::makeNumber(player.getReloadInterval()));
            playerJson.set("alive", Json::Value::makeBoolean(player.isAlive()));
            playerJson.set("timeSinceHit", Json::Value::makeNumber(player.getTimeSinceHit()));
            playerJson.set("health", Json::Value::makeNumber(player.getLife()));
            playerJson.set("air", Json::Value::makeNumber(player.getAir()));
            playerJson.set("points", Json::Value::makeNumber(player.getPerson().getTotalPoints()));
            playerJson.set("kills", Json::Value::makeNumber(player.getPerson().getKills()));
            playerJson.set("deaths", Json::Value::makeNumber(player.getPerson().getDeaths()));
            playerJson.set("roundKills", Json::Value::makeNumber(player.getRoundKills()));
            playerJson.set("bonus", Json::Value::makeString(player.getBonus()->getName()));
            playerJson.set("bonusRemainingTime", Json::Value::makeNumber(player.getBonusRemainingTime()));
            playerJson.set("ammo", Json::Value::makeNumber(player.getAmmo()));
            playerJson.set("weapon", Json::Value::makeString((player.hasGun() ? player.getWeapon().getName() : "unarmed")));

            playersJson.add(playerJson);
        }

        json.set("players", playersJson);
        Json::Writer writer(true);
        return writer.writeToString(json);
    }

}
