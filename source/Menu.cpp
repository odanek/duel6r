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
			for (Size i = 0; i < D6_MAX_PLAYERS; i++)
			{
				willPlay[i] = -1;
			}
			return;
		}

		File file(D6_FILE_PHIST, "rb");
		persons.load(file);		
		file.read(willPlay, 4, 8);
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
			controlSwitch[i]->Clear();
		}

		for (Int32 i = 0; i < 4; i++)
		{			
			std::string keyboardDevice = Format("{0} {1}") << D6_L("Keys") << (i + 1);
			for (Size j = 0; j < D6_MAX_PLAYERS; j++)
			{
				controlSwitch[j]->AddItem(keyboardDevice);
			}
		}

		for (Uint32 i = 0; i < input.getNumJoypads(); i++)
		{
			std::string joypadDevice = Format("{0} {1}") << D6_L("Joypad") << (i + 1);
			for (Size j = 0; j < D6_MAX_PLAYERS; j++)
			{
				controlSwitch[j]->AddItem(joypadDevice);
			}
		}

		for (Size i = 0; i < D6_MAX_PLAYERS; i++)
		{
			controlSwitch[i]->SetCur(i % (4 + input.getNumJoypads()));
		}
	}

	void Menu::initialize()
	{		
		d6Console.print(D6_L("\n===Menu initialization===\n"));
		menuBannerTexture = d6TextureManager.get(D6_TEXTURE_MENU_KEY)[0];
		loadPersonData();
		d6Console.print(D6_L("...Starting GUI library\n"));
		desk = Desk::Create();
		desk->ScreenSize(video.getScreen().getClientWidth(), video.getScreen().getClientHeight(),
			(video.getScreen().getClientWidth() - 800) / 2, (video.getScreen().getClientHeight() - 600) / 2);

		listbox[0] = new listbox_c(true);
		listbox[0]->SetPosition(10, 400, 94, 12, 16);
		listbox[0]->SetNG(12, 1);

		listbox[1] = new listbox_c(true);
		listbox[1]->SetPosition(10, 129, 20, 13, 18);
		listbox[1]->SetNG(0, 1);

		listbox[2] = new listbox_c(false);
		listbox[2]->SetPosition(200, 129, 20, D6_MAX_PLAYERS, 18);
		listbox[2]->SetNG(1, 1);

		listbox[3] = new listbox_c(true);
		listbox[3]->SetPosition(644, 189, 13, 6, 16);
		listbox[3]->SetNG(2, 1);

		listbox[4] = new listbox_c(true);
		listbox[4]->SetPosition(500, 236, 13, 3, 16);
		listbox[4]->SetNG(3, 1);

		listbox[5] = new listbox_c(false);
		listbox[5]->SetPosition(644, 129, 15, 2, 16);
		listbox[5]->SetNG(4, 1);
		listbox[5]->AddItem(D6_L("Fullscreen"));
		listbox[5]->AddItem(D6_L("Split screen"));

		listbox[6] = new listbox_c(true);
		listbox[6]->SetPosition(500, 129, 13, 5, 16);
		listbox[6]->SetNG(5, 1);

		button[0] = new button_c;
		button[0]->SetPosition(200, 281, 80, 31);
		button[0]->SetCaption(">>");
		button[0]->SetNG(6, 1);

		button[1] = new button_c;
		button[1]->SetPosition(200, 316, 80, 31);
		button[1]->SetCaption("<<");
		button[1]->SetNG(7, 1);

		button[2] = new button_c;
		button[2]->SetPosition(284, 281, 80, 31);
		button[2]->SetCaption(D6_L("Remove"));
		button[2]->SetNG(8, 1);

		button[3] = new button_c;
		button[3]->SetPosition(284, 316, 80, 31);
		button[3]->SetCaption(D6_L("Add"));
		button[3]->SetNG(9, 1);

		button[6] = new button_c;
		button[6]->SetPosition(370, 281, 120, 31);
		button[6]->SetCaption(D6_L("Clear (F3)"));
		button[6]->SetNG(30, 1);

		button[4] = new button_c;
		button[4]->SetPosition(500, 300, 125, 73);
		button[4]->SetCaption(D6_L("Play (F1)"));
		button[4]->SetNG(10, 1);

		button[5] = new button_c;
		button[5]->SetPosition(644, 300, 125, 73);
		button[5]->SetCaption(D6_L("Quit (ESC)"));
		button[5]->SetNG(11, 1);

		label[0] = new label_c;
		label[0]->SetPosition(10, 380, 772, 18);
		label[0]->SetCaption(D6_L("      Name       |  Games  |   Wins  |  Shots  | Accuracy |  Kills  | Penalties |  Points"));

		label[1] = new label_c;
		label[1]->SetPosition(500, 216, 125, 18);
		label[1]->SetCaption(D6_L("Background"));

		label[2] = new label_c;
		label[2]->SetPosition(644, 170, 125, 18);
		label[2]->SetCaption(D6_L("Level"));

		label[3] = new label_c;
		label[3]->SetPosition(644, 110, 125, 18);
		label[3]->SetCaption(D6_L("Screen mode"));

		label[4] = new label_c;
		label[4]->SetPosition(500, 110, 125, 18);
		label[4]->SetCaption(D6_L("Zoom"));

		label[5] = new label_c;
		label[5]->SetPosition(10, 110, 181, 18);
		label[5]->SetCaption(D6_L("Persons"));

		label[6] = new label_c;
		label[6]->SetPosition(200, 110, 165, 18);
		label[6]->SetCaption(D6_L("Players"));

		label[7] = new label_c;
		label[7]->SetPosition(370, 110, 120, 18);
		label[7]->SetCaption(D6_L("Controller"));

		textbox = new textbox_c;
		textbox->SetPosition(200, 351, 19, 13, D6_ALL_CHR);
		textbox->SetNG(13, 1);

		// Switchbox - volba ovladani
		for (Size i = 0; i < D6_MAX_PLAYERS; i++)
		{
			controlSwitch[i] = new switchbox_c;
			controlSwitch[i]->SetPosition(370, 131 + i * 18, 120, 0);
			controlSwitch[i]->SetNG(14 + i, 1);
		}

		joyRescan();

		backgroundCount = d6TextureManager.get(D6_TEXTURE_BCG_KEY).size();
		levelList.initialize(D6_FILE_LEVEL, D6_LEVEL_EXTENSION);

		listbox[3]->AddItem(D6_L("Random"));
		for (Size i = 0; i < levelList.getLength(); i++)
		{
			listbox[3]->AddItem(levelList.getFileName(i));
		}

		listbox[4]->AddItem(D6_L("Random"));
		for (Size i = 0; i < backgroundCount; i++)
		{
			listbox[4]->AddItem(std::to_string(i + 1));
		}
		
		for (Int32 i = 5; i < 21; i++)
		{
			listbox[6]->AddItem(std::to_string(i));
		}
		listbox[6]->SetCur(8);

		for (const Person& person : persons.list())
		{
			listbox[1]->AddItem(person.getName());
		}

		playing = 0;
		for (Size i = 0; i < D6_MAX_PLAYERS; i++)
		{
			if (willPlay[i] != -1)
			{
				const Person& person = persons.get(willPlay[i]);
				listbox[2]->AddItem(person.getName());
				playing++;
			}
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
		file.write(willPlay, 4, 8);
		file.close();
	}

	void Menu::rebuildTable()
	{
		listbox[0]->Clear();
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
			listbox[0]->AddItem(personStat);
		}
	}

	void Menu::addPlayer()
	{
		int     i, c;

		c = listbox[1]->CurItem();
		if (c != -1 && playing < D6_MAX_PLAYERS)
		{
			for (i = 0; i < D6_MAX_PLAYERS; i++)
				if (willPlay[i] == c)
					return;
			willPlay[playing++] = c;
			listbox[2]->AddItem(persons.get(c).getName());
		}
	}

	void Menu::addPerson()
	{
		const std::string& personName = textbox->Text();

		if (!personName.empty())
		{
			persons.add(Person(personName));
			listbox[1]->AddItem(personName);
			rebuildTable();
			textbox->Flush();
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
		for (Size i = 0; i < playing; i++)
		{
			Person& person = persons.get(willPlay[i]);
			auto skinForName = playerColors.find(person.getName());
			const PlayerSkinColors& colors = skinForName != playerColors.end() ? skinForName->second : playerColors.at(Format("default_{0}") << i);
			const PlayerControls& controls = controlsManager.get(controlSwitch[i]->CurItem());
			playerDefinitions.push_back(Game::PlayerDefinition(person, colors, controls));
		}

		// Levels
		std::vector<std::string> levels;
		if (!listbox[3]->CurItem())
		{
			for (Size i = 0; i < levelList.getLength(); ++i)
			{
				levels.push_back(levelList.getPath(i));
			}
		}
		else
		{
			levels.push_back(levelList.getPath(listbox[3]->CurItem() - 1));
		}

		// Game backgrounds
		std::vector<Size> backgrounds;
		if (!listbox[4]->CurItem())
		{
			for (Size i = 0; i < backgroundCount; i++)
			{
				backgrounds.push_back(i);
			}
		}
		else
		{
			backgrounds.push_back(listbox[4]->CurItem() - 1);
		}

		// Screen
		ScreenMode screenMode = (playing > 4 || listbox[5]->CurItem() == 0) ? ScreenMode::FullScreen : ScreenMode::SplitScreen;
		Int32 screenZoom = listbox[6]->CurItem() + 5; 

		// Start
		Context::push(*game);
		game->start(playerDefinitions, levels, backgrounds, screenMode, screenZoom);
	}

	void Menu::removePlayer(Int32 c)
	{
		int     i;

		if (c != -1)
		{
			listbox[2]->DelItem(c);
			for (i = c; i < D6_MAX_PLAYERS - 1; i++)
			{
				willPlay[i] = willPlay[i + 1];
			}
			willPlay[i] = -1;
			playing--;
		}
	}

	void Menu::deletePerson()
	{
		if (!deleteQuestion())
			return;

		int c = listbox[1]->CurItem();
		if (c != -1)
		{
			listbox[1]->DelItem(c);
			for (Size i = 0; i < D6_MAX_PLAYERS; i++)
			{
				if (willPlay[i] == c)
				{
					removePlayer(i);
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
		Desk::EventType event;
		DeskControl* from;

		sync += elapsedTime;

		while (sync > wait)
		{
			sync -= wait;

			desk->Check(event, from);
			if (event == Desk::EventType::Released && from->getGroup() == 1)
			{
				switch (from->getNumber())
				{
				case 6:
					addPlayer();
					break;
				case 7:
					removePlayer(listbox[2]->CurItem());
					break;
				case 8:
					deletePerson();
					rebuildTable();
					break;
				case 9:
					addPerson();
					break;
				case 10:
					if (playing > 1)
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

		desk->Draw(font);

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
		desk->keyEvent(keyCode);

		if (keyCode == SDLK_RETURN)
		{
			addPerson();
		}

		if (keyCode == SDLK_F1 && playing > 1)
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
		desk->textInputEvent(text);
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