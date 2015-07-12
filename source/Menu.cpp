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
#define ALL_PLAYER_LIST 1
#define CUR_PLAYERS_LIST 2

namespace Duel6
{
	Menu::Menu(AppService& appService)
		: appService(appService), font(appService.getFont()), video(appService.getVideo()),
		sound(appService.getSound()), controlsManager(appService.getInput()), playMusic(false)
	{}

	void Menu::loadPersonData(const std::string& filePath)
	{
		if (!File::exists(filePath))
		{
			return;
		}

		Json::Parser parser;
		Json::Value json = parser.parse(filePath);
		persons.fromJson(json.get("persons"));

		for (const Person& person : persons.list())
		{
			listbox[ALL_PLAYER_LIST]->addItem(person.getName());
		}

		Json::Value playing = json.get("playing");
		for (Size i = 0; i < playing.getLength(); i++)
		{
			std::string name = playing.get(i).asString();
			listbox[CUR_PLAYERS_LIST]->addItem(name);
			listbox[ALL_PLAYER_LIST]->delItem(name);
		}
	}

	void Menu::joyRescan()
	{
		appService.getInput().joyScan(appService.getConsole());
		Size controls = controlsManager.getNumAvailable();

		for (Size i = 0; i < D6_MAX_PLAYERS; i++)
		{
			Gui::Spinner* control = controlSwitch[i];
			control->clear();
			for (Size j = 0; j < controls; j++)
			{
				control->addItem(controlsManager.get(j).getDescription());
			}
			control->setCur(i % controls);
		}
	}

