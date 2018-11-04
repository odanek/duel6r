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

#include "Lua.h"
#include "../../Player.h"
#include "../../Level.h"
#include "../ScriptException.h"
#include "../../console/console.h"
#include "../ScriptContext.h"
#include "../PersonScriptContext.h"
#include "../../ShotList.h"

namespace Duel6::Script {
    namespace {
        int consolePrint(lua_State *state);
        int playerMetaIndex(lua_State *state);
        int playerPressButton(lua_State *state);
        int levelBlockAt(lua_State *state);
        int levelMetaIndex(lua_State *state);

        void pushPlayerButtonCallback(lua_State *state, Player &player, Uint32 button, const char *name) {
            lua_pushstring(state, name);
            lua_pushlightuserdata(state, &player);
            lua_pushinteger(state, button);
            lua_pushcclosure(state, playerPressButton, 2);
            lua_rawset(state, -3);
        }
    }

    template<>
    void Lua::pushValue(lua_State *state, const bool &value) {
        lua_pushboolean(state, value);
    }

    template<>
    void Lua::pushValue(lua_State *state, const std::string &value) {
        lua_pushstring(state, value.c_str());
    }

    template<>
    void Lua::pushValue(lua_State *state, const Int32 &value) {
        lua_pushinteger(state, value);
    }

    template<>
    void Lua::pushValue(lua_State *state, const Float32 &value) {
        lua_pushnumber(state, value);
    }

    template<>
    void Lua::pushValue(lua_State *state, const Float64 &value) {
        lua_pushnumber(state, value);
    }

    template<>
    void Lua::pushValue(lua_State *state, const Vector &value) {
        lua_newtable(state);
        Lua::pushProperty(state, "x", value.x);
        Lua::pushProperty(state, "y", value.y);
        Lua::pushProperty(state, "z", value.z);
    }

    template<>
    void Lua::pushValue(lua_State *state, const Weapon &value) {
        lua_newtable(state);
        Lua::pushProperty(state, "name", value.getName());
        Lua::pushProperty(state, "reloadInterval", value.getReloadInterval());
        Lua::pushProperty(state, "chargeable", value.isChargeable());
    }

    template<>
    void Lua::pushValue(lua_State *state, Player &value) {
        lua_newtable(state);

        Lua::pushMetaIndex(state, playerMetaIndex, value);
        Lua::pushProperty(state, "name", value.getPerson().getName());

        pushPlayerButtonCallback(state, value, Player::ButtonLeft, "pressLeft");
        pushPlayerButtonCallback(state, value, Player::ButtonRight, "pressRight");
        pushPlayerButtonCallback(state, value, Player::ButtonUp, "pressUp");
        pushPlayerButtonCallback(state, value, Player::ButtonDown, "pressDown");
        pushPlayerButtonCallback(state, value, Player::ButtonShoot, "pressShoot");
        pushPlayerButtonCallback(state, value, Player::ButtonPick, "pressPick");
        pushPlayerButtonCallback(state, value, Player::ButtonStatus, "pressStatus");
    }

    template<>
    void Lua::pushValue(lua_State *state, Console &value) {
        lua_newtable(state);
        lua_pushliteral(state, "print");
        lua_pushlightuserdata(state, &value);
        lua_pushcclosure(state, consolePrint, 1);
        lua_rawset(state, -3);
    }

    template<>
    void Lua::pushValue(lua_State *state, Level &value) {
        lua_newtable(state);

        Lua::pushMetaIndex(state, levelMetaIndex, value);
        Lua::pushProperty(state, "width", value.getWidth());
        Lua::pushProperty(state, "height", value.getHeight());

        lua_pushliteral(state, "blockAt");
        lua_pushlightuserdata(state, &value);
        lua_pushcclosure(state, levelBlockAt, 1);
        lua_rawset(state, -3);
    }

    template<>
    void Lua::pushValue(lua_State *state, ScriptContext &value) {
        lua_newtable(state);
        Lua::pushProperty(state, "console", value.getConsole());
    }

    template<>
    void Lua::pushValue(lua_State *state, PersonScriptContext &value) {
        lua_newtable(state);
        Lua::pushProperty(state, "name", value.getProfileName());
        Lua::pushProperty(state, "path", value.getProfileRoot());
    }

    template<>
    void Lua::pushValue(lua_State *state, Shot &value) {
        lua_newtable(state);
        Lua::pushProperty(state, "centre", value.getCentre());
        Lua::pushProperty(state, "dimensions", value.getDimensions());
        Lua::pushProperty(state, "player", value.getPlayer().getPerson().getName());
        Lua::pushProperty(state, "weapon", value.getWeapon());
        Lua::pushProperty(state, "velocity", value.getVelocity());
        Lua::pushProperty(state, "powerful", value.isPowerful());
    }

