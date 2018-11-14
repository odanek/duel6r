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

#include <stdlib.h>
#include "Sound.h"
#include "Video.h"
#include "PlayerControls.h"
#include "TextureManager.h"
#include "Menu.h"
#include "Game.h"
#include "Util.h"
#include "File.h"
#include "Font.h"
#include "json/JsonParser.h"
#include "json/JsonWriter.h"
#include "GameMode.h"
#include "gamemodes/DeathMatch.h"
#include "gamemodes/TeamDeathMatch.h"
#include "gamemodes/Predator.h"

#define D6_ALL_CHR  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 -=\\~!@#$%^&*()_+|[];',./<>?:{}"

namespace Duel6 {
    Menu::Menu(AppService &appService)
            : appService(appService), font(appService.getFont()), video(appService.getVideo()),
              sound(appService.getSound()), controlsManager(appService.getControlsManager()),
              defaultPlayerSounds(PlayerSounds::makeDefault(sound)), playMusic(false) {}

    void Menu::loadPersonData(const std::string &filePath) {
        if (!File::exists(filePath)) {
            return;
        }

        Json::Parser parser;
        Json::Value json = parser.parse(filePath);
        persons.fromJson(json.get("persons"), personProfiles);

        for (const Person &person : persons.list()) {
            personListBox->addItem(person.getName());
        }

        Json::Value playing = json.get("playing");
        for (Size i = 0; i < playing.getLength(); i++) {
            std::string name = playing.get(i).asString();
            playerListBox->addItem(name);
            personListBox->removeItem(name);
        }
    }

    void Menu::joyRescan() {
        controlsManager.detectJoypads();

        Size controls = controlsManager.getNumAvailable();
        for (Size i = 0; i < D6_MAX_PLAYERS; i++) {
            Gui::Spinner *control = controlSwitch[i];
            for (Size j = 0; j < controls; j++) {
                control->addItem(controlsManager.get(j).getDescription(), j);
            }
        }
    }

