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

#ifndef DUEL6_GAME_H
#define DUEL6_GAME_H

#include <utility>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include "Type.h"
#include "ScreenMode.h"
#include "Context.h"
#include "World.h"
#include "Player.h"
#include "WorldRenderer.h"
#include "Water.h"
#include "AppService.h"
#include "GameSettings.h"
#include "GameResources.h"
#include "Round.h"
#include "net/ServerGameProxy.h"
#include "PlayerDefinition.h"
#include "ChatMessageQueue.h"
#include "ChatInput.h"

namespace Duel6 {
    namespace net {
        class ClientGameProxy;
    }
    class GameMode;

    class Menu;

    class Game: public Context {
        friend class Duel6::net::ClientGameProxy;

        class NetStat {
        public:
            uint32_t inBandwidth = 0;
            uint32_t outBandwidth = 0;
            uint32_t inThrottleEpoch = 0;
            uint32_t outThrottleEpoch = 0;
            uint32_t inDataTotal = 0;
            uint32_t outDataTotal = 0;
            uint32_t packetLoss = 0;
            uint32_t packetLossVariance = 0;
            uint32_t packetThrottle = 0;
            uint32_t packetThrottleLimit = 0;
            uint32_t packetThrottleCounter = 0;
            uint32_t packetThrottleInterval = 0;
            uint32_t lastRTT = 0;
            uint32_t lowestRTT = 0;
            uint32_t lastRTTVariance = 0;
            uint32_t highestRTTVariance = 0;
            uint32_t rtt = 0;
            uint32_t rttVariance = 0;
            uint32_t mtu = 0;
            uint32_t windowSize = 0;
            size_t totalWaitingData = 0;
            size_t choke = 0;
        };
    public:
        NetStat netstat;
        uint16_t tick = 65000;
        bool isServer = false;
        bool isRunning = false;
        bool networkGame = false;
    private:
        friend class net::GameProxy;
        friend class net::ClientGameProxy;
        AppService &appService;
        GameResources &resources;
        GameSettings &settings;
        GameMode *gameMode;
        std::unique_ptr<Round> round;
        net::ServerGameProxy * gameProxy;
        WorldRenderer worldRenderer;
        const Menu *menu;

        std::vector<std::string> levels;
        std::vector<Size> backgrounds;

        Int32 currentRound;
        Int32 playedRounds;

        std::vector<Player> players;
        std::vector<PlayerSkin> skins;
        const PlayerAnimations & playerAnimations;
        const AuxAnimations & playerAuxAnimations;
        bool displayScoreTab = false;
        InfoMessageQueue * infoMessageQueue = nullptr;
        ChatMessageQueue chatMessageQueue;
        ChatInput chatInput;
        std::string localChatName;
    public:
        Int32 maxPlayerId = 0;

        Game(AppService &appService, GameResources &resources, GameSettings &settings);
        void setGameProxyReference(net::ServerGameProxy & serverGameProxy) {
            this->gameProxy = &serverGameProxy;
        }
        void start(std::vector<PlayerDefinition> &playerDefinitions, const std::vector<std::string> &levels,
                   const std::vector<Size> &backgrounds, ScreenMode screenMode, Int32 screenZoom, GameMode &gameMode,
                   bool networkGame);

        size_t joinPlayer(PlayerDefinition &playerDefinitions);

        void joinPlayers(std::vector<PlayerDefinition> &playerDefinitions);

        void disconnectPlayers(const std::vector<Int32> & ids);

        void keyEvent(const KeyPressEvent &event) override;

        void textInputEvent(const TextInputEvent &event) override;

        void mouseButtonEvent(const MouseButtonEvent &event) override;

        void mouseMotionEvent(const MouseMotionEvent &event) override;

        void mouseWheelEvent(const MouseWheelEvent &event) override;

        void joyDeviceAddedEvent(const JoyDeviceAddedEvent & event) override;

        void joyDeviceRemovedEvent(const JoyDeviceRemovedEvent & event) override;

        void windowFocusEvent(const WindowFocusEvent & event) override;

        void update(Float32 elapsedTime) override;

        void render() const override;

        std::string getGameState() override;

        AppService &getAppService() const {
            return appService;
        }

        const std::vector<Size> &getBackgrounds() const {
            return backgrounds;
        }

        std::vector<Player> &getPlayers() {
            return players;
        }

        const std::vector<Player> &getPlayers() const {
            return players;
        }

        GameResources &getResources() {
            return resources;
        }

        const GameResources &getResources() const {
            return resources;
        }

        GameSettings &getSettings() {
            return settings;
        }

        const GameSettings &getSettings() const {
            return settings;
        }

        Round &getRound() {
            return *round;
        }

        const Round &getRound() const {
            return *round;
        }

        Int32 getPlayedRounds() const {
            return playedRounds;
        }

        void setPlayedRounds(Int32 playedRounds) {
            this->playedRounds = playedRounds;
        }

        Int32 getCurrentRound() const;

        bool isOver() const {
            return getRound().isLast() && getRound().isOver();
        }

        bool isDisplayingScoreTab() const {
            return displayScoreTab;
        }

        WorldRenderer &getWorldRenderer() {
            return worldRenderer;
        }

        const WorldRenderer &getWorldRenderer() const {
            return worldRenderer;
        }

        GameMode &getMode() {
            return *gameMode;
        }

        const GameMode &getMode() const {
            return *gameMode;
        }

        void setMenuReference(const Menu &menu) {
            this->menu = &menu;
        }

        void onNextRound();

        void spawnWeapon(LyingWeapon &&weapon);

        void spawnBonus(Bonus &&bonus);

        void pickWeapon(Player &player, unsigned int weaponId);

        void pickBonus(Player &player, unsigned int bonusId);

        void spawnRemoteShot(std::unique_ptr<Shot> && shot); // hack (refactor: add `isRemote` flag to shot?)

        void spawnShot(std::unique_ptr<Shot> && shot);

        void raiseWater();

        void eraseShot(Uint16 id);

        void spawnExplosion(Explosion &&explosion);

        void playSample(const Player &player, PlayerSounds::Type type);

        void broadcastMessage(const Player &player, const std::string & msg, bool display);

        void doubleJumpEffect(Player &player, Float32 x, Float32 y, Float32 angle);

        void onDoubleJumpEffect(Player &player, Float32 x, Float32 y, Float32 angle);

        void setMessageQueue(InfoMessageQueue & queue);

        void broadcastChatMessage(const std::string &msg, bool display = false, bool system = false, const std::string & origin = "");

        const ChatMessageQueue& getChatMessageQueue() const {
            return chatMessageQueue;
        }

        const ChatInput& getChatInput() const {
            return chatInput;
        }

        void announcePeerIsInConsole(bool consoleActive);

    private:
        void beforeStart(Context *prevContext) override;

        void beforeClose(Context *nextContext) override;

        void onStartRound(std::unique_ptr<Level> && levelData);

        void startRound();

        void startRound(std::unique_ptr<Level> && levelData, std::function<void()> callback);

        void nextRound();

        void endRound();

        void onRoundEnd();

        void announcePeerIsChatting(bool chatActive);

        void announcePeerFocusChanged(bool focusGained);
    };
}

#endif
