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
#include "../../World.h"
#include "../ScriptException.h"
#include "../../console/console.h"

namespace Duel6::Script {
    namespace {
        int consolePrint(lua_State *L) {
            const char *str = luaL_checkstring(L, 1);
            auto *console = (Console *) lua_touserdata(L, lua_upvalueindex(1));
            console->printLine(str);
            return 0;
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
        Lua::pushProperty(state, "name", value.getPerson().getName());
        Lua::pushProperty(state, "position", value.getPosition());
        Lua::pushProperty(state, "life", value.getLife());
        Lua::pushProperty(state, "air", value.getAir());
        Lua::pushProperty(state, "ammo", value.getAmmo());
        Lua::pushProperty(state, "roundKills", value.getRoundKills());

        lua_pushliteral(state, "bonus");
        lua_newtable(state);
        //Lua::pushProperty(state, "name", "todo");
        Lua::pushProperty(state, "remainingTime", value.getBonusRemainingTime());
        lua_rawset(state, -3);

        Lua::pushProperty(state, "weapon", value.getWeapon());
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
    void Lua::pushValue(lua_State *state, World &value) {
        lua_newtable(state);
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

    void Lua::invoke(lua_State *state, Int32 nargs, Int32 nresults) {
        int result = lua_pcall(state, nargs, nresults, 0);
        if (result != LUA_OK) {
            std::string message = Format("Script error: {0}") << lua_tostring(state, -1);
            D6_THROW(ScriptException, message);
        }
    }
}


