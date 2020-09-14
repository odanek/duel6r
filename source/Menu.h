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

#ifndef DUEL6_MENU_H
#define DUEL6_MENU_H

#include <vector>
#include <unordered_map>
#include <functional>
#include "Type.h"
#include "Context.h"
#include "LevelList.h"
#include "PersonList.h"
#include "PersonProfile.h"
#include "input/PlayerControls.h"
#include "PlayerSkinColors.h"
#include "Video.h"
#include "AppService.h"
#include "Defines.h"
#include "gui/Desktop.h"
#include "gui/View.h"
#include "gui/Button.h"
#include "gui/CheckBox.h"
#include "gui/ListBox.h"
#include "gui/Label.h"
#include "gui/TextBox.h"
#include "gui/Spinner.h"
#include "GameMode.h"
#include "net/ClientGameProxy.h"

using Duel6::net::ClientGameProxy;

namespace Duel6 {
    class Game; // Forward, TODO: Remove

    class Menu : public Context {
    public:
        class Server {

        public:
            std::string address;
            std::string port;
            std::string descr;
            std::string text;
            Server(const std::string &address, const std::string &port, const std::string &descr, const std::string &text)
                : address(address),
                  port(port),
                  descr(descr), text(text) {
            }
        };

        class ServerList {

        public:
            typedef std::vector<Server> list_t;
            typedef std::function<void(list_t&)> callback_t;
            typedef std::function<void()> callbackReset_t;

        private:
            list_t list;
            callback_t defaultCallback = [](list_t&) {
            };
            callback_t callback = defaultCallback;

        public:
            callbackReset_t setCallback(callback_t);
            void clearCallback() {
                callback = defaultCallback;
            }
            void add(const std::string &address, const std::string &port, const std::string &descr, const std::string &text);
            void clear();
            void notify();
            const std::vector<Server>& get() const;

        };
        class NetConfig {
        public:
            std::string masterServer = "duel6-master.mrakonos.cz";
            Int32 masterServerPort = 5902;
            bool enableMasterServer = false;    // single switch to disable any communication with the master
            bool enableMasterDiscovery = false; // publish the game to master server for others to discover
            bool enableNATPunch = false;        // enable NAT traversal (server/client)
        };
    private:
        AppService &appService;
        Font &font;
        Video &video;
        Renderer &renderer;
        Sound &sound;
        Game *game;
        ClientGameProxy *clientGameProxy;
        std::vector<std::unique_ptr<GameMode>> gameModes;
        Gui::Desktop gui;
        Gui::View * mainView;
        PlayerControlsManager& controlsManager;
        PersonProfileList personProfiles;
        PlayerSounds defaultPlayerSounds;
        LevelList levelList;
        PersonList persons;
        Gui::ListBox *personListBox;
        Gui::ListBox *playerListBox;
        Gui::ListBox *scoreListBox;
        Gui::ListBox *eloListBox;
        Gui::Spinner *controlSwitch[D6_MAX_PLAYERS];
        Gui::Spinner *teamControlSwitch[D6_MAX_PLAYERS];
        Gui::Textbox *textbox;
        Gui::Textbox *host;
        Gui::Textbox *port;
        Gui::Spinner *gameModeSwitch;
        Gui::CheckBox *globalAssistanceCheckBox;
        Gui::CheckBox *quickLiquidCheckBox;
        Gui::CheckBox *reverseConnection; //experimental stuff
        Gui::Label *playersLabel;
        Size backgroundCount;
        Texture menuBannerTexture;
        Sound::Track menuTrack;
        bool playMusic;
        NetConfig netConfig;
        ServerList serverList;
    public:
        explicit Menu(AppService &appService);

        ~Menu() override = default;

        void setGameReference(Game &game) {
            this->game = &game;
        }

        void setClientGameProxyReference(ClientGameProxy &clientGameProxy){
            this->clientGameProxy = &clientGameProxy;
        }

        void initialize();

        void joyRescan();

        void savePersonData() const;

        void keyEvent(const KeyPressEvent &event) override;

        void textInputEvent(const TextInputEvent &event) override;

        void mouseButtonEvent(const MouseButtonEvent &event) override;

        void mouseMotionEvent(const MouseMotionEvent &event) override;

        void mouseWheelEvent(const MouseWheelEvent &event) override;

        void joyDeviceAddedEvent(const JoyDeviceAddedEvent & event) override;

        void joyDeviceRemovedEvent(const JoyDeviceRemovedEvent & event) override;

        void update(Float32 elapsedTime) override;

        void render() const override;

        void enableMusic(bool enable);

        std::unordered_map<std::string, std::unique_ptr<PersonProfile>> &getPersonProfiles() {
            return personProfiles;
        }

        std::vector<std::string> listMaps();

        bool play(std::vector<std::string> levels, bool networkGame);

        void startDedicatedServer(const std::string &host, const std::string &port);

        void serverlist();
    private:

        void quit();

        void beforeStart(Context *prevContext) override;

        void beforeClose(Context *nextContext) override;

        void initializeGameModes();

        void showMessage(const std::string &message);

        bool detectControls(Size playerIndex);

        bool play(bool networkGame);

        void playPlayersSound(const std::string &name);

        void loadPersonProfiles(const std::string &path);

        void loadPersonData(const std::string &filePath);

        void loadNetworkSettings(const std::string &filePath);

        void saveNetworkSettings();

        PersonProfile *getPersonProfile(const std::string &name);

        void cleanPersonData();

        void addPerson();

        void deletePerson();

        void addPlayer(Int32 index);

        void removePlayer(Int32 c);

        void updatePlayerCount();

        void rebuildTable();

        bool question(const std::string &question);

        bool question(const std::string &question, bool &cancel);

        bool deleteQuestion();

        int processEvents(bool single = true);

        int processEvents(bool single, bool & cancelled);

        void consumeInputEvents();

        void shufflePlayers();

        void eloShufflePlayers();

        void startServer();

        void joinServer();
    };
}

#endif