    void Menu::initialize() {
        appService.getConsole().printLine("\n===Menu initialization===");
        menuBannerTexture = appService.getTextureManager().loadList(D6_TEXTURE_MENU_PATH, TextureFilter::LINEAR, true);
        appService.getConsole().printLine("...Starting GUI library");
        gui.screenSize(video.getScreen().getClientWidth(), video.getScreen().getClientHeight(),
                       (video.getScreen().getClientWidth() - 800) / 2, (video.getScreen().getClientHeight() - 700) / 2);

        scoreListBox = new Gui::ListBox(gui, true);
        scoreListBox->setPosition(10, 199, 97, 12, 16);

        personListBox = new Gui::ListBox(gui, true);
        personListBox->setPosition(10, 539, 20, 15, 18);
        personListBox->onDoubleClick([this](Int32 index, const std::string &item) {
            addPlayer(index);
        });

        playerListBox = new Gui::ListBox(gui, false);
        playerListBox->setPosition(200, 541, 15, D6_MAX_PLAYERS, 18);
        playerListBox->onDoubleClick([this](Int32 index, const std::string &item) {
            removePlayer(index);
        });

        levelListBox = new Gui::ListBox(gui, true);
        levelListBox->setPosition(654, 475, 17, 15, 16);

        screenModeListBox = new Gui::ListBox(gui, false);
        screenModeListBox->setPosition(654, 541, 19, 2, 16);
        screenModeListBox->addItem("Fullscreen");
        screenModeListBox->addItem("Split screen");
        screenModeListBox->selectItem(0);

        loadPersonProfiles(D6_FILE_PROFILES);
        loadPersonData(D6_FILE_PHIST);

        auto addPlayerButton = new Gui::Button(gui);
        addPlayerButton->setPosition(200, 253, 80, 25);
        addPlayerButton->setCaption(">>");
        addPlayerButton->onClick([this](Gui::Button &) {
            addPlayer(personListBox->selectedIndex());
        });

        auto removePlayerButton = new Gui::Button(gui);
        removePlayerButton->setPosition(105, 253, 85, 25);
        removePlayerButton->setCaption("<<");
        removePlayerButton->onClick([this](Gui::Button &) {
            removePlayer(playerListBox->selectedIndex());
        });

        auto removePersonButton = new Gui::Button(gui);
        removePersonButton->setPosition(10, 253, 90, 25);
        removePersonButton->setCaption("Remove");
        removePersonButton->onClick([this](Gui::Button &) {
            deletePerson();
            rebuildTable();
        });

        auto addPersonButton = new Gui::Button(gui);
        addPersonButton->setPosition(284, 253, 80, 25);
        addPersonButton->setCaption("Add");
        addPersonButton->onClick([this](Gui::Button &) {
            addPerson();
        });

        auto playButton = new Gui::Button(gui);
        playButton->setPosition(350, 0, 150, 50);
        playButton->setCaption("Play (F1)");
        playButton->onClick([this](Gui::Button &) {
            play();
        });

        auto clearButton = new Gui::Button(gui);
        clearButton->setPosition(505, 0, 150, 50);
        clearButton->setCaption("Clear (F3)");
        clearButton->onClick([this](Gui::Button &) {
            if (deleteQuestion()) {
                cleanPersonData();
            }
        });

        auto quitButton = new Gui::Button(gui);
        quitButton->setPosition(660, 0, 150, 50);
        quitButton->setCaption("Quit (ESC)");
        quitButton->onClick([this](Gui::Button &) {
            close();
        });

        auto scoreLabel = new Gui::Label(gui);
        scoreLabel->setPosition(10, 219, 800, 18);
        scoreLabel->setCaption(
                "    Name   | Games | Wins | Shots | Acc. | Kills | Assists | Pen | PTS | Alive | Damage  | Time");

        auto levelLabel = new Gui::Label(gui);
        levelLabel->setPosition(654, 494, 155, 18);
        levelLabel->setCaption("Level");

        auto screenModeLabel = new Gui::Label(gui);
        screenModeLabel->setPosition(654, 560, 155, 18);
        screenModeLabel->setCaption("Screen mode");

        auto personsLabel = new Gui::Label(gui);
        personsLabel->setPosition(10, 560, 181, 18);
        personsLabel->setCaption("Persons");

        playersLabel = new Gui::Label(gui);
        playersLabel->setPosition(200, 560, 105, 18);

        updatePlayerCount();

        Gui::Button *shuffleButton = new Gui::Button(gui);
        shuffleButton->setCaption("S");
        shuffleButton->setPosition(307, 560, 17, 17);
        shuffleButton->onClick([this](Gui::Button &) {
            shufflePlayers();
        });

        auto controllerLabel = new Gui::Label(gui);
        controllerLabel->setPosition(330, 560, 192, 18);
        controllerLabel->setCaption("Controller");

        textbox = new Gui::Textbox(gui);
        textbox->setPosition(370, 252, 14, 10, D6_ALL_CHR);

        // Player controls
        for (Size i = 0; i < D6_MAX_PLAYERS; i++) {
            controlSwitch[i] = new Gui::Spinner(gui);
            controlSwitch[i]->setPosition(330, 539 - Int32(i) * 18, 192, 0);

            Gui::Button *button = new Gui::Button(gui);
            button->setCaption("D");
            button->setPosition(526, 537 - Int32(i) * 18, 17, 17);
            button->onClick([this, i](Gui::Button &) {
                detectControls(i);
            });
        }

        // Button to detect all user's controllers in a batch
        Gui::Button *button = new Gui::Button(gui);
        button->setCaption("D");
        button->setPosition(526, 558, 17, 17);
        button->onClick([this](Gui::Button &) {
            joyRescan();
            Size curPlayersCount = playerListBox->size();
            for (Size j = 0; j < curPlayersCount; j++) {
                detectControls(j);
            }
        });

        initializeGameModes();
        gameModeSwitch = new Gui::Spinner(gui);
        for (auto &gameMode : gameModes) {
            gameModeSwitch->addItem(gameMode->getName());
        }
        gameModeSwitch->setPosition(10, 0, 330, 20);
        gameModeSwitch->onToggled([this](Int32 selectedIndex) {
            if (selectedIndex < 2) {
                playerListBox->onColorize(Gui::ListBox::defaultColorize);
            } else {
                Int32 teamCount = 1 + selectedIndex / 2;
                playerListBox->onColorize([teamCount](Int32 index, const std::string& label) {
                    return Gui::ListBox::ItemColor{Color::BLACK, TEAMS[index % teamCount].color};
                });
            }
        });


        backgroundCount = File::countFiles(D6_TEXTURE_BCG_PATH, D6_TEXTURE_EXTENSION);
        levelList.initialize(D6_FILE_LEVEL, D6_LEVEL_EXTENSION);

        levelListBox->addItem("Random");
        for (Size i = 0; i < levelList.getLength(); i++) {
            levelListBox->addItem(levelList.getFileName(i).substr(0, levelList.getFileName(i).rfind(".")));
        }
        levelListBox->selectItem(0);

        menuTrack = sound.loadModule("sound/undead.xm");
    }

