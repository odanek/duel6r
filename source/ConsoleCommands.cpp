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

#include "Sound.h"
#include "Util.h"
#include "Math.h"
#include "Menu.h"
#include "WorldRenderer.h"
#include "Game.h"
#include "ConsoleCommands.h"
#include "Weapon.h"
#include "EnumClassHash.h"

namespace Duel6 {
    void ConsoleCommands::maxRounds(Console &console, const Console::Arguments &args, GameSettings &gameSettings) {
        if (args.length() == 2) {
            gameSettings.setMaxRounds(std::stoi(args.get(1)));
        } else {
            console.printLine(Format("Max rounds: {0}") << gameSettings.getMaxRounds());
        }
    }

    void ConsoleCommands::ghostMode(Console &console, const Console::Arguments &args, GameSettings &gameSettings) {
        if (args.length() == 2 && (args.get(1) == "on" || args.get(1) == "off")) {
            gameSettings.setGhostEnabled(args.get(1) == "on");
        } else {
            console.printLine(Format("Ghost Mode [on/off]: {0}") << (gameSettings.isGhostEnabled() ? "on" : "off"));
        }
    }

    void
    ConsoleCommands::levelSelectionMode(Console &console, const Console::Arguments &args, GameSettings &gameSettings) {
        if (args.length() == 2 && (args.get(1) == "random" || args.get(1) == "shuffle")) {
            gameSettings.setLevelSelectionMode(
                    args.get(1) == "random" ? LevelSelectionMode::Random : LevelSelectionMode::Shuffle);
        } else {
            std::string mode(gameSettings.getLevelSelectionMode() == LevelSelectionMode::Random ? "random" : "shuffle");
            console.printLine(Format("Level selection mode [random/shuffle]: {0}") << mode);
        }
    }

    void ConsoleCommands::shotCollision(Console &console, const Console::Arguments &args, GameSettings &gameSettings) {
        static const std::unordered_map<std::string, ShotCollisionSetting> stringOptions = {
                {"none",  ShotCollisionSetting::None},
                {"large", ShotCollisionSetting::Large},
                {"all",   ShotCollisionSetting::All}
        };
        static const std::unordered_map<ShotCollisionSetting, std::string, EnumClassHash<ShotCollisionSetting>> collisionOptions = {
                {ShotCollisionSetting::None,  "none"},
                {ShotCollisionSetting::Large, "large"},
                {ShotCollisionSetting::All,   "all"}
        };

        if (args.length() == 2) {
            auto value = args.get(1);
            auto collision = stringOptions.find(value);
            if (collision != stringOptions.end()) {
                gameSettings.setShotCollision(collision->second);
                console.printLine(Format("Shot collision setting changed to: {0}") << value);
            } else {
                console.printLine("Allowed values: [none, large, all]");
            }
        } else {
            auto collision = collisionOptions.find(gameSettings.getShotCollision());
            console.printLine(Format("Shot collision: {0}") << collision->second);
        }
    }

    void ConsoleCommands::volume(Console &console, const Console::Arguments &args, Sound &sound) {
        if (args.length() == 2) {
            sound.volume(std::stoi(args.get(1)));
        }
    }

    void
    ConsoleCommands::toggleRenderMode(Console &console, const Console::Arguments &args, GameSettings &gameSettings) {
        gameSettings.setWireframe(!gameSettings.isWireframe());

        if (gameSettings.isWireframe()) {
            console.printLine("Rendering mode switched to wireframe");
        } else {
            console.printLine("Rendering mode switched to solid");
        }
    }

    void ConsoleCommands::toggleShowFps(Console &console, const Console::Arguments &args, GameSettings &gameSettings) {
        gameSettings.setShowFps(!gameSettings.isShowFps());

        if (gameSettings.isShowFps()) {
            console.printLine("Fps counter shown");
        } else {
            console.printLine("Fps counter hidden");
        }
    }

    void ConsoleCommands::musicOnOff(Console &console, const Console::Arguments &args, Menu &menu) {
        if (args.length() == 2) {
            if (args.get(1) == "on" || args.get(1) == "off") {
                menu.enableMusic(args.get(1) == "on");
            }
        }
    }

    void ConsoleCommands::joyScan(Console &console, const Console::Arguments &args, Menu &menu) {
        if (menu.isCurrent()) {
            menu.joyRescan();
        }
    }

    void ConsoleCommands::loadSkin(Console &console, const Console::Arguments &args, Menu &menu) {
        auto &profileMap = menu.getPersonProfiles();
        std::string bodyParts[] = {"Hair top", "Hair bottom", "Body outer", "Body inner", "Arm outer", "Arm inner",
                                   "Trousers", "Shoes", "Skin"};
        std::string name;

        if (args.length() == 11) {
            name = args.get(1);
            auto profile = profileMap.find(name);
            if (profile == profileMap.end()) {
                console.printLine(Format("No profile found for name: {0}") << name);
                return;
            }
            PlayerSkinColors &colors = profile->second.getSkinColors();

            for (Size i = 0; i < 9; i++) {
                colors.set((PlayerSkinColors::BodyPart) i, Color::fromString(args.get(i + 2)));
            }

            console.printLine(Format("Skin \"{0}\": OK") << name);
        } else if (args.length() == 2) {
            name = args.get(1);
            auto profile = profileMap.find(name);
            if (profile != profileMap.end()) {
                PlayerSkinColors &colors = profile->second.getSkinColors();
                console.print(Format("Skin \"{0}\":\n") << name);
                for (Size i = 0; i < 9; i++) {
                    const Color &color = colors.get((PlayerSkinColors::BodyPart) i);
                    console.printLine(
                            Format("   {0}: ({1}, {2}, {3})") << bodyParts[i] << color.getRed() << color.getGreen()
                                                              << color.getBlue());
                }
            } else {
                console.printLine(Format("No profile found for name: {0}") << name);
            }
        } else if (args.length() == 1) {
            console.printLine(Format("Available skins ({0}):") << profileMap.size());
            for (auto &profile : profileMap) {
                console.printLine(Format("   \"{0}\"") << profile.first);
            }
        }
    }