	void Menu::initialize()
	{
		appService.getConsole().printLine(D6_L("\n===Menu initialization==="));
		menuBannerTexture = appService.getTextureManager().load(D6_TEXTURE_MENU_PATH, GL_LINEAR, true);
		appService.getConsole().printLine(D6_L("...Starting GUI library"));
		gui.screenSize(video.getScreen().getClientWidth(), video.getScreen().getClientHeight(),
			(video.getScreen().getClientWidth() - 800) / 2, (video.getScreen().getClientHeight() - 600) / 2);

		listbox[0] = new Gui::Listbox(gui, true);
		listbox[0]->setPosition(10, 199, 94, 12, 16);

		listbox[ALL_PLAYER_LIST] = new Gui::Listbox(gui, true);
		listbox[ALL_PLAYER_LIST]->setPosition(10, 470, 20, 13, 18);

		listbox[CUR_PLAYERS_LIST] = new Gui::Listbox(gui, false);
		listbox[CUR_PLAYERS_LIST]->setPosition(200, 470, 20, D6_MAX_PLAYERS, 18);

		listbox[3] = new Gui::Listbox(gui, true);
		listbox[3]->setPosition(654, 410, 13, 6, 16);

		listbox[4] = new Gui::Listbox(gui, true);
		listbox[4]->setPosition(520, 363, 13, 3, 16);

		listbox[5] = new Gui::Listbox(gui, false);
		listbox[5]->setPosition(654, 470, 15, 2, 16);
		listbox[5]->addItem(D6_L("Fullscreen"));
		listbox[5]->addItem(D6_L("Split screen"));

		listbox[6] = new Gui::Listbox(gui, true);
		listbox[6]->setPosition(520, 470, 13, 5, 16);

		loadPersonProfiles(D6_FILE_PROFILES);
		loadPersonData(D6_FILE_PHIST);

		button[0] = new Gui::Button(gui);
		button[0]->setPosition(200, 282, 80, 25);
		button[0]->setCaption(">>");
		button[0]->onClick([this](const Gui::Event&) {
			addPlayer(listbox[ALL_PLAYER_LIST]->selectedIndex());
		});

		button[1] = new Gui::Button(gui);
		button[1]->setPosition(200, 253, 80, 25);
		button[1]->setCaption("<<");
		button[1]->onClick([this](const Gui::Event&) {
			removePlayer(listbox[CUR_PLAYERS_LIST]->selectedIndex());
		});

		button[2] = new Gui::Button(gui);
		button[2]->setPosition(284, 282, 80, 25);
		button[2]->setCaption(D6_L("Remove"));
		button[2]->onClick([this](const Gui::Event&) {
			deletePerson();
			rebuildTable();
		});

		button[3] = new Gui::Button(gui);
		button[3]->setPosition(284, 253, 80, 25);
		button[3]->setCaption(D6_L("Add"));
		button[3]->onClick([this](const Gui::Event&) {
			addPerson();
		});

		button[6] = new Gui::Button(gui);
		button[6]->setPosition(370, 282, 125, 25);
		button[6]->setCaption(D6_L("Clear (F3)"));
		button[6]->onClick([this](const Gui::Event&) {
			if (deleteQuestion())
			{
				cleanPersonData();
			}
		});

		button[4] = new Gui::Button(gui);
		button[4]->setPosition(520, 299, 125, 73);
		button[4]->setCaption(D6_L("Play (F1)"));
		button[4]->onClick([this](const Gui::Event&) {
			play();
		});

		button[5] = new Gui::Button(gui);
		button[5]->setPosition(654, 299, 125, 73);
		button[5]->setCaption(D6_L("Quit (ESC)"));
		button[5]->onClick([this](const Gui::Event&) {
			close();
		});

		label[0] = new Gui::Label(gui);
		label[0]->setPosition(10, 219, 772, 18);
		label[0]->setCaption(D6_L("      Name       |  Games  |   Wins  |  Shots  | Accuracy |  Kills  | Penalties |  Points"));

		label[1] = new Gui::Label(gui);
		label[1]->setPosition(520, 383, 125, 18);
		label[1]->setCaption(D6_L("Background"));

		label[2] = new Gui::Label(gui);
		label[2]->setPosition(654, 429, 125, 18);
		label[2]->setCaption(D6_L("Level"));

		label[3] = new Gui::Label(gui);
		label[3]->setPosition(654, 489, 125, 18);
		label[3]->setCaption(D6_L("Screen mode"));

		label[4] = new Gui::Label(gui);
		label[4]->setPosition(520, 489, 125, 18);
		label[4]->setCaption(D6_L("Zoom"));

		label[5] = new Gui::Label(gui);
		label[5]->setPosition(10, 489, 181, 18);
		label[5]->setCaption(D6_L("Persons"));

		label[6] = new Gui::Label(gui);
		label[6]->setPosition(200, 489, 165, 18);
		label[6]->setCaption(D6_L("Players"));

		label[7] = new Gui::Label(gui);
		label[7]->setPosition(370, 489, 120, 18);
		label[7]->setCaption(D6_L("Controller"));

		textbox = new Gui::Textbox(gui);
		textbox->setPosition(370, 252, 14, 10, D6_ALL_CHR);

		// Switchbox - volba ovladani
		for (Size i = 0; i < D6_MAX_PLAYERS; i++)
		{
			controlSwitch[i] = new Gui::Spinner(gui);
			controlSwitch[i]->setPosition(370, 468 - i * 18, 120, 0);

			// TODO: Might be deleted in future if the use case is fully covered with detect-all functionality
			Gui::Button* button = new Gui::Button(gui);
			button->setCaption("D");
			button->setPosition(494, 466 - i * 18, 17, 17);
			button->onClick([this,i](const Gui::Event&) {
				detectControls(i);
			});
		}

		// Button to detect all user's controllers in a batch
		Gui::Button* button = new Gui::Button(gui);
		button->setCaption("D");
		button->setPosition(490, 487, 24, 17);
		button->onClick([this](const Gui::Event& ){
			Size curPlayersCount = listbox[CUR_PLAYERS_LIST]->size();
			for (Size j = 0; j < curPlayersCount; j++)
			{
				detectControls(j);
			}
		});

		initializeGameModes();
		gameModeSwitch = new Gui::Spinner(gui);
		for(auto& gameMode : gameModes)
		{
			gameModeSwitch->addItem(gameMode->getName());
		}
		gameModeSwitch->setPosition(10,0, 280, 20);

		joyRescan();

		backgroundCount = File::countFiles(D6_TEXTURE_BCG_PATH, D6_TEXTURE_EXTENSION);
		levelList.initialize(D6_FILE_LEVEL, D6_LEVEL_EXTENSION);

		listbox[3]->addItem(D6_L("Random"));
		for (Size i = 0; i < levelList.getLength(); i++)
		{
			listbox[3]->addItem(levelList.getFileName(i).substr(0, levelList.getFileName(i).rfind(".")));
		}

		listbox[4]->addItem(D6_L("Random"));
		for (Size i = 0; i < backgroundCount; i++)
		{
			listbox[4]->addItem(std::to_string(i + 1));
		}

		for (Int32 i = 5; i < 21; i++)
		{
			listbox[6]->addItem(std::to_string(i));
		}
		listbox[6]->setCur(8);

		menuTrack = sound.loadModule("sound/undead.xm");
	}

