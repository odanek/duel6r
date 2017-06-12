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
#include <angelscript/add_on/scriptarray/scriptarray.h>
#include <angelscript/add_on/scriptgrid/scriptgrid.h>
#include <angelscript/add_on/scriptdictionary/scriptdictionary.h>
#include <assert.h>
#include <string>
#include <stdio.h>

#include "../Level.h"
#include "../Player.h"
#include "../File.h"
#include "../Exception.h"
#include "../Vector.h"
#include "../Round.h"
#include "../ai/PathSegment.h"

#define TRY(CALL, MESSAGE) expect(__FILE__, __LINE__, (CALL), MESSAGE);

template<typename T>
void constructor(void * memory){
	new (memory)T();
}

template<typename T>
void constructor(const T & t, void * memory){
	new (memory)T(t);
}

template<typename T>
void constructor(const T && t, void * memory){
	new (memory)T(t);
}
template<typename T>
void destructor(void * memory){
	((T*)memory)->~T();
}

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
	ScriptManager::ScriptManager(Sound & sound, Console * console):console(console), sound(sound){
		engine = asCreateScriptEngine();
		ctx = engine->CreateContext();
		// enable string type
		RegisterStdString(engine);
		// enable array type e.g. array<type> or type[]
		RegisterScriptArray(engine, true);
		// enable grid type e.g. grid<uint> or grid<uint> x = { {1,2,3}, {4,5,6}, {7,8,9}}
		RegisterScriptGrid(engine);
		// enable dictionary type
		RegisterScriptDictionary(engine);
		auto r = engine->SetMessageCallback(asFUNCTION(ScriptManager::MessageCallback), 0, asCALL_CDECL);
		assert(r >= 0);
		r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(ScriptManager::print), asCALL_CDECL);
		assert(r >= 0);
		registerConsoleType();
		registerLevelType();
		registerPersonType();
		registerVectorType();
		registerPlayerType();
		registerSampleType();
		registerSoundType();
		registerWorldType();
		registerRoundType();
		registerPathSegment();
	}

	void ScriptManager::checkedLoad(asIScriptEngine * engine, const char * scriptPath, asIScriptModule ** module) {
		if (File::exists(scriptPath)) {
			*module = loadModuleFromFile(engine, scriptPath);
			if (*module != nullptr) {
				return;
			}
			printf("Cannot load script %s\n", scriptPath);
		}
		*module = engine->GetModule("_empty_", asGM_ONLY_IF_EXISTS);
		if(*module == nullptr){
			CScriptBuilder scriptBuilder;
			scriptBuilder.StartNewModule(engine, "_empty_");
			scriptBuilder.BuildModule();
			*module = scriptBuilder.GetModule();
		}
	}

	void ScriptManager::registerPathSegment() {
		TRY(engine->RegisterObjectType("PathSegment", sizeof(PathSegment), asOBJ_REF),
				"Failed to register PathSegment");
		TRY(engine->RegisterObjectBehaviour("PathSegment", asBEHAVE_FACTORY, "PathSegment@ f()", asFUNCTIONPR(PathSegment::factory,(void), PathSegment *), asCALL_CDECL),
				"Failed to register PathSegment");
		TRY(engine->RegisterObjectBehaviour("PathSegment", asBEHAVE_FACTORY, "PathSegment@ f(uint, uint, uint, int)", asFUNCTIONPR(PathSegment::factory, (uint, uint, uint, int), PathSegment *), asCALL_CDECL),
				"Failed to register PathSegment");
		TRY(engine->RegisterObjectBehaviour("PathSegment", asBEHAVE_ADDREF, "void f()", asMETHOD(PathSegment, addRef), asCALL_THISCALL),
				"Failed to register PathSegment");
		TRY(engine->RegisterObjectBehaviour("PathSegment", asBEHAVE_RELEASE, "void f()", asMETHOD(PathSegment, release), asCALL_THISCALL),
				"Failed to register PathSegment");
		TRY(engine->RegisterObjectProperty("PathSegment", "uint id", asOFFSET(PathSegment, id)),
				"Failed to register PathSegment.id");
		TRY(engine->RegisterObjectProperty("PathSegment", "uint l", asOFFSET(PathSegment, l)),
				"Failed to register PathSegment.l");
		TRY(engine->RegisterObjectProperty("PathSegment", "uint r", asOFFSET(PathSegment, r)),
				"Failed to register PathSegment.r");
		TRY(engine->RegisterObjectProperty("PathSegment", "int y", asOFFSET(PathSegment, y)),
				"Failed to register PathSegment.y");
	}

	void ScriptManager::registerConsoleType() {
		TRY(engine->RegisterObjectType("Console", sizeof(Console), asOBJ_REF | asOBJ_NOCOUNT),
			"Failed to register Console type.\n");
		TRY(engine->RegisterObjectMethod("Console", "void print(const string & in) const", asMETHODPR(Console, print, (const std::string&), Console&), asCALL_THISCALL),
			"Failed to register void Console::print(string & msg).\n");
		// Enables the console.print()
		TRY(engine->RegisterGlobalProperty("const Console @console", &console),
			"Failed to register global Console console.\n");
	}

	int getPlayersAliveCount(void * world) {
		World * w = static_cast<World *>(world);
		int cnt = 0;
		for (auto player : w->getPlayers()){
			if(player.isAlive()) cnt ++;
		}
		return cnt;
	}
	void ScriptManager::registerWorldType() {
		TRY(engine->RegisterObjectType("World", sizeof(World),  asOBJ_REF | asOBJ_NOCOUNT),
			"Failed to register World type.\n");
		TRY(engine->RegisterObjectMethod("World", "void raiseWater()", asMETHOD(World, raiseWater), asCALL_THISCALL),
			"Failed to register void World::raiseWater().\n");
		TRY(engine->RegisterObjectMethod("World", "void setLevelBlock(uint16 block, int x, int y)", asMETHODPR(World, setLevelBlock, (Uint16, Int32, Int32), void), asCALL_THISCALL),
			"Failed to register void World::setLevelBlock().\n");
		TRY(engine->RegisterObjectMethod("World", "int getPlayersAliveCount()", asFUNCTIONPR(getPlayersAliveCount, (void *), int), asCALL_CDECL_OBJLAST),
			"Failed to register void World::setLevelBlock().\n");
	}
	void ScriptManager::registerLevelType() {
		TRY(engine->RegisterObjectType("Level", sizeof(Level),  asOBJ_REF | asOBJ_NOCOUNT),
			"Failed to register Level type.\n");
		TRY(engine->RegisterObjectMethod("Level", "void raiseWater()", asMETHOD(Level, raiseWater), asCALL_THISCALL),
			"Failed to register void Level::raiseWater().\n");
		TRY(engine->RegisterObjectMethod("Level", "int getWidth() const", asMETHODPR(Level, getWidth, (void) const, int), asCALL_THISCALL),
			"Failed to register void Level::getWidtht().\n");
		TRY(engine->RegisterObjectMethod("Level", "int getHeight() const", asMETHODPR(Level, getHeight, (void) const, int), asCALL_THISCALL),
			"Failed to register void Level::getHeigh().\n");
		TRY(engine->RegisterObjectMethod("Level", "bool isWater(int x, int y) const", asMETHODPR(Level, isWater, (Int32, Int32) const, bool), asCALL_THISCALL),
			"Failed to register void Level::isWater().\n");
		TRY(engine->RegisterObjectMethod("Level", "bool isWall(int x, int y, bool outside) const", asMETHODPR(Level, isWall, (Int32, Int32, bool) const, bool), asCALL_THISCALL),
			"Failed to register void Level::isWall().\n");
		TRY(engine->RegisterObjectMethod("Level", "bool isWall(float x, float y, bool outside) const", asMETHODPR(Level, isWall, (Float32, Float32, bool) const, bool), asCALL_THISCALL),
			"Failed to register void Level::isWall().\n");
	}

	void ScriptManager::registerPersonType() {
		TRY(engine->RegisterObjectType("Person", sizeof(Person), asOBJ_REF | asOBJ_NOCOUNT),
			"Failed to register Person type.\n");
		TRY(engine->RegisterObjectMethod("Person", "const string & getName() const", asMETHODPR(Person, getName, (void) const, const std::string &), asCALL_THISCALL),
			"Failed to register void Person::getName().\n");
	}

	void ScriptManager::registerVectorType() {
		TRY(engine->RegisterObjectType("Vector", sizeof(Vector), asOBJ_REF | asOBJ_NOCOUNT),
			"Failed to register Vector type.\n");
		TRY(engine->RegisterObjectProperty("Vector", "float x", asOFFSET(Vector, x)),
			"Failed to register Vector.x.\n");
		TRY(engine->RegisterObjectProperty("Vector", "float y", asOFFSET(Vector, y)),
			"Failed to register Vector.y.\n");
	}

	void ScriptManager::registerPlayerType() {

		TRY(engine->RegisterEnum("Orientation"),
			"Failed to register enum Orientation.\n");
		TRY(engine->RegisterEnumValue("Orientation", "Left", (int) Orientation::Left),
			"Failed to register enum Orientation.\n");
		TRY(engine->RegisterEnumValue("Orientation", "Right", (int) Orientation::Right),
			"Failed to register enum Orientation.\n");
		TRY(engine->RegisterObjectType("Player", sizeof(Player), asOBJ_REF | asOBJ_NOCOUNT),
			"Failed to register Player type.\n");
		TRY(engine->RegisterObjectMethod("Player", "bool isOnElevator() const", asMETHODPR(Player, isOnElevator, (void) const, bool), asCALL_THISCALL),
			"Failed to register void Player::isOnElevator().\n");
		TRY(engine->RegisterObjectMethod("Player", "void die()", asMETHOD(Player, die), asCALL_THISCALL),
			"Failed to register void Player::die().\n");
		TRY(engine->RegisterObjectMethod("Player", "bool isDoubleJumping() const", asMETHODPR(Player, isDoubleJumping, (void) const, bool), asCALL_THISCALL),
			"Failed to register void Player::isDoubleJumping().\n");
		TRY(engine->RegisterObjectMethod("Player", "Person & getPerson()", asMETHODPR(Player, getPerson, (void), Person &), asCALL_THISCALL),
			"Failed to register void Player::getPerson().\n");
		TRY(engine->RegisterObjectMethod("Player", "bool hasFlag(uint flag) const", asMETHODPR(Player, hasFlag, (Uint32) const, bool), asCALL_THISCALL),
			"Failed to register void Player::hasFlag().\n");
		TRY(engine->RegisterObjectMethod("Player", "void setFlag(uint flag)", asMETHODPR(Player, setFlag, (Uint32), void), asCALL_THISCALL),
			"Failed to register void Player::setFlag().\n");
		TRY(engine->RegisterObjectMethod("Player", "void unsetFlag(uint flag)", asMETHODPR(Player, unsetFlag, (Uint32), void), asCALL_THISCALL),
			"Failed to register void Player::unsetFlag().\n");

		TRY(engine->RegisterObjectMethod("Player", "bool hasControlOn(uint8 flag) const", asMETHODPR(Player, hasControlOn, (Uint8) const, bool), asCALL_THISCALL),
			"Failed to register void Player::hasFlag().\n");
		TRY(engine->RegisterObjectMethod("Player", "void setControl(uint8 flag)", asMETHODPR(Player, setControl, (Uint8), void), asCALL_THISCALL),
			"Failed to register void Player::setFlag().\n");
		TRY(engine->RegisterObjectMethod("Player", "void unsetControl(uint8 flag)", asMETHODPR(Player, unsetControl, (Uint8), void), asCALL_THISCALL),
			"Failed to register void Player::unsetFlag().\n");
		TRY(engine->RegisterObjectMethod("Player", "Orientation getOrientation()", asMETHODPR(Player, getOrientation, (void) const, Orientation), asCALL_THISCALL),
			"Failed to register void Player::getOrientation().\n");

		TRY(engine->RegisterObjectProperty("Player", "Vector position", asOFFSET(Player, position)),
			"Failed to register Player.position.\n");
		TRY(engine->RegisterObjectProperty("Player", "uint ammo", asOFFSET(Player, ammo)),
			"Failed to register Player.ammo.\n");
		TRY(engine->RegisterObjectProperty("Player", "float life", asOFFSET(Player, life)),
			"Failed to register Player.life.\n");
	}

	void ScriptManager::registerSoundType() {
		TRY(engine->RegisterObjectType("Sound", sizeof(Sound), asOBJ_REF | asOBJ_NOCOUNT),
			"Failed to register Sound type.\n");
		TRY(engine->RegisterObjectMethod("Sound", "Sample loadSample(const string & in) const", asMETHODPR(Sound, loadSample, (const std::string&), Sound::Sample), asCALL_THISCALL),
			"Failed to register void Sound::loadSample().\n");
		TRY(engine->RegisterGlobalProperty("const Sound  SOUND", &sound),
			"Failed to register global Sound SOUND.\n");
	}

	void ScriptManager::registerRoundType() {
			TRY(engine->RegisterObjectType("Round", sizeof(Round), asOBJ_REF | asOBJ_NOCOUNT),
				"Failed to register Round type.\n");
			TRY(engine->RegisterObjectMethod("Round", "float getRemainingYouAreHere() const", asMETHODPR(Round, getRemainingGameOverWait, (void) const, Float32), asCALL_THISCALL),
				"Failed to register void Round::getRemainingYouAreHere().\n");
			TRY(engine->RegisterObjectMethod("Round", "int getRoundNumber() const", asMETHODPR(Round, getRoundNumber, (void) const, Int32), asCALL_THISCALL),
				"Failed to register void Round::getRoundNumber().\n");

			TRY(engine->RegisterObjectMethod("Round", "World @ getWorld() const", asMETHODPR(Round, getWorld, (void) const, const World &), asCALL_THISCALL),
				"Failed to register void Round::getWorld().\n");
	}


	void ScriptManager::registerSampleType() {
		TRY(engine->RegisterObjectType("Sample", sizeof(Sound::Sample),  asOBJ_VALUE | asGetTypeTraits<Sound::Sample>()),
			"Failed to register Sample type.\n");
		TRY(engine->RegisterObjectBehaviour("Sample", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(constructor<Sound::Sample>, (void *), void), asCALL_CDECL_OBJLAST),
			"Failed to register Sample constructor.\n");
		TRY(engine->RegisterObjectBehaviour("Sample", asBEHAVE_CONSTRUCT, "void f(const Sample & in)", asFUNCTIONPR(constructor<Sound::Sample>, (const Sound::Sample &, void *), void), asCALL_CDECL_OBJLAST),
			"Failed to register Sample constructor.\n");
		TRY(engine->RegisterObjectBehaviour("Sample", asBEHAVE_DESTRUCT, "void f()", asFUNCTIONPR(destructor<Sound::Sample>, (void *), void), asCALL_CDECL_OBJLAST),
			"Failed to register Sample destructor.\n");
		TRY(engine->RegisterObjectMethod("Sample", "Sample & opAssign(const Sample & in)", asMETHODPR(Sound::Sample, operator =, (const Sound::Sample &), Sound::Sample &), asCALL_THISCALL),
			"Failed to register Sample opASsign.\n");
		TRY(engine->RegisterObjectMethod("Sample", "void play()", asMETHOD(Sound::Sample, play), asCALL_THISCALL),
			"Failed to register void Sample::play().\n");
	}

	void ScriptManager::registerMapType(){
//		TRY(engine->RegisterObjectType("MapIntBool", sizeof(std::map<int, bool>),  asOBJ_VALUE | asGetTypeTraits<std::map<int, bool>>() ),
//			"Failed to register MapIntBool type.\n");
//		TRY(engine->RegisterObjectMethod("MapIntBool", "bool & opIndex(int index)", asMETHODPR((std::map<int, bool>), at, (int),  (std::map<int, bool>::mapped_type)), asCALL_THISCALL),
//			"Failed to register void MapIntBool::[].\n");
//		TRY(engine->RegisterObjectMethod("MapIntBool", "const bool & opIndex(int index) const", asMETHODPR((std::map<int, bool>), at, (int), (const bool &)), asCALL_THISCALL),
//			"Failed to register void MapIntBool::[].\n");

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

	void ScriptManager::expect(const char * FILE, int LINE, int r, const char * message){
		if (r < asEXECUTION_FINISHED) {
			fprintf(stderr, "%s:%d %s\n", FILE, LINE, message);
		}
	}

}