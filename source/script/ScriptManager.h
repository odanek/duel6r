/*
* Copyright (c) 2017, Frantisek Veverka
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Frantisek Veverka nor the
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
#pragma once

#include <angelscript.h>
#include <angelscript/add_on/scriptstdstring/scriptstdstring.h>
#include <angelscript/add_on/scriptbuilder/scriptbuilder.h>

#include <assert.h>
#include <string>
#include <stdio.h>

#include "LevelScript.h"

#include "../Level.h"
#include "../Player.h"
#include "../File.h"
#include "../Exception.h"
#include "../Sound.h"
namespace Duel6{
/**
 * Exposes game classes for use in the scripts
 * Loads scripts
 *
 */
class ScriptManager{
public:

	static void print(std::string & msg);
	// Implement a simple message callback function
	static void MessageCallback(const asSMessageInfo *msg, void *param);

	ScriptManager(Sound & sound, Console * console);
	LevelScript * loadLevelScript(const char * scriptPath);

private:
	asIScriptEngine * engine;
	// For synchronous, single threaded scripting, we only need single scriptcontext object
	asIScriptContext * ctx;
	Console * console;
	Sound & sound;
	void registerConsoleType();
	void registerLevelType();
	void registerVectorType();
	void registerPlayerType();
	void registerPersonType();
	void registerPathSegment();
	void registerSoundType();
	void registerRoundType();
	void registerSampleType();
	void registerMapType();

	asIScriptModule * loadModuleFromFile(asIScriptEngine * engine, const char * filePath);
	void expect(const char * FILE, int LINE, int r, const char * message);
};
}