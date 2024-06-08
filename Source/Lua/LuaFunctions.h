#ifndef LUA_FUNCTIONS_H
#define LUA_FUNCTIONS_H

struct lua_State;

namespace LuaFunctions {
	// LUA REGISTER FUNCTION
	void registerFunctions(lua_State *ls);
}

#endif //LUA_FUNCTIONS_H
