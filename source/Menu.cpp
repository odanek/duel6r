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
#include "input/PlayerControls.h"
#include "TextureManager.h"
#include "Menu.h"
#include "Game.h"
#include "File.h"
#include "Font.h"
#include "json/JsonParser.h"
#include "json/JsonWriter.h"
#include "GameMode.h"
#include "gamemodes/DeathMatch.h"
#include "gamemodes/TeamDeathMatch.h"
#include "gamemodes/Predator.h"
#include "gui/Dialog.h"
#include "gui/Image.h"
#define D6_ALL_CHR  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 -=\\~!@#$%^&*()_+|[];',./<>?:{}"
#define D6_DIGITS_CHR "1234567890"
namespace Duel6 {
    Menu::Menu(AppService &appService)
            : appService(appService), font(appService.getFont()), video(appService.getVideo()),
              renderer(video.getRenderer()), sound(appService.getSound()), gui(video.getRenderer(), font),
              mainView(new Gui::View(gui, 0,0, 960, 900)),
              controlsManager(appService.getControlsManager()),
              defaultPlayerSounds(PlayerSounds::makeDefault(sound)), playMusic(false) {}

    void Menu::loadNetworkSettings(const std::string &filePath) {
        if (!File::exists(filePath)) {
            host->setText("localhost");
            port->setText("5900");
            return;
        }
        Json::Value defaultMasterURL = Json::Value::makeString("oraculum-eu1.duel6.mrakonos.cz");
        Json::Value defaultMasterPort = Json::Value::makeNumber(25901);
        Json::Value defaultMaster = Json::Value::makeObject();
        defaultMaster.set("URL", defaultMasterURL);
        defaultMaster.set("port", defaultMasterPort);

        Json::Parser parser;
        Json::Value json = parser.parse(filePath);
        Json::Value host = json.getOrDefault("host", Json::Value::makeString("localhost"));
        Json::Value port = json.getOrDefault("port", Json::Value::makeString("25800"));
        Json::Value master = json.getOrDefault("master", defaultMaster);
        Json::Value enableMaster = json.getOrDefault("enableMaster", Json::Value::makeBoolean(false));
        Json::Value enableMasterDiscovery = json.getOrDefault("enableMasterDiscovery", Json::Value::makeBoolean(false));
        Json::Value enableNAT = json.getOrDefault("enableNAT", Json::Value::makeBoolean(false));
        Json::Value localIPAddress = json.getOrDefault("localIPAddress", Json::Value::makeString("0.0.0.0"));
        Json::Value serverDescription = json.getOrDefault("serverDescription", Json::Value::makeString("unnamed server"));
        Json::Value publicIPAddress = json.getOrDefault("publicIPAddress", Json::Value::makeString("0.0.0.0"));
        Json::Value publiPort = json.getOrDefault("publicPort", Json::Value::makeNumber(25800));

        this->host->setText(host.asString());
        this->port->setText(port.asString());
        this->netConfig.masterServer = master.getOrDefault("URL", Json::Value::makeString("oraculum-eu1.duel6.mrakonos.cz")).asString();
        this->netConfig.masterServerPort = master.getOrDefault("port", Json::Value::makeNumber(25901)).asInt();
        this->netConfig.enableMasterServer = enableMaster.asBoolean();
        this->netConfig.localIPAddress = localIPAddress.asString();
        this->netConfig.enableMasterDiscovery = enableMasterDiscovery.asBoolean();
        this->netConfig.enableNATPunch = enableNAT.asBoolean();
        this->netConfig.serverDescription = serverDescription.asString();
        this->netConfig.publicIPAddress = publicIPAddress.asString();
        this->netConfig.publicPort = publiPort.asInt();
    }

