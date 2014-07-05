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

#include <time.h>
#include "VideoException.h"
#include "Globals.h"
#include "InfoMessageQueue.h"
#include "Game.h"
#include "Menu.h"
#include "World.h"
#include "ConsoleCommands.h"
#include "Sound.h"
#include "Video.h"
#include "Font.h"
#include "Weapon.h"
#include "ElevatorList.h"
#include "Fire.h"
#include "Math.h"
#include "Application.h"

namespace Duel6
{
	Console d6Console(Console::ExpandFlag);
	TextureManager d6TextureManager(D6_TEXTURE_EXTENSION);
	Float32 d6Cos[450];
	InfoMessageQueue d6MessageQueue(D6_INFO_DURATION);
	SpriteList d6SpriteList;

	void Application::textInputEvent(Context& context, const char* text)
	{
		if (d6Console.isActive())
		{
			d6Console.textInputEvent(text);
		}
		else
		{
			context.textInputEvent(text);
		}
	}

	void Application::keyEvent(Context& context, SDL_Keycode keyCode, Uint16 keyModifiers)
	{
		if (keyCode == SDLK_BACKQUOTE)
		{
			d6Console.toggle();
			if (d6Console.isActive())
			{
				SDL_StartTextInput();
			}
			else if (context.is(game))
			{
				SDL_StopTextInput();
			}
		}

		if (d6Console.isActive())
		{
			d6Console.keyEvent(keyCode, keyModifiers);
		}
		else
		{
			context.keyEvent(keyCode, keyModifiers);
		}
	}

	void Application::processEvents(Context& context)
	{
		SDL_Event event;
		SDL_Keysym key;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				key = event.key.keysym;
				input.setPressed(key.sym, true);
				keyEvent(context, key.sym, key.mod);
				break;
			case SDL_KEYUP:
				key = event.key.keysym;
				input.setPressed(key.sym, false);
				break;
			case SDL_TEXTINPUT:
				textInputEvent(context, event.text.text);
				break;
			case SDL_QUIT:
				requestClose = true;
				break;
			}
		}
	}

	void Application::syncUpdateAndRender(Context& context)
	{
		static Uint32 curTime = 0;
		Uint32 lastTime = 0;

		lastTime = curTime;
		curTime = SDL_GetTicks();

		// Draw
		context.render();
		video.screenUpdate(d6Console, font);

		// Update
		if (curTime - lastTime < 70)
		{
			float elapsedTime = (curTime - lastTime) * 0.001f;;
			context.update(elapsedTime);
		}
	}

	void Application::setup(Int32 argc, char** argv)
	{
		srand((unsigned)time(nullptr));

		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			D6_THROW(VideoException, std::string("Unable to set graphics mode: ") + SDL_GetError());
		}

		font.load(APP_FILE_FONT);		
		menu.setGameReference(&game);
		game.setMenuReference(&menu);

		Console::registerBasicCommands(d6Console);
		ConsoleCommands::registerCommands(d6Console, menu, game);

		Math::initialize(D6_FILE_COS);
		Sound::initialize(20);

		// Read config file
		d6Console.print("\n===Config===\n");
		d6Console.exec("exec data/config.txt");
		// Init player skins
		d6Console.print("\n====Skin====\n");
		d6Console.exec(std::string("exec ") + D6_FILE_SKIN);

		for (int i = 1; i < argc; i++)
		{
			d6Console.exec(argv[i]);
		}

		video.initialize(APP_NAME, APP_FILE_ICON, d6Console);

		d6TextureManager.load(D6_TEXTURE_BCG_KEY, D6_TEXTURE_BCG_PATH, GL_LINEAR, true);
		d6TextureManager.load(D6_TEXTURE_EXPL_KEY, D6_TEXTURE_EXPL_PATH, GL_NEAREST, true);
		d6TextureManager.load(D6_TEXTURE_MENU_KEY, D6_TEXTURE_MENU_PATH, GL_LINEAR, true);
		d6TextureManager.load(D6_TEXTURE_WPN_KEY, D6_TEXTURE_WPN_PATH, GL_LINEAR, true);
		d6TextureManager.load(D6_TEXTURE_BLOCK_KEY, D6_TEXTURE_BLOCK_PATH, GL_LINEAR, true);

		WPN_Init();
		ELEV_Init();
		FIRE_Init();

		menu.initialize();
	}

	void Application::run()
	{
		Context::push(menu);

		while (!requestClose)
		{
			Context& context = Context::getCurrent();
			processEvents(context);
			syncUpdateAndRender(context);
		}

		Context::pop();
	}

	void Application::tearDown()
	{
		Sound::deInit();
		d6TextureManager.freeAll();
		SDL_Quit();
	}
}