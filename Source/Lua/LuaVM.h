#ifndef LUA_VM_H
#define LUA_VM_H

#include <string>

struct lua_State;

#define LUAF_DEFINITION( name ) int luaf_##name( lua_State *ls )
#define REGISTER_LUAF( name )   lua_register(ls, #name, luaf_##name )

/**
* Lua Virtual Machine to execute script code.
*
* It is a singleton.
*/

class LuaVM
{
protected:
	static std::string pathToScripts;

	lua_State *ls;

public:
	LuaVM();
	~LuaVM();

	void load();

	bool execute( const std::string &pScript );
	bool executeCmd( const char *cmd, ... );
};

#endif //LUA_VM_H