    void Menu::saveNetworkSettings() {
        Json::Value json = Json::Value::makeObject();
        Json::Value masterURL = Json::Value::makeString(this->netConfig.masterServer);
        Json::Value masterPort = Json::Value::makeNumber(this->netConfig.masterServerPort);
        Json::Value master = Json::Value::makeObject();
        master.set("URL", masterURL);
        master.set("port", masterPort);

        json.set("host", Json::Value::makeString(this->host->getText()));
        json.set("port", Json::Value::makeString(this->port->getText()));
        json.set("master", master);
        json.set("enableMaster", Json::Value::makeBoolean(this->netConfig.enableMasterServer));
        json.set("enableMasterDiscovery", Json::Value::makeBoolean(this->netConfig.enableMasterDiscovery));
        json.set("enableNAT", Json::Value::makeBoolean(this->netConfig.enableNATPunch));
        json.set("localIPAddress", Json::Value::makeString(this->netConfig.localIPAddress));
        json.set("serverDescription", Json::Value::makeString(this->netConfig.serverDescription));
        json.set("publicIPAddress", Json::Value::makeString(this->netConfig.publicIPAddress));
        json.set("publicPort", Json::Value::makeNumber(this->netConfig.publicPort));

        Json::Writer writer(true);
        writer.writeToFile(D6_FILE_NETWORK_SETTINGS, json);
    }

