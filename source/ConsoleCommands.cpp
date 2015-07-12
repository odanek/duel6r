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


#include <SDL2/SDL_mixer.h>
#include "Sound.h"
#include "Util.h"
#include "Math.h"
#include "Menu.h"
#include "Render.h"
#include "Game.h"
#include "ConsoleCommands.h"
#include "Weapon.h"

namespace Duel6
{
	/*
	==================================================
	Set language - console command
	==================================================
	*/
	void ConsoleCommands::language(Console& console, const Console::Arguments& args)
	{
		/*
		if (args.length() == 2)
		{
			if (args.get(1) == "lang/czech.lang")
			{
				MY_LangFree();
			}
			MY_LangLoad(args.get(1).c_str());
		}
		else
		{
			console.print(Format("{0}: {1}\n") << D6_L("Language") << D6_L("english"));
		}
		*/
	}

	void ConsoleCommands::maxRounds(Console& console, const Console::Arguments& args, GameSettings& gameSettings)
	{
		if (args.length() == 2)
		{
			gameSettings.setMaxRounds(std::stoi(args.get(1)));
		}
		else
		{
			console.printLine(Format(D6_L("Max rounds: {1}")) << gameSettings.getMaxRounds());
		}
	}

	void ConsoleCommands::ghostMode(Console& console, const Console::Arguments& args, GameSettings& gameSettings)
	{
		if (args.length() == 2 && (args.get(1) == "on" || args.get(1) == "off"))
		{
			gameSettings.setGhostEnabled(args.get(1) == "on");
		}
		else
		{
			console.printLine(Format(D6_L("Ghost Mode [on/off]: {0}")) << (gameSettings.getGhostEnabled() ? "on" : "off"));
		}
	}

	void ConsoleCommands::volume(Console& console, const Console::Arguments& args, Sound& sound)
	{
		if (args.length() == 2)
		{
			sound.volume(std::stoi(args.get(1)));
		}
	}

	void ConsoleCommands::toggleRenderMode(Console& console, const Console::Arguments& args, GameSettings& gameSettings)
	{
		gameSettings.setWireframe(!gameSettings.isWireframe());

		if (gameSettings.isWireframe())
		{
			console.printLine(D6_L("Rendering mode switched to wireframe"));
		}
		else
		{
			console.printLine(D6_L("Rendering mode switched to solid"));
		}
	}

	void ConsoleCommands::toggleShowFps(Console& console, const Console::Arguments& args, GameSettings& gameSettings)
	{
		gameSettings.setShowFps(!gameSettings.isShowFps());
		
		if (gameSettings.isShowFps())
		{
			console.printLine(D6_L("Fps counter shown"));
		}
		else
		{
			console.printLine(D6_L("Fps counter hidden"));
		}
	}

	void ConsoleCommands::musicOnOff(Console& console, const Console::Arguments& args, Menu& menu)
	{
		if (args.length() == 2)
		{
			if (args.get(1) == "on" || args.get(1) == "off")
			{
				menu.enableMusic(args.get(1) == "on");
			}
		}
	}

	void ConsoleCommands::joyScan(Console& console, const Console::Arguments& args, Menu& menu)
	{
		if (menu.isCurrent())
		{
			menu.joyRescan();
		}
	}

	void ConsoleCommands::loadSkin(Console& console, const Console::Arguments& args, Menu& menu)
	{
		auto& profileMap = menu.getPersonProfiles();
		std::string bodyParts[] = {"Hair top", "Hair bottom", "Body outer", "Body inner", "Arm outer", "Arm inner", "Trousers", "Shoes", "Skin"};
		std::string name;

		if (args.length() == 11)
		{
			name = args.get(1);
			auto profile = profileMap.find(name);
			if (profile == profileMap.end())
			{
				console.printLine(Format("No profile found for name: {0}") << name);
				return;
			}
			PlayerSkinColors& colors = profile->second.getSkinColors();

			for (Size i = 0; i < 9; i++)
			{
				colors.set((PlayerSkinColors::BodyPart)i, Color::fromString(args.get(i + 2)));
			}

			console.printLine(Format(D6_L("Skin \"{0}\": OK")) << name);
		}
		else if (args.length() == 2)
		{
			name = args.get(1);
			auto profile = profileMap.find(name);
			if (profile != profileMap.end())
			{
				PlayerSkinColors& colors = profile->second.getSkinColors();
				console.print(Format(D6_L("Skin \"{0}\":\n")) << name);
				for (Size i = 0; i < 9; i++)
				{
					const Color& color = colors.get((PlayerSkinColors::BodyPart)i);
					console.printLine(Format("   {0}: ({1}, {2}, {3})") << bodyParts[i] << color.getRed() << color.getGreen() << color.getBlue());
				}
			}
			else
			{
				console.printLine(Format("No profile found for name: {0}") << name);
			}
		}
		else if (args.length() == 1)
		{
			console.printLine(Format("Available skins ({0}):") << profileMap.size());
			for (auto& profile : profileMap)
			{
				console.printLine(Format("   \"{0}\"") << profile.first);
			}
		}
	}

