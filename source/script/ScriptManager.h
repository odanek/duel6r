#include <angelscript.h>
#include <angelscript/add_on/scriptstdstring/scriptstdstring.h>
#include <assert.h>
#include <string>
#include <stdio.h>

namespace Duel6{
class ScriptManager{
public:

	static void print(std::string & msg){
		printf("%s", msg.c_str());
	}
	// Implement a simple message callback function
	static void MessageCallback(const asSMessageInfo *msg, void *param)
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
	ScriptManager(const Console & console):console(console){
		engine = asCreateScriptEngine();
		RegisterStdString(engine);
		auto r = engine->SetMessageCallback(asFUNCTION(ScriptManager::MessageCallback), 0, asCALL_CDECL);
		assert(r >= 0);
	    r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(ScriptManager::print), asCALL_CDECL);
	    assert(r >= 0);
		registerConsole();
	}
private:
	asIScriptEngine * engine;
	const Console & console;

	void registerConsole(){
		auto r = engine->RegisterObjectType("Console", sizeof(Console), asOBJ_REF | asOBJ_NOCOUNT);
		 if(r < 0){
		        printf("Failed to register void Console type.\n");
		 }

		 r = engine->RegisterObjectMethod("Console", "void print(const string & in)", asMETHOD(Console, print), asCALL_THISCALL);
		 if(r < 0){
		        printf("Failed to register void Console::print(string & msg).\n");
		 }
	}

};



//asIScriptEngine* initScriptEngine() {
//    // Create the script engine
//    asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
//    auto r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
//    assert(r >= 0);
//    // AngelScript doesn't have a built-in string type, as there is no definite standard
//    // string type for C++ applications. Every developer is free to register it's own string type.
//    // The SDK do however provide a standard add-on for registering a string type, so it's not
//    // necessary to implement the registration yourself if you don't want to.
//    RegisterStdString(engine);
//    r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
//    assert(r >= 0);
//    return engine;
//}
}