    void Menu::loadPersonData(const std::string &filePath) {
        if (!File::exists(filePath)) {
            return;
        }

        personListBox->clear();
        playerListBox->clear();

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

        updatePlayerCount();

        Int32 playedRounds = json.getOrDefault("rounds", Json::Value::makeNumber(0)).asInt();
        game->setPlayedRounds(playedRounds);
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
        menuBannerTexture = appService.getTextureManager().loadStack(D6_TEXTURE_MENU_PATH, TextureFilter::Linear, true);

        appService.getConsole().printLine("...Starting GUI library");
        gui.screenSize(video.getScreen().getClientWidth(), video.getScreen().getClientHeight(),
                                    (video.getScreen().getClientWidth() - 960) / 2, (video.getScreen().getClientHeight() - 900) / 2);

        video.onResize([this](Int32 width, Int32 height){
            gui.screenSize(width, height, (width - 960) / 2, (height - 900) / 2);
            renderer.setViewport(0,0, width, height);
            video.setMode(Video::Mode::Orthogonal);
        });

        Gui::View & mainView = *(this->mainView);
        Material material = Material::makeTexture(menuBannerTexture);
        new Gui::Image(mainView, material, 350, 770, 200, 95);

        scoreListBox = new Gui::ListBox(mainView, true);
        scoreListBox->setPosition(10, 289, 110, 12, 16);

        personListBox = new Gui::ListBox(mainView, true);
        personListBox->setPosition(10, 641, 20, 15, 19);
        personListBox->onDoubleClick([this](Int32 index, const std::string &item) {
            addPlayer(index);
        });

        playerListBox = new Gui::ListBox(mainView, false);
        playerListBox->setPosition(200, 641, 20, D6_MAX_PLAYERS, 19);
        playerListBox->onDoubleClick([this](Int32 index, const std::string &item) {
            removePlayer(index);
        });

        eloListBox = new Gui::ListBox(mainView, true);
        eloListBox->setPosition(695, 639, 24, 19, 16);

        loadPersonProfiles(D6_FILE_PROFILES);

        auto addPlayerButton = new Gui::Button(mainView);
        addPlayerButton->setPosition(200, 343, 80, 25);
        addPlayerButton->setCaption(">>");
        addPlayerButton->onClick([this](Gui::Button &) {
            addPlayer(personListBox->selectedIndex());
        });

        auto removePlayerButton = new Gui::Button(mainView);
        removePlayerButton->setPosition(105, 343, 85, 25);
        removePlayerButton->setCaption("<<");
        removePlayerButton->onClick([this](Gui::Button &) {
            removePlayer(playerListBox->selectedIndex());
        });

        auto removePersonButton = new Gui::Button(mainView);
        removePersonButton->setPosition(10, 343, 90, 25);
        removePersonButton->setCaption("Remove");
        removePersonButton->onClick([this](Gui::Button &) {
            deletePerson();
            rebuildTable();
        });

        auto addPersonButton = new Gui::Button(mainView);
        addPersonButton->setPosition(284, 343, 80, 25);
        addPersonButton->setCaption("Add");
        addPersonButton->onClick([this](Gui::Button &) {
            addPerson();
        });

        auto playButton = new Gui::Button(mainView);
        playButton->setPosition(445, 90, 150, 50);
        playButton->setCaption("Play (F1)");
        playButton->onClick([this](Gui::Button &) {
            play(false);
        });

        auto clearButton = new Gui::Button(mainView);
        clearButton->setPosition(600, 90, 150, 50);
        clearButton->setCaption("Clear (F3)");
        clearButton->onClick([this](Gui::Button &) {
            if (deleteQuestion()) {
                cleanPersonData();
            }
        });

        host = new Gui::Textbox(mainView);
        host->setPosition(10, 730, 30, 20, D6_ALL_CHR);
        host->setText("localhost");
        host->setPlaceholder("(address e.g. google.com or 1.2.3.4)");
        port = new Gui::Textbox(mainView);
        port->setPosition(280, 730, 20, 5, D6_DIGITS_CHR);
        port->setText("5900");
        port->setPlaceholder("(port e.g. 5900)");


        auto serverList = new Gui::Button(mainView);

        serverList->setPosition(10, 800, 110, 30);
        serverList->setCaption("List");
        serverList->onClick([this](Gui::Button &) {
            serverlist();
            Int32 w = 800;
            Int32 h = 400;
            Gui::Dialog * dialog = new Gui::Dialog(this->gui, 10, 400, w, h, std::string("Servers"));
            Gui::ListBox * servers = new Gui::ListBox(*dialog, true);
            Gui::Button * bt = new Gui::Button(*dialog);
            bt->setPosition(5, 40, 110, 30);
            bt->setCaption("Refresh");
            Gui::Button * connectBt = new Gui::Button(*dialog);
            connectBt->setPosition(160, 40, 110, 30);
            connectBt->setCaption("Connect");
            connectBt->setEnabled(false);
            servers->setPosition(5, h - 40, w / 8 - 4, (h - 60) / 16 - 2, 16);
            dialog->onResize([servers](Gui::View& dialog, Int32 x , Int32 y, Int32 w, Int32 h){
                servers->setPosition(5, h - 40, w / 8 - 4, (h - 60) / 16 - 2, 16);
            });
            bt->onClick([this](Gui::Button &){
                serverlist();
            });
            servers->onItemSelected([this, connectBt](Int32 index, const std::string &item){
                pickedServerIndex = index;
                connectBt->setEnabled(true);
            });
            connectBt->onClick([this, dialog](Gui::Button &){
                this->joinServerFromServerList(pickedServerIndex);
                dialog->close();
            });
            servers->onDoubleClick([this, dialog](Int32 index, const std::string &item){
                this->joinServerFromServerList(index);
                dialog->close();
            });
           Menu::ServerList::callbackReset_t resetCallback = this->serverList.setCallback([servers](Menu::ServerList::list_t & list){
               servers->clear();
               for(const auto & server : list)
               servers->addItem(server.text);
           });
           dialog->onClose([resetCallback](Gui::View & dialog){
               resetCallback();
           });
        });

        auto startServerB = new Gui::Button(mainView);

        startServerB->setPosition(10, 700, 110, 30);
        startServerB->setCaption("Server");
        startServerB->onClick([this](Gui::Button &) {
            startServer();
        });
        auto connect = new Gui::Button(mainView);

        connect->setPosition(140, 700, 110, 30);
        connect->setCaption("Connect");
        connect->onClick([this](Gui::Button &) {
            joinServer();
        });
        reverseConnection = new Gui::CheckBox(mainView);
        reverseConnection->setPosition(260, 700, 60, 20);
        reverseConnection->setLabel("Reverse connection");

        auto quitButton = new Gui::Button(mainView);
        quitButton->setPosition(755, 90, 150, 50);
        quitButton->setCaption("Quit (ESC)");
        quitButton->onClick([this](Gui::Button &) {
            quit();
        });
        auto versionLabel = new Gui::Label(mainView);
        versionLabel->setCaption(Format("{0} {1}") << "version" << APP_VERSION);
        versionLabel->setPosition(10, 40, 0, 18);
        auto scoreLabel = new Gui::Label(mainView);
        scoreLabel->setPosition(10, 309, 800, 18);
        scoreLabel->setCaption(
                "    Name   |   Elo | Pts | Win | Kill | Assist | Pen | Death |  K/D | Shot | Acc. | GmTm |  Dmg ");

        auto eloLabel = new Gui::Label(mainView);
        eloLabel->setPosition(695, 660, 210, 18);
        eloLabel->setCaption("Elo scoreboard");

        auto personsLabel = new Gui::Label(mainView);
        personsLabel->setPosition(10, 660, 181, 18);
        personsLabel->setCaption("Persons");

        playersLabel = new Gui::Label(mainView);
        playersLabel->setPosition(200, 660, 118, 18);

        updatePlayerCount();

        Gui::Button *shuffleButton = new Gui::Button(mainView);
        shuffleButton->setCaption("S");
        shuffleButton->setPosition(347, 660, 17, 17);
        shuffleButton->onClick([this](Gui::Button &) {
            shufflePlayers();
        });

        Gui::Button *eloShuffleButton = new Gui::Button(mainView);
        eloShuffleButton->setCaption("E");
        eloShuffleButton->setPosition(330, 660, 17, 17);
        eloShuffleButton->onClick([this](Gui::Button &) {
            eloShufflePlayers();
        });

        auto controllerLabel = new Gui::Label(mainView);
        controllerLabel->setPosition(370, 660, 192, 18);
        controllerLabel->setCaption("Controller");

        textbox = new Gui::Textbox(mainView);
        textbox->setPosition(370, 342, 14, 10, D6_ALL_CHR);
        textbox->onEnter([this](Gui::Textbox &) {
            addPerson();
        });
        // Player controls
        for (Size i = 0; i < D6_MAX_PLAYERS; i++) {
            controlSwitch[i] = new Gui::Spinner(mainView);
            controlSwitch[i]->setPosition(370, 639 - Int32(i) * 19, 192, 0);
        }

        for (Size i = 0; i < D6_MAX_PLAYERS; i++) {
            Gui::Button *button = new Gui::Button(mainView);
            button->setCaption("D");
            button->setPosition(566, 637 - Int32(i) * 19, 18, 18);
            button->onClick([this, i](Gui::Button &) {
                detectControls(i);
            });
        }

        // Button to detect all user's controllers in a batch
        Gui::Button *button = new Gui::Button(mainView);
        button->setCaption("D");
        button->setPosition(566, 658, 18, 18);
        button->onClick([this](Gui::Button &) {
            joyRescan();
            Size curPlayersCount = playerListBox->size();
            for (Size j = 0; j < curPlayersCount; j++) {
                if(!detectControls(j)){
                    break;
                }
            }
        });

        auto teamSelectorLabel = new Gui::Label(mainView);
        teamSelectorLabel->setPosition(590, 660, 60, 18);
        teamSelectorLabel->setCaption("Team");

        // Player teams
        for (Size i = 0; i < D6_MAX_PLAYERS; i++) {
            teamControlSwitch[i] = new Gui::Spinner(mainView);
            teamControlSwitch[i]->setPosition(590, 639 - Int32(i) * 19, 60, 0);
            teamControlSwitch[i]->addItem("", 0, false);
            teamControlSwitch[i]->addItem("", 1, false);
            teamControlSwitch[i]->addItem("", 2, false);
            teamControlSwitch[i]->addItem("", 3, false);
            teamControlSwitch[i]->addItem("", 4, false);
            teamControlSwitch[i]->onColorize([i](Int32 index, const std::string &item) {
                if(index == 0) {
                    return Gui::Spinner::defaultColorize(index, item);
                }
                return Gui::Spinner::ItemColor{Color::BLACK, TEAMS[(index - 1) % 4].color};
            });
        }

        initializeGameModes();
        gameModeSwitch = new Gui::Spinner(mainView);
        for (auto &gameMode : gameModes) {
            gameModeSwitch->addItem(gameMode->getName());
        }
        gameModeSwitch->setPosition(10, 90, 330, 20);
        gameModeSwitch->onToggled([this](Int32 selectedIndex) {
            Int32 teamCount = 1 + selectedIndex / 2;
            for(size_t i = 0; i < playerListBox->size(); i++){
                if(selectedIndex < 2){
                    teamControlSwitch[i]->setCurrent(0);
                } else {
                    teamControlSwitch[i]->setCurrent(i % teamCount + 1);
                }
            }
        });

        globalAssistanceCheckBox = new Gui::CheckBox(mainView, true);
        globalAssistanceCheckBox->setLabel("Assistance");
        globalAssistanceCheckBox->setPosition(11, 69, 130, 20);

        quickLiquidCheckBox = new Gui::CheckBox(mainView, false);
        quickLiquidCheckBox->setLabel("Quick Liquid");
        quickLiquidCheckBox->setPosition(151, 69, 150, 20);

        backgroundCount = File::countFiles(D6_TEXTURE_BCG_PATH);
        levelList.initialize(D6_FILE_LEVEL, D6_LEVEL_EXTENSION);

        menuTrack = sound.loadModule("sound/undead.xm");

        appService.getConsole().printLine("\n===Loading network.json===");
        loadNetworkSettings(D6_FILE_NETWORK_SETTINGS);
        appService.getNetClient().setLocalIPAddress(netConfig.localIPAddress);
        appService.getNetClient().setMasterAddressAndPort(netConfig.masterServer, netConfig.masterServerPort);
        appService.getNetHost().setMasterAddressAndPort(netConfig.masterServer, netConfig.masterServerPort);
        appService.getNetHost().setServerConfig(netConfig.getPublicIPAddress(), netConfig.publicPort, netConfig.serverDescription, netConfig.enableMasterDiscovery, netConfig.enableNATPunch);
        appService.getNetClient().initNetHost();
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

        json.set("rounds", Json::Value::makeNumber(game->getPlayedRounds()));

        Json::Writer writer(true);
        writer.writeToFile(D6_FILE_PHIST, json);
    }