	void ConsoleCommands::enableWeapon(Console& console, const Console::Arguments& args)
	{
		if (args.length() == 3)
		{
			Int32 gn = std::stoi(args.get(1));
			if (gn >= 0 && gn < D6_WEAPONS)
			{
				d6WpnDef[gn].enabled = (args.get(2) == "true");
				std::string enabled = d6WpnDef[gn].enabled ? D6_L("enabled") : D6_L("disabled");
				console.printLine(Format("\t{0}. {1} {2}") << gn << D6_L(d6WpnDef[gn].name) << enabled);
			}
		}
		else
		{
			for (Int32 gn = 0; gn < D6_WEAPONS; gn++)
			{
				std::string enabled = d6WpnDef[gn].enabled ? D6_L("enabled") : D6_L("disabled");
				console.printLine(Format("\t{0,2}. {1,-13} {2}") << gn << D6_L(d6WpnDef[gn].name) << enabled);
			}
		}
	}

	void ConsoleCommands::ammoRange(Console& console, const Console::Arguments& args, GameSettings& gameSettings)
	{
		if (args.length() == 3)
		{
			int min = std::stoi(args.get(1)), max = std::stoi(args.get(2));

			if (min <= max && min >= 0)
			{
				gameSettings.setAmmoRange(std::make_pair(min, max));
			}
		}
		else if (args.length() == 1)
		{
			const std::pair<Int32, Int32>& range = gameSettings.getAmmoRange();
			console.printLine(Format(D6_L("\tmin = {0}\n\tmax = {1}")) << range.first << range.second);
		}
		else
		{
			console.printLine(Format(D6_L("{0}: {0} [min max]")) << args.get(0));
		}
	}

	void ConsoleCommands::openGLInfo(Console& console, const Console::Arguments& args)
	{
		console.printLine(D6_L("\n===OpenGL info==="));
		console.printLine(Format(D6_L("Vendor     : {0}")) << (const char *)glGetString(GL_VENDOR));
		console.printLine(Format(D6_L("Renderer   : {0}")) << (const char *)glGetString(GL_RENDERER));
		console.printLine(Format(D6_L("Version    : {0}")) << (const char *)glGetString(GL_VERSION));
		console.printLine(D6_L("Extensions :"));

		const char* estr = (const char *)glGetString(GL_EXTENSIONS);

		if (estr == nullptr || strlen(estr) < 2)
		{
			console.printLine(D6_L("...No supported extensions"));
		}
		else
		{
			while (*estr)
			{
				while (*estr == ' ')
				{
					estr++;
				}

				if (*estr == 0)
				{
					break;
				}

				std::string extensionName;
				while (*estr != ' ' && *estr != 0)
				{
					extensionName += *estr;
					++estr;
				}

				console.printLine(Format("...{0}") << extensionName);
			}
		}

		console.printLine("");
	}

	void ConsoleCommands::registerCommands(Console& console, AppService& appService, Menu& menu, GameSettings& gameSettings)
	{
		SDL_version sdlVersion;
		std::string verStr = D6_L("version");

		// Print application info
		console.printLine(D6_L("\n===Application information==="));
		console.printLine(Format("{0} {1}: {2}") << APP_NAME << verStr << APP_VERSION);
		SDL_GetVersion(&sdlVersion);
		console.printLine(Format("SDL {0}: {1}.{2}.{3}") << verStr << sdlVersion.major << sdlVersion.minor << sdlVersion.patch);
		const SDL_version* mixVersion = Mix_Linked_Version();
		console.printLine(Format("SDL_mixer {0}: {1}.{2}.{3}") << verStr << mixVersion->major << mixVersion->minor << mixVersion->patch);
		console.printLine(D6_L("Language: english"));

		// Set some console functions
		console.setLast(15);
		console.registerCommand("switch_render_mode", [&gameSettings](Console& con, const Console::Arguments& args) {
			toggleRenderMode(con, args, gameSettings);
		});
		console.registerCommand("show_fps", [&gameSettings](Console& con, const Console::Arguments& args) {
			toggleShowFps(con, args, gameSettings);
		});
		console.registerCommand("gl_info", openGLInfo);
		console.registerCommand("lang", language);
		console.registerCommand("volume", [&appService](Console& con, const Console::Arguments& args) {
			volume(con, args, appService.getSound());
		});
		console.registerCommand("rounds", [&gameSettings](Console& con, const Console::Arguments& args) {
			maxRounds(con, args, gameSettings);
		});
		console.registerCommand("ghosts", [&gameSettings](Console& con, const Console::Arguments& args) {
			ghostMode(con, args, gameSettings);
		});
		console.registerCommand("music", [&menu](Console& con, const Console::Arguments& args) {
			musicOnOff(con, args, menu);
		});
		console.registerCommand("joy_scan", [&menu](Console& con, const Console::Arguments& args) {
			joyScan(con, args, menu);
		});
		console.registerCommand("skin", [&menu](Console& con, const Console::Arguments& args) {
			loadSkin(con, args, menu);
		});
		console.registerCommand("gun", enableWeapon);
		console.registerCommand("start_ammo_range", [&gameSettings](Console& con, const Console::Arguments& args) {
			ammoRange(con, args, gameSettings);
		});
	}
}