	void Menu::initializeGameModes()
	{
		gameModes.push_back(std::make_unique<DeathMatch>());
		gameModes.push_back(std::make_unique<TeamDeathMatch>(2, false));
		gameModes.push_back(std::make_unique<TeamDeathMatch>(2, true));
		gameModes.push_back(std::make_unique<TeamDeathMatch>(3, false));
		gameModes.push_back(std::make_unique<TeamDeathMatch>(3, true));
		gameModes.push_back(std::make_unique<Predator>());
	}

	void Menu::savePersonData()
	{
		Json::Value json = Json::Value::makeObject();
		json.set("persons", persons.toJson());

		Json::Value playing = Json::Value::makeArray();
		for (Size i = 0; i < listbox[CUR_PLAYERS_LIST]->size(); i++)
		{
			playing.add(Json::Value::makeString(listbox[CUR_PLAYERS_LIST]->getItem(i)));
		}
		json.set("playing", playing);

		Json::Writer writer(true);
		writer.writeToFile(D6_FILE_PHIST, json);
	}

	void Menu::rebuildTable()
	{
		listbox[0]->clear();
		if (persons.isEmpty())
			return;

		std::vector<const Person*> ranking;

		for (const Person& person : persons.list())
		{
			ranking.push_back(&person);
		}

		std::sort(ranking.begin(), ranking.end(), [](const Person* left, const Person* right) {
			return left->hasHigherScoreThan(*right);
		});

		for (auto person : ranking)
		{
			std::string personStat = Format("{0,-17}|{1,8} |{2,8} |{3,8} |{4,8}% |{5,8} |{6,10} |{7,10}")
				<< person->getName()
				<< person->getGames()
				<< person->getWins()
				<< person->getShots()
				<< person->getAccuracy()
				<< person->getKills()
				<< person->getPenalties()
				<< person->getTotalPoints();
			listbox[0]->addItem(personStat);
		}
	}

	void Menu::showMessage(const std::string& message)
	{
		Int32 width = message.size() * 8 + 60;
		Int32 x = video.getScreen().getClientWidth() / 2 - width / 2,
			  y = video.getScreen().getClientHeight() / 2 - 10;

		glColor3f(1.0f, 0.8f, 0.8f);
		glBegin(GL_QUADS);
			glVertex2i(x, y);
			glVertex2i(x, y + 20);
			glVertex2i(x + width, y + 20);
			glVertex2i(x + width, y);
		glEnd();
		glColor3f(0, 0, 0);
		glLineWidth(2);
		glBegin(GL_LINE_LOOP);
			glVertex2i(x, y);
			glVertex2i(x, y + 20);
			glVertex2i(x + width, y + 20);
			glVertex2i(x + width, y);
		glEnd();
		glLineWidth(1);
		font.print(x + 30, y + 2, Color(255, 0, 0), message);
		video.screenUpdate(appService.getConsole(), font);
	}

	bool Menu::question(const std::string& question)
	{
		showMessage(question);
		SDL_Event event;
		bool answer;
		while (true)
		{
			if (SDL_PollEvent(&event))
			{
				if (event.type == SDL_KEYDOWN)
				{
					if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_y)
					{
						answer = true;
						break;
					}
					else if (event.key.keysym.sym == SDLK_n)
					{
						answer = false;
						break;
					}
				}
			}
		}

		while (SDL_PollEvent(&event))
		{
			// Eat all remaining keyboard events;
		}

