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
            enet_uint32 netAddress;
            enet_uint16 netPort;
            enet_uint32 publicIPAddress;
            enet_uint16 publicPort;
            enet_uint32 localAddress;
            enet_uint16 localPort;
            std::string address;
            std::string port;
            std::string publicIPAddressStr;
            std::string publicPortStr;

            std::string localAddressStr;
            std::string localPortStr;
            std::string descr;
            std::string text;
            bool enableNAT;
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
            void add(enet_uint32 netAddress, enet_uint16 netPort,
                     enet_uint32 publicAddress, enet_uint16 publicPort,
                     enet_uint32 localAddress, enet_uint16 localPort,
                     const std::string &address, const std::string &port,
                     const std::string &publicAddressStr, const std::string &publicPortStr,
                     const std::string &localAddressStr, const std::string &localPortStr,
                     const std::string &descr, const std::string &text,
                     bool enableNAT);
            void clear();
            void notify();
            const std::vector<Server>& get() const;

        };
        class NetConfig {
        private:
            union hostAddress {
                address_t address;
                uint8_t a[4];
            };
        public:
            // address of the master server
            std::string masterServer = "duel6-master.mrakonos.cz";
            Int32 masterServerPort = 5902;

            // single switch to disable any communication with the master
            // TODO (not implemented yet)
            bool enableMasterServer = false;

            // publish the game to master server for others to discover
            bool enableMasterDiscovery = false;

            // enable NAT traversal (server/client)
            //  - client: false means we won't be able to connect to NATed servers
            //  - server: false means we need public IP address for others to be able to contact us
            bool enableNATPunch = false;


            // local IP address for contacting the master server (in case we have more network interfaces in the computer)
            std::string localIPAddress = "0.0.0.0";

            std::string serverDescription = "unnamed server";

            // experimental feature
            // - for public server on a public IP address
            // - but requiring a NAT handshake
            // this is mainly for development purposes
            std::string publicIPAddress = "0.0.0.0";
            Int32 publicPort = 5900;
            enet_uint32 getPublicIPAddress(){
                hostAddress addr;
                addr.address = 0;
                if(publicIPAddress == "0.0.0.0" || publicIPAddress == "any" || publicIPAddress.length() > 15 || publicIPAddress.length() < 7){
                    return 0;
                }
                auto a = 0;
                std::string buffer;
                buffer.reserve(16);
                for(size_t i = 0 ; i < publicIPAddress.length(); i++){
                    if(publicIPAddress.compare(i,1,".") == 0 && a < 4){
                        addr.a[a++] = std::stoi(buffer);
                        buffer.clear();
                    } else {
                        buffer += publicIPAddress[i];
                    }
                }
                addr.a[a] = std::stoi(buffer);
                return addr.address;
            }
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

        void startDedicatedServer();

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

        void joinServerFromServerList(Int32 index);
    };
}

#endif
