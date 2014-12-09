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
#include <stdarg.h>
#include "Sound.h"
#include "Video.h"
#include "PlayerControls.h"
#include "TextureManager.h"
#include "Menu.h"
#include "Game.h"
#include "Util.h"
#include "File.h"
#include "Font.h"
#include "Globals.h"

#define D6_ALL_CHR  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 -=\\~!@#$%^&*()_+|[];',./<>?:{}"

namespace Duel6
{
	static const char   *d6SndFl[D6_SOUNDS] =
	{
		"sound/smrt.wav",
		"sound/bnpick.wav",
		"sound/blesk.wav",
		"sound/bmblesk.wav",
		"sound/laser.wav",
		"sound/pistol.wav",
		"sound/plasma.wav",
		"sound/shotgun.wav",
		"sound/bazooka.wav",
		"sound/bmbazook.wav",
		"sound/voda.wav",
		"sound/triton.wav",
		"sound/kulomet.wav",
		"sound/hit.wav",
		"sound/luk.wav",
		"sound/sliz.wav",
		"sound/kiss2.wav",
		"sound/kiss.wav",
		"sound/spray.wav",
		"sound/prak.wav",
		"sound/spunt.wav",
		"sound/shit.wav",
		"sound/shit-hit.wav",
		"sound/letsrock.wav",
		"sound/gameover.wav"
	};

	Menu::Menu(Video& video, Input& input, const Font& font)		
		: video(video), input(input), font(font), controlsManager(input), playMusic(false)
	{}

	void Menu::loadPersonData()
	{
		if (File::getSize(D6_FILE_PHIST) < 20)
		{
			return;
		}

		File file(D6_FILE_PHIST, "rb");
		persons.load(file);

		Int32 playing;
		file.read(&playing, 4, 1);
		for (Int32 i = 0; i < playing; i++)
		{
			Int32 personIndex;
			file.read(&personIndex, 4, 1);
			playingPersons.push_back(personIndex);
		}
		
		file.close();;
	}

	/*
	==================================================
	Umoznuje hot plug joypadu za behu hry. Vyhleda
	nove pripojena zarizeni.
	==================================================
	*/
	void Menu::joyRescan()
	{
		d6Console.print(D6_L("\n===Initialization of input devices===\n"));
		input.joyScan(d6Console);

		for (Size i = 0; i < D6_MAX_PLAYERS; i++)
		{
			controlSwitch[i]->clear();
		}

		for (Int32 i = 0; i < 4; i++)
		{			
			std::string keyboardDevice = Format("{0} {1}") << D6_L("Keys") << (i + 1);
			for (Size j = 0; j < D6_MAX_PLAYERS; j++)
			{
				controlSwitch[j]->addItem(keyboardDevice);
			}
		}

		for (Uint32 i = 0; i < input.getNumJoypads(); i++)
		{
			std::string joypadDevice = Format("{0} {1}") << D6_L("Joypad") << (i + 1);
			for (Size j = 0; j < D6_MAX_PLAYERS; j++)
			{
				controlSwitch[j]->addItem(joypadDevice);
			}
		}

		for (Size i = 0; i < D6_MAX_PLAYERS; i++)
		{
			controlSwitch[i]->setCur(i % (4 + input.getNumJoypads()));
		}
	}

