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
#include <SDL2/SDL_opengl.h>
#include "Type.h"
#include "Context.h"
#include "LevelList.h"
#include "PersonList.h"
#include "PersonProfile.h"
#include "Gui.h"
#include "PlayerControls.h"
#include "PlayerSkinColors.h"
#include "Video.h"
#include "AppService.h"
#include "Defines.h"

namespace Duel6
{
	class Game; // Forward, TODO: Remove

	class Menu
		: public Context
	{
	private:
		AppService& appService;
		Font& font;
		Video& video;
		Sound& sound;
		Game* game;
		Gui::Desktop gui;
		PlayerControlsManager controlsManager;
		std::unordered_map<std::string, PersonProfile> personProfiles;
		LevelList levelList;
		PersonList persons;
		Gui::Button* button[7];
		Gui::Listbox* listbox[7];
		Gui::Label* label[8];
		Gui::Combobox* controlSwitch[D6_MAX_PLAYERS];
		Gui::Textbox* textbox;
		Size backgroundCount;
		TextureManager::Texture menuBannerTexture;
		Sound::Track menuTrack;
		bool playMusic;

	public:
		Menu(AppService& appService);

		~Menu()
		{
		}

		void setGameReference(Game* game)
		{
			this->game = game;
		}

		void initialize();		

		void joyRescan();

		void savePersonData();

		void keyEvent(SDL_Keycode keyCode, Uint16 keyModifiers) override;
		void textInputEvent(const char* text) override;
		void update(Float32 elapsedTime) override;
		void render() const override;

		void enableMusic(bool enable);

		std::unordered_map<std::string, PersonProfile>& getPersonProfiles()
		{
			return personProfiles;
		}

		void loadPersonProfiles(const std::string& path);

		void loadPersonData(const std::string& filePath);

	private:
		void beforeStart(Context* prevContext) override;
		void beforeClose(Context* nextContext) override;

		void showMessage(const std::string& message);
		void detectControls(Size playerIndex);
		void play();

		PersonProfile& getPersonProfile(const std::string& name, Size index);
		void cleanPersonData();
		void addPerson();
		void deletePerson();
		void addPlayer(Int32 c);
		void removePlayer(Int32 c);
		void rebuildTable();
		bool question(const std::string& question);
		bool deleteQuestion();
	};
}

#endif