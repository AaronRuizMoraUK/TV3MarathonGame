#ifndef LUA_LOGIC_H
#define LUA_LOGIC_H

struct lua_State;

/**
* Logic routines for lua.
*/

namespace LuaLogic {
	// LUA REGISTER FUNCTION
	void registerFunctions(lua_State *ls);
}

#endif //LUA_LOGIC_H
