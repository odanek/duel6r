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

#include "ScriptManager.h"
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

namespace Duel6{


	void ScriptManager::print(std::string & msg){
		printf("%s", msg.c_str());
	}
	// Implement a simple message callback function
	void ScriptManager::MessageCallback(const asSMessageInfo *msg, void *param)
	{
	  const char *type;

	  switch(msg->type){
		case asMSGTYPE_ERROR:
			type = "ERR ";
			break;
		case asMSGTYPE_WARNING:
			type = "WARN";
			break;
		case asMSGTYPE_INFORMATION:
			type = "INFO";
			break;
	  }
	  printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
	}
	ScriptManager::ScriptManager(const Console & console):console(console){
		engine = asCreateScriptEngine();
		ctx = engine->CreateContext();

		RegisterStdString(engine);
		auto r = engine->SetMessageCallback(asFUNCTION(ScriptManager::MessageCallback), 0, asCALL_CDECL);
		assert(r >= 0);
		r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(ScriptManager::print), asCALL_CDECL);
		assert(r >= 0);
		registerConsoleType();
		registerLevelType();
		registerPlayerType();
	}

	LevelScript * ScriptManager::loadLevelScript(const char * scriptPath){
		if(File::exists(scriptPath)){
			asIScriptModule * module = loadModuleFromFile(engine, scriptPath);
			if(module == nullptr){
				D6_THROW(Exception, std::string("Cannot load script ") + std::string(scriptPath));
			}
			return new LevelScript(module, ctx);
		} else {
			D6_THROW(Exception, std::string("Cannot load script ") + std::string(scriptPath));
		}
	}

	void ScriptManager::registerConsoleType() {
		auto r = engine->RegisterObjectType("Console", sizeof(Console), asOBJ_REF | asOBJ_NOCOUNT);
		if (r < 0) {
			printf("Failed to register Console type.\n");
		}

		r = engine->RegisterObjectMethod("Console", "void print(const string & in)", asMETHOD(Console, print), asCALL_THISCALL);
		if (r < 0) {
			printf("Failed to register void Console::print(string & msg).\n");
		}
	}

	void ScriptManager::registerLevelType() {
		auto r = engine->RegisterObjectType("Level", sizeof(Level), asOBJ_REF | asOBJ_NOCOUNT);
		if (r < 0) {
			printf("Failed to register Level type.\n");
		}

		r = engine->RegisterObjectMethod("Level", "void raiseWater()", asMETHOD(Level, raiseWater), asCALL_THISCALL);
		if (r < 0) {
			printf("Failed to register void Level::raiseWater().\n");
		}
	}

	void ScriptManager::registerPlayerType() {
		auto r = engine->RegisterObjectType("Player", sizeof(Player), asOBJ_REF | asOBJ_NOCOUNT);
		if (r < 0) {
			printf("Failed to register Player type.\n");
		}

//		 r = engine->RegisterObjectProperty("Player", ""declaration, byteOffset)
		r = engine->RegisterObjectMethod("Player", "void die()", asMETHOD(Player, die), asCALL_THISCALL);
		if (r < 0) {
			printf("Failed to register void Player::die().\n");
		}
	}

	asIScriptModule * ScriptManager::loadModuleFromFile(asIScriptEngine * engine, const char * filePath) {
		CScriptBuilder scriptBuilder;

		int r = scriptBuilder.StartNewModule(engine, filePath);
		if (r < 0) {
			printf("Unrecoverable error while starting a new module with script %s\n", filePath);
			return nullptr;
		}
		r = scriptBuilder.AddSectionFromFile(filePath);
		if (r < 0) {
			printf("Please correct the errors in the script %s and try again.\n", filePath);
			return nullptr;
		}
		r = scriptBuilder.BuildModule();
		if (r < 0) {
			printf("Please correct the errors in the script %s and try again.\n", filePath);
			return nullptr;
		}

		return scriptBuilder.GetModule();
	}

}