    void ConsoleCommands::enableWeapon(Console &console, const Console::Arguments &args, GameSettings &gameSettings) {
        if (args.length() == 3) {
            Int32 weaponIndex = std::stoi(args.get(1));

            if (weaponIndex >= 0 && weaponIndex < (Int32) Weapon::values().size()) {
                const Weapon &weapon = Weapon::values()[weaponIndex];
                bool enable = (args.get(2) == "true");
                gameSettings.enableWeapon(weapon, enable);
                console.printLine(Format("\t{0}. {1} {2}") << weaponIndex << weapon.getName()
                                                           << (enable ? "enabled" : "disabled"));
            }
        } else {
            Size index = 0;
            for (auto &weapon : Weapon::values()) {
                bool enabled = gameSettings.isWeaponEnabled(weapon);
                console.printLine(Format("\t{0,2}. {1,-13} {2}") << index << weapon.getName()
                                                                 << (enabled ? "enabled" : "disabled"));
                index++;
            }
        }
    }

    void ConsoleCommands::ammoRange(Console &console, const Console::Arguments &args, GameSettings &gameSettings) {
        if (args.length() == 3) {
            int min = std::stoi(args.get(1)), max = std::stoi(args.get(2));

            if (min <= max && min >= 0) {
                gameSettings.setAmmoRange(std::make_pair(min, max));
            }
        } else if (args.length() == 1) {
            const std::pair<Int32, Int32> &range = gameSettings.getAmmoRange();
            console.printLine(Format("\tmin = {0}\n\tmax = {1}") << range.first << range.second);
        } else {
            console.printLine(Format("{0}: {0} [min max]") << args.get(0));
        }
    }

    void ConsoleCommands::openGLInfo(Console &console, const Console::Arguments &args) {
        Renderer::Info info = globRenderer->getInfo();

        console.printLine("\n===OpenGL info===");
        console.printLine(Format("Vendor     : {0}") << info.vendor);
        console.printLine(Format("Renderer   : {0}") << info.renderer);
        console.printLine(Format("Version    : {0}") << info.version);
        console.printLine("Extensions :");

        if (info.extensions.empty()) {
            console.printLine("...No supported extensions");
        } else {
            for (const auto &name : info.extensions) {
                console.printLine(Format("...{0}") << name);
            }
        }

        console.printLine("");
    }

    void ConsoleCommands::vsync(Console &console, const Console::Arguments &args) {
        if (args.length() == 1) {
            bool enabled = (SDL_GL_GetSwapInterval() == 1);
            console.printLine(Format("Vertical synchronization: {0}") << (enabled ? "on" : "off"));
        } else if (args.length() == 2) {
            bool enable = (args.get(1) == "on");
            SDL_GL_SetSwapInterval(enable ? 1 : 0);
        }
    }

    void ConsoleCommands::registerCommands(Console &console, AppService &appService, Menu &menu,
                                           GameSettings &gameSettings) {
        // Set some console functions
        console.setLast(15);
        console.registerCommand("switch_render_mode", [&gameSettings](Console &con, const Console::Arguments &args) {
            toggleRenderMode(con, args, gameSettings);
        });
        console.registerCommand("show_fps", [&gameSettings](Console &con, const Console::Arguments &args) {
            toggleShowFps(con, args, gameSettings);
        });
        console.registerCommand("gl_info", openGLInfo);
        console.registerCommand("vsync", vsync);
        console.registerCommand("volume", [&appService](Console &con, const Console::Arguments &args) {
            volume(con, args, appService.getSound());
        });
        console.registerCommand("rounds", [&gameSettings](Console &con, const Console::Arguments &args) {
            maxRounds(con, args, gameSettings);
        });
        console.registerCommand("ghosts", [&gameSettings](Console &con, const Console::Arguments &args) {
            ghostMode(con, args, gameSettings);
        });
        console.registerCommand("level_selection_mode", [&gameSettings](Console &con, const Console::Arguments &args) {
            levelSelectionMode(con, args, gameSettings);
        });
        console.registerCommand("shot_collision", [&gameSettings](Console &con, const Console::Arguments &args) {
            shotCollision(con, args, gameSettings);
        });
        console.registerCommand("music", [&menu](Console &con, const Console::Arguments &args) {
            musicOnOff(con, args, menu);
        });
        console.registerCommand("joy_scan", [&menu](Console &con, const Console::Arguments &args) {
            joyScan(con, args, menu);
        });
        console.registerCommand("skin", [&menu](Console &con, const Console::Arguments &args) {
            loadSkin(con, args, menu);
        });
        console.registerCommand("gun", [&gameSettings](Console &con, const Console::Arguments &args) {
            enableWeapon(con, args, gameSettings);
        });
        console.registerCommand("start_ammo_range", [&gameSettings](Console &con, const Console::Arguments &args) {
            ammoRange(con, args, gameSettings);
        });
    }
}