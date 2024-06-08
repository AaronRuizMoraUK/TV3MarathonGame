#ifndef LUA_ROUTINES_H
#define LUA_ROUTINES_H

struct lua_State;

/**
* Generic routines for lua.
*/

namespace LuaRoutines {
	// LUA REGISTER FUNCTION
	void registerFunctions(lua_State *ls);
}

#endif //LUA_ROUTINES_H
