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

    class Menu
            : public Context {
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
        Gui::Spinner *gameModeSwitch;
        Gui::CheckBox *globalAssistanceCheckBox;
        Gui::CheckBox *quickLiquidCheckBox;
        Gui::Label *playersLabel;
        Size backgroundCount;
        Texture menuBannerTexture;
        Sound::Track menuTrack;
        bool playMusic;

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

        void play(std::vector<std::string> levels);

    private:
        void beforeStart(Context *prevContext) override;

        void beforeClose(Context *nextContext) override;

        void initializeGameModes();

        void showMessage(const std::string &message);

        void detectControls(Size playerIndex);

        void play();

        void playPlayersSound(const std::string &name);

        void loadPersonProfiles(const std::string &path);

        void loadPersonData(const std::string &filePath);

        PersonProfile *getPersonProfile(const std::string &name);

        void cleanPersonData();

        void addPerson();

        void deletePerson();

        void addPlayer(Int32 index);

        void removePlayer(Int32 c);

        void updatePlayerCount();

        void rebuildTable();

        bool question(const std::string &question);

        bool deleteQuestion();

        int processEvents(bool single = true);

        void consumeInputEvents();

        void shufflePlayers();

        void eloShufflePlayers();


        void startServer();

        void joinServer();
    };
}

#endif
