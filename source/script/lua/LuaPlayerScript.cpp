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

#include "LuaPlayerScript.h"
#include "../ScriptException.h"

namespace Duel6::Script {
    namespace {
        int consolePrint(lua_State *L) {
            const char *str = luaL_checkstring(L, 1);
            auto *console = (Console *) lua_touserdata(L, lua_upvalueindex(1));
            console->printLine(Format("Script says: {0}") << str);
            return 0;
        }
    }

    LuaPlayerScript::LuaPlayerScript(const std::string &path, ScriptContext &context)
            : path(path), context(context), state(luaL_newstate()) {
    }

    LuaPlayerScript::~LuaPlayerScript() {
        lua_close(state);
    }

    void LuaPlayerScript::load() {
        luaL_openlibs(state);

        int status = luaL_loadfile(state, path.c_str());
        if (status) {
            std::string message = Format("Couldn't load script: {0}: {1}") << path << lua_tostring(state, -1);
            lua_close(state);
            D6_THROW(ScriptException, message);
        }

        registerGlobalContext();

        lua_pcall(state, 0, LUA_MULTRET, 0);
    }

    void LuaPlayerScript::registerGlobalContext() {
        registerConsoleType();
    }

    void LuaPlayerScript::registerConsoleType() {
        lua_newtable(state);
        lua_pushliteral(state, "print");
        lua_pushlightuserdata(state, &context.getConsole());
        lua_pushcclosure(state, consolePrint, 1);
        lua_rawset(state, -3);
        lua_setglobal(state, "console");
    }

    void LuaPlayerScript::roundStart(Player &player, RoundScriptContext &roundContext) {

    }

    void LuaPlayerScript::roundUpdate(Player &player, RoundScriptContext &roundContext) {

    }

    void LuaPlayerScript::roundEnd(Player &player, RoundScriptContext &roundContext) {

    }
}