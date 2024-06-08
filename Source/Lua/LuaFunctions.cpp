#include <cassert>
#include "LuaFunctions.h"
#include "LuaVM.h"
#include "Global/Variables.h"
#include "Object/AnimatedObject/Player/Player.h"
#include "GameState/Common/DinamicHelp.h"
#include "GameState/Game/FirstLevel/FirstLevelState.h"
#include "GameState/Game/SecondLevel/SecondLevelState.h"
#include "GameState/Game/ThirdLevel/ThirdLevelState.h"
#include "GameState/Game/FourthLevel/FourthLevelState.h"

extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
}

// --------------------------------------------------------------------------

namespace LuaFunctions {

	// LUA ROUTINES
	LUAF_DEFINITION( enable_player_controller ) {
		Player::player_controller_enabled = true;
		return 0;
	}

	LUAF_DEFINITION( disable_player_controller ) {
		Player::player_controller_enabled = false;
		return 0;
	}

	LUAF_DEFINITION( create_logic_timer ) {
		int num_args = lua_gettop(ls);
		if( num_args != 3 ) {
			assert( !"Number of parameters incorrect in go_to_game_state in LuaRoutines" );
			return 0;
		}

		const char *text = lua_tostring(ls, 1 );
		float       time = (float)lua_tonumber(ls, 2);
		unsigned logic_timer_type = (unsigned)lua_tointeger(ls, 3);
		assert( logic_timer_type < LogicTimer::NUM_TYPES );

		Global::triggers->addLogicTimer( text, time, (LogicTimer::Type)logic_timer_type );
		return 0;
	}

	LUAF_DEFINITION( show_player_hurt ) {
		// Si no hay estado actual no hacer nada
		if( IGameState::currentState == NULL )
			return 0;

		const char *curr_state_name = IGameState::currentState->getName( );
		if( strcmp( curr_state_name, "first_level_state" ) == 0 )
			((FirstLevelState *)IGameState::currentState)->playerHurt( );
		else if( strcmp( curr_state_name, "second_level_state" ) == 0 )
			((SecondLevelState *)IGameState::currentState)->playerHurt( );
		else if( strcmp( curr_state_name, "third_level_state" ) == 0 )
			((ThirdLevelState *)IGameState::currentState)->playerHurt( );
		else if( strcmp( curr_state_name, "fourth_level_state" ) == 0 )
			((FourthLevelState *)IGameState::currentState)->playerHurt( );

		return 0;
	}

	// -----------------------------------------------------------

	// LUA REGISTER FUNCTION
	void registerFunctions(lua_State *ls) {
		REGISTER_LUAF( enable_player_controller );
		REGISTER_LUAF( disable_player_controller );
		REGISTER_LUAF( create_logic_timer );
		REGISTER_LUAF( show_player_hurt );
	}
}