	void Menu::initialize()
	{		
		d6Console.print(D6_L("\n===Menu initialization===\n"));
		menuBannerTexture = d6TextureManager.get(D6_TEXTURE_MENU_KEY)[0];
		loadPersonData();
		d6Console.print(D6_L("...Starting GUI library\n"));
		gui.screenSize(video.getScreen().getClientWidth(), video.getScreen().getClientHeight(),
			(video.getScreen().getClientWidth() - 800) / 2, (video.getScreen().getClientHeight() - 600) / 2);

		listbox[0] = new Gui::Listbox(gui, true);
		listbox[0]->setPosition(10, 199, 94, 12, 16);
		listbox[0]->setNG(12, 1);

		listbox[1] = new Gui::Listbox(gui, true);
		listbox[1]->setPosition(10, 470, 20, 13, 18);
		listbox[1]->setNG(0, 1);

		listbox[2] = new Gui::Listbox(gui, false);
		listbox[2]->setPosition(200, 470, 20, D6_MAX_PLAYERS, 18);
		listbox[2]->setNG(1, 1);

		listbox[3] = new Gui::Listbox(gui, true);
		listbox[3]->setPosition(644, 410, 13, 6, 16);
		listbox[3]->setNG(2, 1);

		listbox[4] = new Gui::Listbox(gui, true);
		listbox[4]->setPosition(500, 363, 13, 3, 16);
		listbox[4]->setNG(3, 1);

		listbox[5] = new Gui::Listbox(gui, false);
		listbox[5]->setPosition(644, 470, 15, 2, 16);
		listbox[5]->setNG(4, 1);
		listbox[5]->addItem(D6_L("Fullscreen"));
		listbox[5]->addItem(D6_L("Split screen"));

		listbox[6] = new Gui::Listbox(gui, true);
		listbox[6]->setPosition(500, 470, 13, 5, 16);
		listbox[6]->setNG(5, 1);

		button[0] = new Gui::Button(gui);
		button[0]->setPosition(200, 318, 80, 31);
		button[0]->setCaption(">>");
		button[0]->setNG(6, 1);

		button[1] = new Gui::Button(gui);
		button[1]->setPosition(200, 283, 80, 31);
		button[1]->setCaption("<<");
		button[1]->setNG(7, 1);

		button[2] = new Gui::Button(gui);
		button[2]->setPosition(284, 318, 80, 31);
		button[2]->setCaption(D6_L("Remove"));
		button[2]->setNG(8, 1);

		button[3] = new Gui::Button(gui);
		button[3]->setPosition(284, 283, 80, 31);
		button[3]->setCaption(D6_L("Add"));
		button[3]->setNG(9, 1);

		button[6] = new Gui::Button(gui);
		button[6]->setPosition(370, 318, 120, 31);
		button[6]->setCaption(D6_L("Clear (F3)"));
		button[6]->setNG(30, 1);

		button[4] = new Gui::Button(gui);
		button[4]->setPosition(500, 299, 125, 73);
		button[4]->setCaption(D6_L("Play (F1)"));
		button[4]->setNG(10, 1);

		button[5] = new Gui::Button(gui);
		button[5]->setPosition(644, 299, 125, 73);
		button[5]->setCaption(D6_L("Quit (ESC)"));
		button[5]->setNG(11, 1);

		label[0] = new Gui::Label(gui);
		label[0]->setPosition(10, 219, 772, 18);
		label[0]->setCaption(D6_L("      Name       |  Games  |   Wins  |  Shots  | Accuracy |  Kills  | Penalties |  Points"));

		label[1] = new Gui::Label(gui);
		label[1]->setPosition(500, 383, 125, 18);
		label[1]->setCaption(D6_L("Background"));

		label[2] = new Gui::Label(gui);
		label[2]->setPosition(644, 429, 125, 18);
		label[2]->setCaption(D6_L("Level"));

		label[3] = new Gui::Label(gui);
		label[3]->setPosition(644, 489, 125, 18);
		label[3]->setCaption(D6_L("Screen mode"));

		label[4] = new Gui::Label(gui);
		label[4]->setPosition(500, 489, 125, 18);
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
		textbox->setPosition(200, 248, 19, 10, D6_ALL_CHR);
		textbox->setNG(13, 1);

		// Switchbox - volba ovladani
		for (Size i = 0; i < D6_MAX_PLAYERS; i++)
		{
			controlSwitch[i] = new Gui::Combobox(gui);
			controlSwitch[i]->setPosition(370, 468 - i * 18, 120, 0);
			controlSwitch[i]->setNG(14 + i, 1);
		}

		joyRescan();

		backgroundCount = d6TextureManager.get(D6_TEXTURE_BCG_KEY).size();
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

		for (const Person& person : persons.list())
		{
			listbox[1]->addItem(person.getName());
		}

		for (Int32 personIndex : playingPersons)
		{
			listbox[2]->addItem(persons.get(personIndex).getName());
		}

		d6Console.print(D6_L("\n===Loading sound data===\n"));
		Sound::loadModule("sound/undead.xm");
		for (Size i = 0; i < D6_SOUNDS; i++)
		{
			Sound::loadSample(d6SndFl[i]);
		}
	}

