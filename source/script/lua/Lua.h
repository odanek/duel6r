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

#ifndef DUEL6_SCRIPT_LUA_LUA_H
#define DUEL6_SCRIPT_LUA_LUA_H

#include <lua.hpp>
#include "../../Type.h"
#include <string>

namespace Duel6::Script {
    class Lua {
    public:
        template<class T>
        static void pushValue(lua_State *state, T &value);

        template<class T>
        static void pushValue(lua_State *state, const T &value);

        template<class T>
        static void pushProperty(lua_State *state, const std::string &name, T &value) {
            lua_pushstring(state, name.c_str());
            pushValue(state, value);
            lua_rawset(state, -3);
        }

        template<class T>
        static void pushProperty(lua_State *state, const std::string &name, const T &value) {
            lua_pushstring(state, name.c_str());
            pushValue(state, value);
            lua_rawset(state, -3);
        }

        template<class T>
        static void registerGlobal(lua_State *state, const std::string &name, T &value) {
            pushValue(state, value);
            lua_setglobal(state, name.c_str());
        }

        template<class T>
        static void registerGlobal(lua_State *state, const std::string &name, const T &value) {
            pushValue(state, value);
            lua_setglobal(state, name.c_str());
        }

        template<class C, class T>
        static void pushMetaIndex(lua_State *state, C callback, T &value) {
            lua_newtable(state);
            lua_pushliteral(state, "__index");
            lua_pushlightuserdata(state, &value);
            lua_pushcclosure(state, callback, 1);
            lua_rawset(state, -3);
            lua_setmetatable(state, -2);
        }

        static void invoke(lua_State *state, Int32 nargs, Int32 nresults);
    };
}

#endif