		return answer;
	}

	bool Menu::deleteQuestion()
	{
		return question(D6_L("Really delete? (Y/N)"));
	}

	void Menu::cleanPersonData()
	{
		for (Person& person : persons.list())
		{
			person.reset();
		}
		rebuildTable();
		savePersonData();
	}

	void Menu::detectControls(Size playerIndex)
	{
		render();
		showMessage("Player " + listbox[CUR_PLAYERS_LIST]->getItem(playerIndex)  + ": Press any control");

		SDL_Event event;
		SDL_Keysym key;
		bool detected = false;

		while (!detected)
		{
			if (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
				case SDL_KEYDOWN:
					key = event.key.keysym;
					appService.getInput().setPressed(key.sym, true);
					break;
				case SDL_KEYUP:
					key = event.key.keysym;
					appService.getInput().setPressed(key.sym, false);
					break;
				}

				for (Size i = 0; i < controlsManager.getNumAvailable(); i++)
				{
					const PlayerControls& pc = controlsManager.get(i);

					if (pc.getLeft().isPressed() ||
						pc.getRight().isPressed() ||
						pc.getDown().isPressed() ||
						pc.getUp().isPressed() ||
						pc.getShoot().isPressed() ||
						pc.getPick().isPressed())
					{
						controlSwitch[playerIndex]->setCur(i);
						detected = true;
					}
				}
			}
		}

		while (SDL_PollEvent(&event))
		{
			// Eat all remaining keyboard events;
		}
	}

	void Menu::play()
	{
		if (listbox[CUR_PLAYERS_LIST]->size() < 2)
		{
			// TODO: message
			return;
		}

		if (game->getSettings().isRoundLimit())
		{
			if(!question(D6_L("Clear statistics and start a new game? (Y/N)")))
			{
				return;

			}
			cleanPersonData();
		}

		// Persons, colors, controls
		std::vector<Game::PlayerDefinition> playerDefinitions;
		for (Size i = 0; i < listbox[CUR_PLAYERS_LIST]->size(); i++)
		{
			Person& person = persons.getByName(listbox[CUR_PLAYERS_LIST]->getItem(i));
			auto& profile = getPersonProfile(person.getName(), i);
			const PlayerControls& controls = controlsManager.get(controlSwitch[i]->curItem());
			playerDefinitions.push_back(Game::PlayerDefinition(person, profile.getSkinColors(), profile.getSounds(), controls));
		}

		// Levels
		std::vector<std::string> levels;
		if (!listbox[3]->selectedIndex())
		{
			for (Size i = 0; i < levelList.getLength(); ++i)
			{
				levels.push_back(levelList.getPath(i));
			}
		}
		else
		{
			levels.push_back(levelList.getPath(listbox[3]->selectedIndex() - 1));
		}

		// Game backgrounds
		std::vector<Size> backgrounds;
		if (!listbox[4]->selectedIndex())
		{
			for (Size i = 0; i < backgroundCount; i++)
			{
				backgrounds.push_back(i);
			}
		}
		else
		{
			backgrounds.push_back(listbox[4]->selectedIndex() - 1);
		}

		// Screen
		ScreenMode screenMode = (listbox[CUR_PLAYERS_LIST]->size() > 4 || listbox[5]->selectedIndex() == 0) ? ScreenMode::FullScreen : ScreenMode::SplitScreen;
		Int32 screenZoom = listbox[6]->selectedIndex() + 5;

		// Start
		Context::push(*game);
		GameMode& selectedMode = *gameModes[gameModeSwitch->curItem()];
		game->start(playerDefinitions, levels, backgrounds, screenMode, screenZoom, selectedMode);
	}

	void Menu::addPlayer(Int32 c)
	{
		if (c != -1 && listbox[CUR_PLAYERS_LIST]->size() < D6_MAX_PLAYERS)
		{
			const std::string& name = listbox[ALL_PLAYER_LIST]->getItem(c);
			for (Size i = 0; i < listbox[CUR_PLAYERS_LIST]->size(); i++)
			{
				if(name.compare(listbox[CUR_PLAYERS_LIST]->getItem(i)) == 0)
				{
					return;
				}
			}
			listbox[CUR_PLAYERS_LIST]->addItem(name);
			listbox[ALL_PLAYER_LIST]->delItem(c);
		}
	}

	void Menu::removePlayer(Int32 c)
	{
		if (c != -1)
		{
			listbox[ALL_PLAYER_LIST]->addItem(listbox[CUR_PLAYERS_LIST]->getItem(c));
			listbox[CUR_PLAYERS_LIST]->delItem(c);
		}
	}

	void Menu::addPerson()
	{
		const std::string& personName = textbox->getText();

		if (!personName.empty() && !persons.contains(personName))
		{
			persons.add(Person(personName));
			listbox[ALL_PLAYER_LIST]->addItem(personName);
			rebuildTable();
			textbox->flush();
		}
	}

	void Menu::deletePerson()
	{
		if (!deleteQuestion())
			return;

		Int32 c = listbox[ALL_PLAYER_LIST]->selectedIndex();
		if (c != -1)
		{
			const std::string& playerName = listbox[ALL_PLAYER_LIST]->selectedItem();
			listbox[ALL_PLAYER_LIST]->delItem(playerName);
			persons.remove(playerName);
		}
	}

	void Menu::beforeStart(Context* prevContext)
	{
		SDL_ShowCursor(SDL_ENABLE);
		SDL_StartTextInput();
		rebuildTable();
		if (playMusic)
		{
			menuTrack.play(false);
		}
	}

	void Menu::update(Float32 elapsedTime)
	{
		static Float32 sync = 0, wait = 0.0163f;

		sync += elapsedTime;
		while (sync > wait)
		{
			sync -= wait;
			gui.update();
		}
	}

	void Menu::render() const
	{
		Int32 tr_x = (video.getScreen().getClientWidth() - 800) / 2, tr_y = (video.getScreen().getClientHeight() - 600) / 2;

		gui.draw(font);

		glPushMatrix();
		glTranslatef((GLfloat)tr_x, (GLfloat)-tr_y, 0);

		font.print(687, video.getScreen().getClientHeight() - 20, Color(255, 255, 255), Format("{0} {1}") << D6_L("version") << APP_VERSION);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, menuBannerTexture.getGlTextures()[0]);
		glColor3ub(255, 255, 255);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2i(300, video.getScreen().getClientHeight() - 5);
			glTexCoord2f(1.0f, 0.0f); glVertex2i(500, video.getScreen().getClientHeight() - 5);
			glTexCoord2f(1.0f, 1.0f); glVertex2i(500, video.getScreen().getClientHeight() - 100);
			glTexCoord2f(0.0f, 1.0f); glVertex2i(300, video.getScreen().getClientHeight() - 100);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
	}

	void Menu::keyEvent(SDL_Keycode keyCode, Uint16 keyModifiers)
	{
		gui.keyEvent(keyCode);

		if (keyCode == SDLK_RETURN)
		{
			addPerson();
		}

		if (keyCode == SDLK_F1)
		{
			play();
		}

		if (keyCode == SDLK_F3)
		{
			if (deleteQuestion())
			{
				cleanPersonData();
			}
		}

		if (keyCode == SDLK_ESCAPE)
		{
			close();
		}
	}

	void Menu::textInputEvent(const char* text)
	{
		gui.textInputEvent(text);
	}

	void Menu::beforeClose(Context* newContext)
	{
		SDL_StopTextInput();
		sound.stopMusic();
		savePersonData();
	}

	void Menu::enableMusic(bool enable)
	{
		playMusic = enable;

		if (isCurrent())
		{
			if (enable)
			{
				menuTrack.play(false);
			}
			else
			{
				sound.stopMusic();
			}
		}
	}

	void Menu::loadPersonProfiles(const std::string& path)
	{
		appService.getConsole().printLine("\n===Person profile initialization===");

		std::vector<std::string> profileDirs;
		File::listDirectory(path, "", profileDirs);
		for (auto& profileName : profileDirs)
		{
			std::string profilePath = Format("{0}/{1}/") << path << profileName;
			personProfiles.insert(std::make_pair(profileName, PersonProfile(sound, profilePath)));
		}
	}

	PersonProfile& Menu::getPersonProfile(const std::string& name, Size index)
	{
		auto profile = personProfiles.find(name);
		if (profile != personProfiles.end())
		{
			return profile->second;
		}

		std::string defaultProfileName = Format("default_{0}") << index;
		return personProfiles.at(defaultProfileName);
	}
}