	void Menu::savePersonData()
	{
		File file(D6_FILE_PHIST, "wb");
		persons.save(file);

		Int32 playing = playingPersons.size();
		file.write(&playing, 4, 1);
		for (Int32 personIndex : playingPersons)
		{
			file.write(&personIndex, 4, 1);
		}
		
		file.close();
	}

	void Menu::rebuildTable()
	{
		listbox[0]->clear();
		if (persons.isEmpty())
			return;

		std::vector<Size> pi(persons.getLength());
		for (Size i = 0; i < persons.getLength(); i++)
		{
			pi[i] = i;
		}

		// Bubble sort persons according to total points
		for (Size k = 0; k < persons.getLength() - 1; k++)
		{
			for (Size i = 0; i < persons.getLength() - 1 - k; i++)
			{
				if (persons.get(pi[i + 1]).getTotalPoints() > persons.get(pi[i]).getTotalPoints())
				{
					std::swap(pi[i], pi[i + 1]);
				}
			}
		}

		for (Size i = 0; i < persons.getLength(); i++)
		{
			const Person& person = persons.get(pi[i]);
			Int32 accuracy = (person.getShots() > 0) ? person.getHits() * 100 / person.getShots() : 0;
			std::string personStat = Format("{0,-17}|{1,8} |{2,8} |{3,8} |{4,8}% |{5,8} |{6,10} |{7,10}") 
				<< person.getName()
				<< person.getGames() 
				<< person.getWins() 
				<< person.getShots() 
				<< accuracy 
				<< person.getKills() 
				<< person.getPenalties() 
				<< person.getTotalPoints();
			listbox[0]->addItem(personStat);
		}
	}

	void Menu::addPlayer()
	{
		Int32 c = listbox[1]->curItem();
		if (c != -1 && playingPersons.size() < D6_MAX_PLAYERS)
		{
			if (std::find(playingPersons.begin(), playingPersons.end(), c) != playingPersons.end())
			{
				return;
			}
			playingPersons.push_back(c);
			listbox[2]->addItem(persons.get(c).getName());
		}
	}

	void Menu::addPerson()
	{
		const std::string& personName = textbox->getText();

		if (!personName.empty())
		{
			persons.add(Person(personName));
			listbox[1]->addItem(personName);
			rebuildTable();
			textbox->flush();
		}
	}

