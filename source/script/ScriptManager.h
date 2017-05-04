#include <angelscript.h>
#include <angelscript/add_on/scriptstdstring/scriptstdstring.h>
#include <assert.h>
#include <string>
#include <stdio.h>

namespace Duel6{
class ScriptManager{
public:

	void print(std::string & msg){
		printf("%s", msg.c_str());
	}
	// Implement a simple message callback function
	void MessageCallback(const asSMessageInfo *msg, void *param)
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
	ScriptManager(){
		engine = asCreateScriptEngine();
		RegisterStdString(engine);
		auto r = engine->SetMessageCallback(asMETHOD(ScriptManager, MessageCallback), this, asCALL_THISCALL);
		assert(r >= 0);
	}
private:
	asIScriptEngine * engine;
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