    void Menu::rebuildTable() {
        scoreListBox->clear();
        if (persons.isEmpty())
            return;

        std::vector<const Person *> ranking;

        for (const Person &person : persons.list()) {
            if (person.getGames() > 0) {
                ranking.push_back(&person);
            }
        }

        std::sort(ranking.begin(), ranking.end(), [](const Person *left, const Person *right) {
            return left->hasHigherScoreThan(*right);
        });

        for (auto person : ranking) {
            std::string personStat =
                    Format("{0,-11}|{1,5}{2,1} |{3,4} |{4,4} |{5,5} |{6,7} |{7,4} |{8,6} |{9,5} |{10,5} |{11,4}% |{12,4}% |{13,5} ")
                            << person->getName()
                            << person->getElo()
                            << (person->getEloTrend() > 0 ? '+' : (person->getEloTrend() < 0 ? '-' : '='))
                            << person->getTotalPoints()
                            << person->getWins()
                            << person->getKills()
                            << person->getAssistances()
                            << person->getPenalties()
                            << person->getDeaths()
                            << Person::getKillsToDeathsRatio(person->getKills(), person->getDeaths())
                            << person->getShots()
                            << person->getAccuracy()
                            << person->getAliveRatio()
                            << person->getTotalDamage();
            scoreListBox->addItem(personStat);
        }

        std::vector<const Person *> eloRanking;
        for (const Person &person : persons.list()) {
            if (person.getEloGames() > 0) {
                eloRanking.push_back(&person);
            }
        }
        std::sort(eloRanking.begin(), eloRanking.end(), [](const Person *left, const Person *right) {
            return left->getElo() > right->getElo();
        });

        eloListBox->clear();
        Int32 index = 1;
        for (auto person : eloRanking) {
            auto trend = person->getEloTrend();
            auto sign = trend > 0 ? "+" : "-";
            std::string trendStr = trend == 0 ? std::string() : Format("{0}{1}") << sign << std::abs(trend);
            eloListBox->addItem(Format("{0,2|0} {1,-11} {2,4} {3,4}") << index << person->getName() << person->getElo() << trendStr);
            index++;
        }
    }