	bool Menu::question(const std::string& question)
	{
		Int32 width = question.size() * 8 + 60;
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
		font.print(x + 30, y + 2, Color(255, 0, 0), question);		
		video.screenUpdate(d6Console, font);

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

	void Menu::play()
	{
		bool roundLimit = (game->getMaxRounds() > 0);

		if (roundLimit)
		{
			if(!question(D6_L("Clear statistics and start a new game? (Y/N)")))
			{
				return;
				
			}
			cleanPersonData();
		}
				
		// Persons, colors, controls
		std::vector<Game::PlayerDefinition> playerDefinitions;
		for (Size i = 0; i < playingPersons.size(); i++)
		{
			Person& person = persons.get(playingPersons[i]);
			auto skinForName = playerColors.find(person.getName());
			const PlayerSkinColors& colors = skinForName != playerColors.end() ? skinForName->second : playerColors.at(Format("default_{0}") << i);
			const PlayerControls& controls = controlsManager.get(controlSwitch[i]->curItem());
			playerDefinitions.push_back(Game::PlayerDefinition(person, colors, controls));
		}

		// Levels
		std::vector<std::string> levels;
		if (!listbox[3]->curItem())
		{
			for (Size i = 0; i < levelList.getLength(); ++i)
			{
				levels.push_back(levelList.getPath(i));
			}
		}
		else
		{
			levels.push_back(levelList.getPath(listbox[3]->curItem() - 1));
		}

		// Game backgrounds
		std::vector<Size> backgrounds;
		if (!listbox[4]->curItem())
		{
			for (Size i = 0; i < backgroundCount; i++)
			{
				backgrounds.push_back(i);
			}
		}
		else
		{
			backgrounds.push_back(listbox[4]->curItem() - 1);
		}

		// Screen
		ScreenMode screenMode = (playingPersons.size() > 4 || listbox[5]->curItem() == 0) ? ScreenMode::FullScreen : ScreenMode::SplitScreen;
		Int32 screenZoom = listbox[6]->curItem() + 5; 

		// Start
		Context::push(*game);
		game->start(playerDefinitions, levels, backgrounds, screenMode, screenZoom);
	}

	void Menu::removePlayer(Int32 c)
	{
		if (c != -1)
		{
			listbox[2]->delItem(c);
			playingPersons.erase(playingPersons.begin() + c);
		}
	}

	void Menu::deletePerson()
	{
		if (!deleteQuestion())
			return;

		int c = listbox[1]->curItem();
		if (c != -1)
		{
			listbox[1]->delItem(c);
			auto iter = std::find(playingPersons.begin(), playingPersons.end(), c);
			if (iter != playingPersons.end())
			{
				removePlayer(iter - playingPersons.begin());
			}

			for (Int32& personIndex : playingPersons)
			{
				if (personIndex >= c)
				{
					--personIndex;
				}
			}

			persons.remove(c);
		}
	}

	void Menu::beforeStart(Context* prevContext)
	{
		SDL_ShowCursor(SDL_ENABLE);
		SDL_StartTextInput();
		rebuildTable();
		if (playMusic)
		{
			Sound::startMusic(0, false);
		}
	}

	void Menu::update(Float32 elapsedTime)
	{
		static  float sync = 0, wait = 0.0163f;
		Gui::EventType event;
		Gui::Control* from;

		sync += elapsedTime;

		while (sync > wait)
		{
			sync -= wait;

			gui.check(event, from);
			if (event == Gui::EventType::Released && from->getGroup() == 1)
			{
				// TODO: Eliminate nubmer and group and change to callbacks
				switch (from->getNumber())
				{
				case 6:
					addPlayer();
					break;
				case 7:
					removePlayer(listbox[2]->curItem());
					break;
				case 8:
					deletePerson();
					rebuildTable();
					break;
				case 9:
					addPerson();
					break;
				case 10:
					if (playingPersons.size() > 1)
					{
						play();
					}
					break;
				case 11:
					sendQuitEvent();
					return;
				case 30:
					if (deleteQuestion())
					{
						cleanPersonData();
					}
					break;
				}
			}
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
		glBindTexture(GL_TEXTURE_2D, menuBannerTexture);
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

		if (keyCode == SDLK_F1 && playingPersons.size() > 1)
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
			sendQuitEvent();
		}
	}

	void Menu::textInputEvent(const char* text)
	{
		gui.textInputEvent(text);
	}

	void Menu::beforeClose(Context* newContext)
	{
		SDL_StopTextInput();
		Sound::stopMusic();
		savePersonData();
	}

	void Menu::enableMusic(bool enable)
	{
		playMusic = enable;

		if (isCurrent())
		{
			if (enable)
			{
				Sound::startMusic(0, false);
			}
			else
			{
				Sound::stopMusic();
			}
		}
	}

	void Menu::sendQuitEvent()
	{
		SDL_Event quitEvent;
		quitEvent.type = SDL_QUIT;
		SDL_PushEvent(&quitEvent);
	}
}