    void Menu::initializeGameModes() {
        gameModes.push_back(std::make_unique<DeathMatch>());
        gameModes.push_back(std::make_unique<Predator>());
        gameModes.push_back(std::make_unique<TeamDeathMatch>(2, false));
        gameModes.push_back(std::make_unique<TeamDeathMatch>(2, true));
        gameModes.push_back(std::make_unique<TeamDeathMatch>(3, false));
        gameModes.push_back(std::make_unique<TeamDeathMatch>(3, true));
        gameModes.push_back(std::make_unique<TeamDeathMatch>(4, false));
        gameModes.push_back(std::make_unique<TeamDeathMatch>(4, true));
    }

    void Menu::savePersonData() const {
        Json::Value json = Json::Value::makeObject();
        json.set("persons", persons.toJson());

        Json::Value playing = Json::Value::makeArray();
        for (Size i = 0; i < playerListBox->size(); i++) {
            playing.add(Json::Value::makeString(playerListBox->getItem(i)));
        }
        json.set("playing", playing);

        Json::Writer writer(true);
        writer.writeToFile(D6_FILE_PHIST, json);
    }

    void Menu::rebuildTable() {
        scoreListBox->clear();
        if (persons.isEmpty())
            return;

        std::vector<const Person *> ranking;

        for (const Person &person : persons.list()) {
            ranking.push_back(&person);
        }

        std::sort(ranking.begin(), ranking.end(), [](const Person *left, const Person *right) {
            return left->hasHigherScoreThan(*right);
        });

        for (auto person : ranking) {
            std::string personStat =
                    Format("{0,-11}|{1,6} |{2,5} |{3,6} |{4,4}% |{5,6} | {6,7} | {7,3} |{8,4} |{9,5}% |{10,4}|{12,4}|{11,4}m ")
                            << person->getName()
                            << person->getGames()
                            << person->getWins()
                            << person->getShots()
                            << person->getAccuracy()
                            << person->getKills()
                            << person->getAssistances()
                            << person->getPenalties()
                            << person->getTotalPoints()
                            << person->getAliveRatio()
                            << person->getTotalDamage()
                            << (person->getTotalGameTime() + 30) / 60
                            << person->getAssistedDamage();
            scoreListBox->addItem(personStat);
        }
    }

    void Menu::showMessage(const std::string &message) {
        Int32 width = Int32(message.size()) * 8 + 60;
        Int32 x = video.getScreen().getClientWidth() / 2 - width / 2,
                y = video.getScreen().getClientHeight() / 2 - 10;

        globRenderer->quadXY(Vector(x, y), Vector(width, 20), Color(255, 204, 204));
        globRenderer->frame(Vector(x, y), Vector(width, 20), 2, Color::BLACK);

        font.print(x + 30, y + 2, Color::RED, message);
        video.screenUpdate(appService.getConsole(), font);
    }