    void Menu::showMessage(const std::string &message) {
        const Float32 fontHeight = 32;
        const std::string cancel = "[ESC] to cancel";
        Int32 width = font.getTextWidth(message, fontHeight) + 60;
        Int32 cancelWidth = font.getTextWidth(cancel, fontHeight);
        Int32 height = fontHeight * 2; //2 lines
        Int32 x = video.getScreen().getClientWidth() / 2 - width / 2,
            y = video.getScreen().getClientHeight() / 2 - height / 2;

        renderer.quadXY(Vector(x, y), Vector(width, height + 4), Color(255, 204, 204));
        renderer.frame(Vector(x, y), Vector(width, height + 4), 2, Color::BLACK);

        font.print(x + 30, y + 32, 0.5f, Color::RED, message, fontHeight);
        font.print(x + (width - cancelWidth) / 2.0f, y + 2, 0.5f, Color::RED, cancel, fontHeight);
        video.screenUpdate(appService.getConsole(), font);
    }
    bool Menu::question(const std::string &q) {
        bool cancel;
        return question(q, cancel);
    }
    bool Menu::question(const std::string &question, bool & cancel) {
        showMessage(question);
        appService.getConsole().printLine(question);
#ifdef D6_RENDERER_HEADLESS
        return true;
#endif
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
                    } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        answer = false;
                        cancel = true;
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