    template<>
    void Lua::pushValue(lua_State *state, ShotList &value) {
        lua_newtable(state);

        Int32 index = 1;
        value.forEach([&index, &state] (Shot &shot) -> bool {
            lua_pushinteger(state, index);
            Lua::pushValue(state, shot);
            lua_rawset(state, -3);
            ++index;
            return true;
        });
    }

    void Lua::invoke(lua_State *state, Int32 nargs, Int32 nresults) {
        int result = lua_pcall(state, nargs, nresults, 0);
        if (result != LUA_OK) {
            std::string message = Format("Script error: {0}") << lua_tostring(state, -1);
            D6_THROW(ScriptException, message);
        }
    }

    namespace {
        int consolePrint(lua_State *state) {
            auto &console = *((Console *) lua_touserdata(state, lua_upvalueindex(1)));
            const char *str = luaL_checkstring(state, 1);
            console.printLine(str);
            return 0;
        }

        int playerMetaIndex(lua_State *state) {
            auto &player = *((Player *) lua_touserdata(state, lua_upvalueindex(1)));
            const char *propertyName = luaL_checkstring(state, 2);

            if (!strcmp(propertyName, "centre")) {
                Lua::pushValue(state, player.getCentre());
                return 1;
            } else if (!strcmp(propertyName, "dimensions")) {
                Lua::pushValue(state, player.getDimensions());
                return 1;
            } else if (!strcmp(propertyName, "life")) {
                Lua::pushValue(state, player.getLife() / D6_MAX_LIFE);
                return 1;
            } else if (!strcmp(propertyName, "air")) {
                Lua::pushValue(state, player.getAir() / D6_MAX_AIR);
                return 1;
            } else if (!strcmp(propertyName, "ammo")) {
                Lua::pushValue(state, player.getAmmo());
                return 1;
            } else if (!strcmp(propertyName, "roundKills")) {
                Lua::pushValue(state, player.getRoundKills());
                return 1;
            } else if (!strcmp(propertyName, "bonus")) {
                auto bonus = player.getBonus();
                if (bonus == BonusType::NONE) {
                    lua_pushnil(state);
                } else {
                    lua_newtable(state);
                    Lua::pushProperty(state, "name", bonus.getName());
                    Lua::pushProperty(state, "remainingTime", player.getBonusRemainingTime());
                }
                return 1;
            } else if (!strcmp(propertyName, "weapon")) {
                Lua::pushValue(state, player.getWeapon());
                return 1;
            } else if (!strcmp(propertyName, "alive")) {
                Lua::pushValue(state, player.isAlive());
                return 1;
            } else if (!strcmp(propertyName, "velocity")) {
                Lua::pushValue(state, player.getVelocity());
                return 1;
            } else if (!strcmp(propertyName, "reloadInterval")) {
                Lua::pushValue(state, player.getReloadInterval());
                return 1;
            } else if (!strcmp(propertyName, "reloadTime")) {
                Lua::pushValue(state, player.getReloadTime());
                return 1;
            }

            return 0;
        }

        int playerPressButton(lua_State *state) {
            auto &player = *((Player *) lua_touserdata(state, lua_upvalueindex(1)));
            auto button = (Uint32) lua_tointeger(state, lua_upvalueindex(2));
            player.pressButton(button);
            return 0;
        }

        int levelBlockAt(lua_State *state) {
            auto &level = *((Level *) lua_touserdata(state, lua_upvalueindex(1)));
            auto x = (Int32) luaL_checknumber(state, 1);
            auto y = (Int32) luaL_checknumber(state, 2);

            if (!level.isInside(x, y)) {
                lua_pushnil(state);
                return 1;
            }

            const Block &block = level.getBlockMeta(x, y);

            lua_newtable(state);
            Lua::pushProperty(state, "wall", block.is(Block::Type::Wall));
            Lua::pushProperty(state, "water", block.is(Block::Type::Water));
            Lua::pushProperty(state, "waterfall", block.is(Block::Type::Waterfall));
            Lua::pushProperty(state, "waterType", block.getWaterType().getName());

            return 1;
        }

        int levelMetaIndex(lua_State *state) {
            auto &level = *((Level *) lua_touserdata(state, lua_upvalueindex(1)));
            const char *propertyName = luaL_checkstring(state, 2);

            if (!strcmp(propertyName, "waterLevel")) {
                Lua::pushValue(state, level.getWaterLevel());
                return 1;
            } else if (!strcmp(propertyName, "raisingWater")) {
                Lua::pushValue(state, level.isRaisingWater());
                return 1;
            }

            return 0;
        }
    }
}