    bool Menu::question(const std::string &question) {
        showMessage(question);
        SDL_Event event;
        bool answer;

        while (true) {
            if (SDL_PollEvent(&event)) {
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_y) {
                        answer = true;
                        break;
                    } else if (event.key.keysym.sym == SDLK_n) {
                        answer = false;
                        break;
                    }
                }
            }
        }

        consumeInputEvents();
        return answer;
    }

    bool Menu::deleteQuestion() {
        return question("Really delete? (Y/N)");
    }

    void Menu::cleanPersonData() {
        for (Person &person : persons.list()) {
            person.reset();
        }
        rebuildTable();
        savePersonData();
    }

    void Menu::detectControls(Size playerIndex) {
        render();

        const std::string &name = playerListBox->getItem(playerIndex);
        showMessage("Player " + name + ": Press any control");
        playPlayersSound(name);

        SDL_Event event;
        SDL_Keysym key;
        bool detected = false;

        while (!detected) {
            if (processEvents(true)) {
                for (Size i = 0; i < controlsManager.getNumAvailable(); i++) {
                    const PlayerControls &pc = controlsManager.get(i);

                    if ( (!pc.getLeft().isJoyPadAxis() && pc.getLeft().isPressed()) ||
                        (!pc.getRight().isJoyPadAxis() && pc.getRight().isPressed()) ||
                        (!pc.getDown().isJoyPadAxis() && pc.getDown().isPressed()) ||
                        (!pc.getUp().isJoyPadAxis() && pc.getUp().isPressed()) ||
                        pc.getShoot().isPressed() ||
                        pc.getPick().isPressed()) {
                        controlSwitch[playerIndex]->setCurrent((Int32) i);
                        detected = true;
                    }
                }
            }
        }

        consumeInputEvents();
    }

    void Menu::playPlayersSound(const std::string &name) {
        Person &person = persons.getByName(name);
        auto profile = getPersonProfile(person.getName());
        if (profile) {
            profile->getSounds().getRandomSample(PlayerSounds::Type::GotHit).play();
        }
    }

    void Menu::play() {
        if (playerListBox->size() < 2) {
            showMessage("Can't play alone ...");
            SDL_Event event;
            while (true) {
                if (SDL_PollEvent(&event)) {
                    break;
                }
            }

            consumeInputEvents();
            return;
        }

        if (game->getSettings().isRoundLimit() && game->getPlayedRounds() > 0 &&
            game->getPlayedRounds() < game->getSettings().getMaxRounds()) {
            if (!question("Resume previous game? (Y/N)")) {
                cleanPersonData();
                game->setPlayedRounds(0);
            }
        } else {
            if (question("Clear statistics? (Y/N)")) {
                cleanPersonData();
                game->setPlayedRounds(0);
            }
        }

        GameMode &selectedMode = *gameModes[gameModeSwitch->currentItem()];

        std::vector<Game::PlayerDefinition> playerDefinitions;
        for (Size i = 0; i < playerListBox->size(); i++) {
            Person &person = persons.getByName(playerListBox->getItem(i));
            auto profile = getPersonProfile(person.getName());
            const PlayerControls &controls = controlsManager.get(controlSwitch[i]->currentValue().first);
            PlayerSkinColors colors = profile ? profile->getSkinColors() : PlayerSkinColors::makeRandom();
            const PlayerSounds &sounds = profile ? profile->getSounds() : defaultPlayerSounds;
            playerDefinitions.push_back(Game::PlayerDefinition(person, colors, sounds, controls));
        }
        selectedMode.initializePlayers(playerDefinitions);

        // Levels
        std::vector<std::string> levels;
        if (!levelListBox->selectedIndex()) {
            for (Size i = 0; i < levelList.getLength(); ++i) {
                levels.push_back(levelList.getPath(i));
            }
        } else {
            levels.push_back(levelList.getPath(levelListBox->selectedIndex() - 1));
        }

        // Game backgrounds
        std::vector<Size> backgrounds;
        for (Size i = 0; i < backgroundCount; i++) {
            backgrounds.push_back(i);
        }

        // Screen
        ScreenMode screenMode = (playerListBox->size() > 4 || screenModeListBox->selectedIndex() == 0)
                                ? ScreenMode::FullScreen : ScreenMode::SplitScreen;
        Int32 screenZoom = 13;

        // Start
        Context::push(*game);
        game->start(playerDefinitions, levels, backgrounds, screenMode, screenZoom, selectedMode);
    }

    void Menu::addPlayer(Int32 index) {
        if (index != -1 && playerListBox->size() < D6_MAX_PLAYERS) {
            const std::string &name = personListBox->getItem(index);
            playerListBox->addItem(name);
            personListBox->removeItem(index);
        }
        updatePlayerCount();
    }

    void Menu::removePlayer(Int32 index) {
        if (index != -1) {
            const std::string &playerName = playerListBox->getItem(index);
            personListBox->addItem(playerName);
            playerListBox->removeItem(index);
        }
        updatePlayerCount();
    }

    void Menu::updatePlayerCount() {
        playersLabel->setCaption(Format("Players {0,5}") << playerListBox->size());
    }

    void Menu::addPerson() {
        const std::string &personName = textbox->getText();

        if (!personName.empty() && !persons.contains(personName)) {
            persons.add(Person(personName, nullptr));
            personListBox->addItem(personName);
            rebuildTable();
            textbox->flush();
        }
    }

    void Menu::deletePerson() {
        Int32 index = personListBox->selectedIndex();
        if (index != -1) {
            if (!deleteQuestion())
                return;

            const std::string &playerName = personListBox->selectedItem();
            persons.remove(playerName);
            personListBox->removeItem(playerName);
        }
    }

    void Menu::beforeStart(Context *prevContext) {
        joyRescan();
        SDL_ShowCursor(SDL_ENABLE);
        SDL_StartTextInput();
        rebuildTable();
        if (playMusic) {
            menuTrack.play(false);
        }
    }

    void Menu::update(Float32 elapsedTime) {
        gui.update(elapsedTime);
    }

    void Menu::render() const {
        Int32 trX = (video.getScreen().getClientWidth() - 800) / 2;
        Int32 trY = (video.getScreen().getClientHeight() - 700) / 2;

        gui.draw(font);

        globRenderer->setViewMatrix(Matrix::translate(Float32(trX), -Float32(trY), 0));

        font.print(687, video.getScreen().getClientHeight() - 20, Color::WHITE,
                   Format("{0} {1}") << "version" << APP_VERSION);

        Int32 clientHeight = video.getScreen().getClientHeight();
        Material material = Material::makeTexture(menuBannerTexture.at(0));
        globRenderer->quadXY(Vector(300, clientHeight - 100), Vector(200, 95), Vector(0, 1), Vector(1, -1), material);

        globRenderer->setViewMatrix(Matrix::IDENTITY);
    }

    void Menu::keyEvent(const KeyPressEvent &event) {
        gui.keyEvent(event);

        if (event.getCode() == SDLK_RETURN) {
            addPerson();
        }

        if (event.getCode() == SDLK_F1) {
            play();
        }

        if (event.getCode() == SDLK_F3) {
            if (deleteQuestion()) {
                cleanPersonData();
            }
        }

        if (event.getCode() == SDLK_ESCAPE) {
            close();
        }
    }

    void Menu::textInputEvent(const TextInputEvent &event) {
        gui.textInputEvent(event);
    }

    void Menu::mouseButtonEvent(const MouseButtonEvent &event) {
        gui.mouseButtonEvent(event);
    }

    void Menu::mouseMotionEvent(const MouseMotionEvent &event) {
        gui.mouseMotionEvent(event);
    }

    void Menu::mouseWheelEvent(const MouseWheelEvent &event) {
        gui.mouseWheelEvent(event);
    }

    void Menu::joyDeviceAddedEvent(const JoyDeviceAddedEvent & event) {
        joyRescan();
    }

    void Menu::joyDeviceRemovedEvent(const JoyDeviceRemovedEvent & event) {
        joyRescan();
    }

    void Menu::beforeClose(Context *newContext) {
        SDL_StopTextInput();
        sound.stopMusic();
        savePersonData();
    }

    void Menu::enableMusic(bool enable) {
        playMusic = enable;

        if (isCurrent()) {
            if (enable) {
                menuTrack.play(false);
            } else {
                sound.stopMusic();
            }
        }
    }

    void Menu::loadPersonProfiles(const std::string &path) {
        appService.getConsole().printLine("\n===Person profile initialization===");

        std::vector<std::string> profileDirs = File::listDirectory(path, "");
        for (auto &profileName : profileDirs) {
            std::string profilePath = Format("{0}/{1}/") << path << profileName;
            auto profile = std::make_unique<PersonProfile>(profileName, profilePath);
            profile->loadSounds(sound);
            profile->loadSkinColors();
            profile->loadScripts(appService.getScriptManager());
            personProfiles.insert(std::make_pair(profileName, std::move(profile)));
        }

        appService.getConsole().printLine("");
    }

    PersonProfile *Menu::getPersonProfile(const std::string &name) {
        auto profile = personProfiles.find(name);
        if (profile != personProfiles.end()) {
            return profile->second.get();
        }

        return nullptr;
    }
    int Menu::processEvents(bool single) {
        SDL_Event event;
        int result = 0;
        //TODO This logic duplicates event processing logic in Application. Should be refactored.
        while ((result = SDL_PollEvent(&event)) != 0) {
            switch (event.type) {
                case SDL_KEYDOWN:{
                    auto key = event.key.keysym;
                    appService.getInput().setPressed(key.sym, true);
                    break;
                }
                case SDL_KEYUP:{
                    auto key = event.key.keysym;
                    appService.getInput().setPressed(key.sym, false);
                    break;
                }
                case SDL_JOYDEVICEADDED:{
                    appService.getConsole().printLine("Device added");

                    auto deviceIndex = event.jdevice.which;
                    auto joy = SDL_JoystickOpen(deviceIndex);
                    if(SDL_JoystickGetAttached(joy)){
                       appService.getInput().joyAttached(joy);
                       joyRescan();
                    } else {
                        appService.getConsole().printLine(Format("Joy attached, but has been detached again -> skipping."));
                        break;
                    }

                    break;
                }
                case SDL_JOYDEVICEREMOVED: {
                    appService.getConsole().printLine("Device removed");
                    auto instanceId = event.jdevice.which;
                    appService.getInput().joyDetached(instanceId);
                    joyRescan();
                    break;
                }
            }
            if(single){
                return result;
            }
        }
        return result;
    }
    void Menu::consumeInputEvents() {
        processEvents();
    }

    void Menu::shufflePlayers() {
        std::vector<std::string> players;
        for (Size i = 0; i < playerListBox->size(); i++) {
            players.push_back(playerListBox->getItem(i));
        }

        std::shuffle(players.begin(), players.end(), Math::randomEngine);
        playerListBox->clear();
        for (std::string &player : players) {
            playerListBox->addItem(player);
        }
    }
}
