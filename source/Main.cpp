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

#include "Globals.h"
#include "InfoMessageQueue.h"
#include "Game.h"
#include "Menu.h"
#include "World.h"
#include "Setup.h"
#include "Main.h"

namespace Duel6
{
	Game d6Game;
	Menu d6Menu;
	TextureManager d6TextureManager;
	Float32 d6Cos[450];
	bool d6Wireframe = false;
	bool d6ShowFps = false;
	std::vector<PlayerSkinColors> d6PlayerColors(D6_MAX_PLAYERS);
	int d6AmmoRangeMin = 15, d6AmmoRangeMax = 15;
	bool d6ShowRanking = true;
	InfoMessageQueue d6MessageQueue(D6_INFO_DURATION);
	SpriteList d6SpriteList;
	World d6World(D6_FILE_ANM, D6_ANM_SPEED, D6_WAVE_HEIGHT);

	void Main::textInputEvent(Context& context, const char* text)
	{
		if (g_app.con->isactive())
		{
			g_app.con->textInputEvent(text);
		}
		else
		{
			context.textInputEvent(text);
		}
	}

	void Main::keyEvent(Context& context, SDL_Keycode keyCode, Uint16 keyModifiers)
	{
		if (keyCode == SDLK_BACKQUOTE)
		{
			g_app.con->toggle();
			if (g_app.con->isactive())
			{
				SDL_StartTextInput();
			}
			else if (context.is(d6Game))
			{
				SDL_StopTextInput();
			}
		}

		if (g_app.con->isactive())
		{
			g_app.con->keyEvent(keyCode, keyModifiers);
		}
		else
		{
			context.keyEvent(keyCode, keyModifiers);
		}
	}

	void Main::processEvents(Context& context)
	{
		SDL_Event   event;
		SDL_Keysym  key;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				key = event.key.keysym;
				g_inp.pressedKeys.insert(key.sym);
				keyEvent(context, key.sym, key.mod);
				break;
			case SDL_KEYUP:
				key = event.key.keysym;
				g_inp.pressedKeys.erase(key.sym);
				break;
			case SDL_TEXTINPUT:
				textInputEvent(context, event.text.text);
				break;
			case SDL_QUIT:
				g_app.flags |= APP_FLAG_QUIT;
				break;
			}
		}
	}

	void Main::syncUpdateAndRender(Context& context)
	{
		static unsigned long cur_time = 0, last_fps_time = 0;
		static int frame_counter = 0;
		unsigned long last_time;

		last_time = cur_time;
		cur_time = SDL_GetTicks();

		// Calculate fps
		if (cur_time - last_fps_time >= 1000)
		{
			g_app.fps = frame_counter * 1000 / float(cur_time - last_fps_time);
			last_fps_time = cur_time;
			frame_counter = 0;
		}
		frame_counter++;

		// Draw
		context.render();
		VID_SwapBuffers();

		// Update
		if (cur_time - last_time < 70)
		{
			float elapsedTime = (cur_time - last_time) * 0.001f;;
			context.update(elapsedTime);
		}
	}

	void Main::run()
	{
		while (!(g_app.flags & APP_FLAG_QUIT))
		{
			Context& context = Context::getCurrent();
			processEvents(context);
			syncUpdateAndRender(context);
		}
	}
}

/*
==================================================
Ulozeni obsahu konzoly na disk a uklid po chybe
==================================================
*/
static void MAIN_ErrorHandler(const char *str)
{
    if (g_app.con.get() == nullptr)
    {
        g_app.con->printf(str);
        g_app.con->exec("dump chyba.con");
    }

    Duel6::SET_DeInit ();
    CO_DeInit ();
}

/*
==================================================
Vstupni bod aplikace
==================================================
*/
int main(int argc, char *argv[])
{
    CO_Init(MAIN_ErrorHandler);
    Duel6::SET_Init ();

	for (int i = 1; i < argc; i++)
	{
		g_app.con->exec(argv[i]);
	}

	Duel6::Main app;
	app.run();

    Duel6::SET_DeInit();
    CO_DeInit();

    return 0;
}