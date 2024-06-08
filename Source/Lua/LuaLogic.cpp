#include <cassert>
#include "LuaLogic.h"
#include "LuaVM.h"
#include "Logic/FirstLevelLogic.h"
#include "Logic/SecondLevelLogic.h"
#include "Logic/FourthLevelLogic.h"

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}

// --------------------------------------------------------------------------

namespace LuaLogic {

	// LUA ROUTINES
	LUAF_DEFINITION( first_level_win ) {
		getFirstLevelLogic( ).winLevel( );
		return 0;
	}

	LUAF_DEFINITION( fourth_level_win ) {
		getFourthLevelLogic( ).winLevel( );
		return 0;
	}

	LUAF_DEFINITION( dentro_zona_plantar_serotonina) {
		getSecondLevelLogic( ).dentro_zona_plantar_serotonina( );
		return 0;
	}

	LUAF_DEFINITION( dentro_zona_plantar_noradrenalina) {
		getSecondLevelLogic( ).dentro_zona_plantar_noradrenalina( );
		return 0;
	}

	// -----------------------------------------------------------

	// LUA REGISTER FUNCTION
	void registerFunctions(lua_State *ls) {
		REGISTER_LUAF( first_level_win );
		REGISTER_LUAF( fourth_level_win );
		REGISTER_LUAF( dentro_zona_plantar_serotonina );
		REGISTER_LUAF( dentro_zona_plantar_noradrenalina );
	}
}