    bool Menu::detectControls(Size playerIndex) {
        render();
        if (playerIndex >= playerListBox->size()) {
            return false;
        }
        const std::string &name = playerListBox->getItem(playerIndex);
        showMessage("Player " + name + ": Press any control");
        playPlayersSound(name);

        bool detected = false;
        bool cancelled = false;
        while (!detected && !cancelled) {
            if (processEvents(true, cancelled)) {
                for (Size i = 0; i < controlsManager.getNumAvailable(); i++) {
                    const PlayerControls &pc = controlsManager.get(i);

                    if ((!pc.getLeft().isJoyPadAxis() && pc.getLeft().isPressed()) ||
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
        return !cancelled;
    }

    void Menu::playPlayersSound(const std::string &name) {
        Person &person = persons.getByName(name);
        auto profile = getPersonProfile(person.getName());
        if (profile) {
            profile->getSounds().getRandomSample(PlayerSounds::Type::GotHit).play();
        }
    }

    bool Menu::play(bool networkGame) {
       return play(listMaps(), networkGame);
    }

    bool Menu::play(std::vector<std::string> levels, bool networkGame) {
        game->getSettings().setQuickLiquid(quickLiquidCheckBox->isChecked());
        game->getSettings().setGlobalAssistances(globalAssistanceCheckBox->isChecked());
        if (game->getSettings().isRoundLimit()) {
            if (game->getPlayedRounds() == 0 || game->getPlayedRounds() >= game->getSettings().getMaxRounds() || !question("Resume previous game? (Y/N)")) {
                cleanPersonData();
                game->setPlayedRounds(0);
            }
        } else {
            bool cancelled = false;
            if (question("Clear statistics? (Y/N)", cancelled)) {
                cleanPersonData();
                game->setPlayedRounds(0);
            }
            if(cancelled){
                return false;
            }
        }

        GameMode &selectedMode = *gameModes[gameModeSwitch->currentItem()];

        std::vector<PlayerDefinition> playerDefinitions;
        for (Size i = 0; i < playerListBox->size(); i++) {
            Person &person = persons.getByName(playerListBox->getItem(i));
            auto profile = getPersonProfile(person.getName());
            const PlayerControls &controls = controlsManager.get(controlSwitch[i]->currentValue().first);
            PlayerSkinColors colors = profile ? profile->getSkinColors() : PlayerSkinColors::makeRandom();
            const PlayerSounds &sounds = profile ? profile->getSounds() : defaultPlayerSounds;
            playerDefinitions.push_back(PlayerDefinition(person, colors, sounds, controls, teamControlSwitch[i]->currentValue().first, i, 0, i));
        }
        //selectedMode.initializePlayers(playerDefinitions);


        // Game backgrounds
        std::vector<Size> backgrounds;
        for (Size i = 0; i < backgroundCount; i++) {
            backgrounds.push_back(i);
        }

        // Screen
        ScreenMode screenMode = ScreenMode::FullScreen;
        Int32 screenZoom = 13;

        // Clear elo trend
        for (auto &person : persons.list()) {
            person.setEloTrend(0);
        }

        // Start
        Context::push(*game);
        game->start(playerDefinitions, levels, backgrounds, screenMode, screenZoom, selectedMode, networkGame);
        return true;
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

            for (Int32 i = index; i + 1 < D6_MAX_PLAYERS; i++) {
                controlSwitch[i]->setCurrent(controlSwitch[i + 1]->currentItem());
            }
        }
        updatePlayerCount();
    }

    void Menu::updatePlayerCount() {
        playersLabel->setCaption(Format("Players {0,3}") << playerListBox->size());
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
        loadPersonData(D6_FILE_PHIST);
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
        gui.draw(font);
    }

    void Menu::keyEvent(const KeyPressEvent &event) {
        if(gui.keyEvent(event)){
            return;
        }

        if (event.getCode() == SDLK_F1) {
            play(false);
        }

        if (event.getCode() == SDLK_F3) {
            if (deleteQuestion()) {
                cleanPersonData();
            }
        }

        if (event.getCode() == SDLK_ESCAPE) {
            quit();
        }
    }

    void Menu::quit() {
        if (Menu::question("Quit now ? Y/N")) {
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

    void Menu::joyDeviceAddedEvent(const JoyDeviceAddedEvent &event) {
        joyRescan();
    }

    void Menu::joyDeviceRemovedEvent(const JoyDeviceRemovedEvent &event) {
        joyRescan();
    }

    void Menu::beforeClose(Context *newContext) {
        SDL_StopTextInput();
        sound.stopMusic();
        savePersonData();
        saveNetworkSettings();
    }

    void Menu::startDedicatedServer() {
        appService.getConsole().printLine(Format("Starting server"));
        game->isServer = !reverseConnection->isChecked();
        playerListBox->clear(); // remove any players that might be accidentaly set
        if(!play(true)){
            appService.getConsole().printLine("Cannot play");
        } else {
            appService.getNetHost().listen(*game, host->getText(), std::stoi(port->getText()));
            appService.getConsole().printLine(Format("Started"));
        }
    }
    void Menu::startServer(){
        game->isServer = true && !reverseConnection->isChecked();
        if(!play(true)){
            return;
        }
        appService.getNetHost().listen(*game, host->getText(), std::stoi(port->getText()));
    }

    void Menu::joinServer(){
        game->isServer = false || reverseConnection->isChecked();
        if(!play(true)){
            return;
        }

        /// TODO To make connection via server's public address and its local address in case it sits on the
        // same network as this client
        appService.getNetClient().connect(*game, host->getText(), std::stoi(port->getText()));
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
        bool cancelled = false;
        return processEvents(single, cancelled);
    }

    int Menu::processEvents(bool single, bool & cancelled) {
        SDL_Event event;
        int result = 0;
        //TODO This logic duplicates event processing logic in Application. Should be refactored.
        while ((result = SDL_PollEvent(&event)) != 0) {
            switch (event.type) {
                case SDL_KEYDOWN: {
                    auto key = event.key.keysym;
                    appService.getInput().setPressed(key.sym, true);
                    if(key.sym == SDLK_ESCAPE){
                        cancelled = true;
                    }
                    break;
                }
                case SDL_KEYUP: {
                    auto key = event.key.keysym;
                    appService.getInput().setPressed(key.sym, false);
                    break;
                }
                case SDL_JOYDEVICEADDED: {
                    appService.getConsole().printLine("Device added");

                    auto deviceIndex = event.jdevice.which;
                    auto joy = SDL_JoystickOpen(deviceIndex);
                    if (SDL_JoystickGetAttached(joy)) {
                        appService.getInput().joyAttached(joy, deviceIndex);
                        joyRescan();
                    } else {
                        appService.getConsole().printLine(
                                Format("Joy attached, but has been detached again -> skipping."));
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
            if (single) {
                return result;
            }
        }
        return result;
    }

    void Menu::consumeInputEvents() {
        processEvents();
    }

    void Menu::shufflePlayers() {
        auto playerCount = playerListBox->size();

        std::vector<Size> shuffle;
        std::vector<std::string> players;
        std::vector<Int32> controls;

        for (Size i = 0; i < playerCount; i++) {
            players.push_back(playerListBox->getItem(i));
            shuffle.push_back(i);
            controls.push_back(controlSwitch[i]->currentItem());
        }

        std::shuffle(shuffle.begin(), shuffle.end(), Math::randomEngine);

        playerListBox->clear();
        for (Size i = 0; i < playerCount; i++) {
            auto pos = shuffle[i];
            playerListBox->addItem(players[pos]);
            controlSwitch[i]->setCurrent(controls[pos]);
        }
    }

    std::vector<std::string> Menu::listMaps() {
        std::vector<std::string> levels;
        for (Size i = 0; i < levelList.getLength(); ++i) {
            levels.push_back(levelList.getPath(i));
        }
        return levels;
    }

    void Menu::serverlist() {
        appService.getNetClient().requestServerList([this](masterserver::serverlist_t & serverList) {
            this->serverList.clear();
            for(const auto & server: serverList){
                appService.getConsole().printLine(Format("Server: {0}") << hostToIPaddress(server.address, server.port));
                this->serverList.add(
                    server.address, server.port,
                    server.publicIPAddress, server.publicPort,
                    server.localNetworkAddress, server.localNetworkPort,
                    addressToStr(server.address),
                    Format("{0}") << server.port,
                    addressToStr(server.publicIPAddress),
                    Format("{0}") << server.publicPort,
                    addressToStr(server.localNetworkAddress),
                    Format("{0}") << server.localNetworkPort,
                    server.description,
                    Format("{0,8} {1} {2}")
                    << (server.enableNAT ? "P2P" : "direct")
                    << hostToIPaddress(server.address, server.port)
                    << server.description,
                    server.enableNAT);
            }
            this->serverList.notify();
        });
    }

    void Menu::eloShufflePlayers() {
        auto playerCount = playerListBox->size();

        std::vector<const Person *> players;
        std::unordered_map<std::string, Int32> controls;
        std::unordered_map<std::string, Int32> team;

        for (Size i = 0; i < playerCount; i++) {
            std::string name = playerListBox->getItem(i);
            Person &person = persons.getByName(name);
            players.push_back(&person);
            controls[name] = controlSwitch[i]->currentItem();
            team[name] = teamControlSwitch[i]->currentItem();
        }

        std::sort(players.begin(), players.end(), [](const Person *left, const Person *right) {
            return left->getElo() > right->getElo();
        });

        std::vector<Size> shuffle;
        for (Size i = 0; i < playerCount; i++) {
            shuffle.push_back(i);

        }

        Int32 teamPlayerCount = 1 + gameModeSwitch->currentItem() / 2;
        for (Int32 start = 0; start < Int32(playerCount); start += teamPlayerCount) {
            auto span = std::min(teamPlayerCount, Int32(playerCount) - start);
            auto first = shuffle.begin() + start;
            std::shuffle(first, first + span, Math::randomEngine);
        }

        playerListBox->clear();
        for (Size i = 0; i < playerCount; i++) {
            auto pos = shuffle[i];
            std::string name = players[pos]->getName();
            playerListBox->addItem(name);
            controlSwitch[i]->setCurrent(controls[name]);
            teamControlSwitch[i]->setCurrent(team[name]);
        }
    }
    void Menu::ServerList::add(enet_uint32 netAddress, enet_uint16 netPort,
                               enet_uint32 publicAddress, enet_uint16 publicPort,
                               enet_uint32 localAddress, enet_uint16 localPort,
                               const std::string &address, const std::string &port,
                               const std::string &publicAddressStr, const std::string &publicPortStr,
                               const std::string &localAddressStr, const std::string &localPortStr,
                               const std::string &descr, const std::string &text,
                               bool enableNAT) {
        list.emplace_back(Server{netAddress, netPort,
            publicAddress, publicPort,
            localAddress, localPort,
            address, port,
            publicAddressStr, publicPortStr,
            localAddressStr, localPortStr,
            descr, text,
            enableNAT});
    }

    void Menu::ServerList::clear(){
        list.clear();
    }

    void Menu::ServerList::notify(){
        callback(list);
    }

    Menu::ServerList::callbackReset_t Menu::ServerList::setCallback(callback_t cb){
        callback = cb;
        return [this](){
            clearCallback();
        };
    }

    const std::vector<Menu::Server>& Menu::ServerList::get() const {
        return list;
    }

    std::string Menu::getGameState(){
        return "{\"state\": \"MENU\"}";
    }

    void Menu::joinServerFromServerList(Int32 index){
        const auto &server = this->serverList.get()[index];

        std::string addressText = server.address;
        std::string portText = server.port;
        if (server.enableNAT) {
            if (server.publicIPAddress != 0 && server.publicPort != 0) {
                addressText = server.publicIPAddressStr;
                portText = server.publicPortStr;
            }
            appService.getNetClient().requestNATPunch(server.netAddress, server.netPort,
                server.publicIPAddress, server.publicPort,
                server.localAddress, server.localPort);
        } else {
            if (server.publicIPAddress != 0 && server.publicPort != 0) {
                addressText = server.publicIPAddressStr;
                portText = server.publicPortStr;
            }
        }
        this->host->setText(addressText);
        this->port->setText(portText);
        this->joinServer();
    }

}


