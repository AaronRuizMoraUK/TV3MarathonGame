#include <cassert>
#include "LuaVM.h"
#include "Global/Variables.h"
#include "Global/Rendering.h"

#include "LuaRoutines.h"
#include "LuaLogic.h"
#include "LuaFunctions.h"

extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
}

//#define WARN_NO_SCRIPT

std::string LuaVM::pathToScripts = "";

LuaVM::LuaVM()
: ls(NULL)
{
}

LuaVM::~LuaVM() {
	printDebugString("Destroying LuaVM...\n");

	lua_close(ls); // Close lua
}

void LuaVM::load() {
	// Get path to scripts
	if( pathToScripts.empty( ) )
		pathToScripts = Global::data_locator.getPath("script");

	// LUA
	ls=lua_open();

	// Register LUA Routines
	LuaRoutines::registerFunctions(ls);
	LuaLogic::registerFunctions(ls);
	LuaFunctions::registerFunctions(ls);
}

bool LuaVM::execute( const std::string &pScript ) 
{
	std::string script = pathToScripts + pScript;

	int result = luaL_dofile(ls, script.c_str());
	bool ok = (result==0);

#ifndef NDEBUG
#ifdef WARN_NO_SCRIPT
	if(!ok)
		warn( "Lua File \"%s\" not found", script.c_str( ) );
#endif
#endif

	return ok;
}

bool LuaVM::executeCmd( const char *cmd, ... ) 
{
	char txt[512];
	va_list ap;

	va_start (ap, cmd);
	_vsnprintf_s (txt, sizeof(txt), 512, cmd, ap);
	va_end(ap);

	int result = luaL_dostring( ls, txt );
	bool ok = (result==0);

#ifndef NDEBUG
	if(!ok)
		warn( "Lua command \"%s\" unknown", cmd );
#endif
	
	return ok;
}
