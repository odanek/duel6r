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

#include "LuaPersonScript.h"
#include "../ScriptException.h"
#include "../../Player.h"
#include "../../World.h"
#include "Lua.h"

namespace Duel6::Script {
    namespace {
        int roundMetaIndex(lua_State *state) {
            auto &context = *((RoundScriptContext *) lua_touserdata(state, lua_upvalueindex(1)));
            const char *propertyName = luaL_checkstring(state, 2);

            if (!strcmp(propertyName, "shots")) {
                Lua::pushValue(state, context.getWorld().getShotList());
                return 1;
            }

            return 0;
        }
    }

    LuaPersonScript::LuaPersonScript(const std::string &path, ScriptContext &context, PersonScriptContext &personContext)
            : path(path), context(context), personContext(personContext), state(luaL_newstate()) {
    }

    LuaPersonScript::~LuaPersonScript() {
        lua_close(state);
    }

    void LuaPersonScript::load() {
        luaL_openlibs(state);

        int status = luaL_loadfile(state, path.c_str());
        if (status) {
            std::string message = Format("Couldn't load script: {0}: {1}") << path << lua_tostring(state, -1);
            D6_THROW(ScriptException, message);
        }

        registerGlobalContext();

        Lua::invoke(state, 0, LUA_MULTRET);
    }

    void LuaPersonScript::registerGlobalContext() {
        Lua::registerGlobal(state, "script", context);
        Lua::registerGlobal(state, "person", personContext);
    }

    void LuaPersonScript::registerRoundContext(Player &player, RoundScriptContext &roundContext) {
        lua_newtable(state);
        Lua::pushMetaIndex(state, roundMetaIndex, roundContext);

        Lua::pushProperty(state, "player", player);

        lua_pushliteral(state, "otherPlayers");
        registerOtherPlayers(player, roundContext);
        lua_rawset(state, -3);

        Lua::pushProperty(state, "level", roundContext.getWorld().getLevel());
    }

    void LuaPersonScript::registerOtherPlayers(Player &player, RoundScriptContext &roundContext) {
        World &world = roundContext.getWorld();
        lua_newtable(state);
        Int32 index = 1;
        for (Player &otherPlayer : world.getPlayers()) {
            if (!player.is(otherPlayer)) {
                lua_pushinteger(state, index);
                Lua::pushValue(state, otherPlayer);
                lua_rawset(state, -3);
                ++index;
            }
        }
    }

    void LuaPersonScript::registerShots(RoundScriptContext &roundContext) {
        lua_newtable(state);
    }

    void LuaPersonScript::roundStart(Player &player, RoundScriptContext &roundContext) {
        registerRoundContext(player, roundContext);

        lua_getglobal(state, "roundStart");
        lua_pushvalue(state, -2);
        Lua::invoke(state, 1, 0);
    }

    void LuaPersonScript::roundUpdate(Uint32 roundTime, Player &player, RoundScriptContext &roundContext) {
        lua_getglobal(state, "roundUpdate");
        lua_pushvalue(state, -2);
        lua_pushinteger(state, roundTime);
        Lua::invoke(state, 2, 0);
    }

    void LuaPersonScript::roundEnd(Uint32 roundTime, Player &player, RoundScriptContext &roundContext) {
        lua_getglobal(state, "roundEnd");
        lua_pushvalue(state, -2);
        lua_pushinteger(state, roundTime);
        Lua::invoke(state, 2, 0);

        // Pop the round context
        lua_pop(state, 1);
    }
}