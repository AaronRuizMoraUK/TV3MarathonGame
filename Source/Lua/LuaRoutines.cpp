#include <cassert>
#include "LuaRoutines.h"
#include "LuaVM.h"
#include "Global/Rendering.h"
#include "GameState/IGameState.h"
#include "Global/Variables.h"
#include "SoundPlayer/SoundPlayer.h"
#include "Logic/Logic.h"
#include "Particles/PSManager.h"

#include "Factories/CollisionShapeFactory.h"
#include "Factories/MeshFactory.h"
#include "Factories/QuadFactory.h"
#include "Factories/TextureFactory.h"
#include "Factories/EffectFactory.h"
#include "Factories/ParticleFactory.h"

#include "Managers/ControllerManager.h"
#include "Managers/PlanetMapManager.h"
#include "Managers/ROAResourceManager.h"
#include "Managers/CalAnimCoreModelManager.h"
#include "Managers/EnemyManager.h"
#include "Managers/BrainManager.h"

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}

// --------------------------------------------------------------------------

template<class T>
void loadSingleton( ) {
	T::get( )->load( );
}

template<class T>
void destroySingleton( ) {
	T *singleton = T::get( );
	if( singleton != NULL )
		delete singleton, singleton=NULL;
}

template<class T>
void reload( ) {
#ifndef NDEBUG
	T::get( )->reload( );
#endif
}

// --------------------------------------------------------------------------

namespace LuaRoutines {

	// LUA ROUTINES
	LUAF_DEFINITION( go_to_game_state ) {
		int num_args = lua_gettop(ls);
		if( num_args != 1 ) {
			assert( !"Number of parameters incorrect in go_to_game_state in LuaRoutines" );
			return 0;
		}
		const char *game_state = lua_tostring(ls, 1);
		IGameState::getState( game_state )->getFocus( );
		return 0;
	}

	LUAF_DEFINITION( load_everything ) {
		// -- Global
		Global::score_recorder.load( );
		Global::fade.load( );

		// -- Sound Player
		loadSingleton<SoundPlayer>( );

		// -- Particles
		loadSingleton<PSManager>( );

		// -- Managers
		loadSingleton<ControllerManager>( );
		loadSingleton<ROAResourceManager>( );
		loadSingleton<PlanetMapManager>( );
		loadSingleton<CalAnimCoreModelManager>( );
		loadSingleton<EnemyManager>( );
		loadSingleton<BrainManager>( );
		// SpatialManager y CollisionManager no hace falta

		// -- Player
		Global::player = new Player( );
		assert( Global::player );
		Global::player->setCalAnimModel( "player" );
		Global::player->SETMATERIAL("player_skin.dds", "shader.fx", "skinning_tech");
		const ShapeHandle *shape_handle = CollisionShapeFactory::get( )->createCollisionShape( Global::player->getMesh( )
			, CollisionShapeFactory::CYLINDER_SHAPE, 1.0f );
		assert( shape_handle );
		Global::player->loc.createCollisionObject( shape_handle );

		// -- sky
		const IMesh *sky_mesh = MeshFactory::get( )->createMesh( "sky.mesh" );
		assert( sky_mesh );
		Global::sky.setMesh( sky_mesh );
		Global::sky.SETMATERIAL( "sky.dds", "shader.fx", "dummy_tech" );
		IAnimationController *ai_controller = ControllerManager::get( )->getAnimationController( "sky_rotation" );
		assert( ai_controller );
		Global::sky.setAnimationController( ai_controller );

		// -- Trigger Manager
		Global::triggers = new TriggerManager( );
		assert( Global::triggers );

		// -- Logic
		Logic::load( );

		// -- GameStates
		IGameState::load( );

		return 0;
	}

	LUAF_DEFINITION( destroy_everything ) {
		// -- GameStates
		IGameState::destroy();

		// -- Logic
		Logic::destroy( );

		// -- Trigger Manager
		if( Global::triggers )
			delete Global::triggers, Global::triggers = NULL;

		// -- Player
		if( Global::player )
			delete Global::player, Global::player = NULL;

		// -- Managers
		destroySingleton<BrainManager>( );
		destroySingleton<EnemyManager>( );
		destroySingleton<CalAnimCoreModelManager>( );
		destroySingleton<PlanetMapManager>( );
		destroySingleton<ROAResourceManager>( );
		destroySingleton<ControllerManager>( );
		// SpatialManager y SpatialManager no hace falta

		// -- Particles
		destroySingleton<PSManager>( );

		// -- Sound Player
		destroySingleton<SoundPlayer>( );

		// -- Factories
		destroySingleton<CollisionShapeFactory>( );
		destroySingleton<TextureFactory>( );
		destroySingleton<MeshFactory>( );
		destroySingleton<QuadFactory>( );
		destroySingleton<EffectFactory>( );
		destroySingleton<ParticleFactory>( );

		return 0;
	}

	LUAF_DEFINITION( show_dinamic_help ) {
		int num_args = lua_gettop(ls);
		if( num_args != 1 ) {
			assert( !"Number of parameters incorrect in show_dinamic_help in LuaRoutines" );
			return 0;
		}

		const char *help_desc_name = lua_tostring(ls, 1 );
		DinamicHelp::DinamicHelpDesc help_desc = IGameState::getDinamicHelpDesc( help_desc_name );
		IGameState::showDinamicHelp( help_desc );
		return 0;
	}

	// DEBUG ----------------------------------------------------

	LUAF_DEFINITION( print ) {
#ifndef NDEBUG
		int num_args = lua_gettop(ls);
		if( num_args != 1 ) {
			assert( !"Number of parameters incorrect in print in LuaRoutines" );
			return 0;
		}
		std::string msg = lua_tostring(ls, 1);
		msg = "LUA: " + msg + "\n";
		printDebugString( msg.c_str() );
#endif
		return 0;
	}

	LUAF_DEFINITION( warn ) {
#ifndef NDEBUG
		int num_args = lua_gettop(ls);
		if( num_args != 1 ) {
			assert( !"Number of parameters incorrect in warn in LuaRoutines" );
			return 0;
		}
		std::string msg = lua_tostring(ls, 1);
		msg = "LUA: " + msg + "\n";
		warn( msg.c_str() );
#endif
		return 0;
	}

	LUAF_DEFINITION( reload_textures ) {
		reload<TextureFactory>( );
		return 0;
	}

	LUAF_DEFINITION( reload_shaders ) {
		reload<EffectFactory>( );
		return 0;
	}

	LUAF_DEFINITION( reload_planetmaps ) {
		reload<PlanetMapManager>( );
		return 0;
	}

	LUAF_DEFINITION( reload_roa_resources ) {
		reload<ROAResourceManager>( );
		return 0;
	}

	LUAF_DEFINITION( reload_animations ) {
		reload<CalAnimCoreModelManager>( );
		return 0;
	}

	LUAF_DEFINITION( reload_enemies ) {
		reload<EnemyManager>( );
		return 0;
	}

	// -----------------------------------------------------------

	// LUA REGISTER FUNCTION
	void registerFunctions(lua_State *ls) {
		REGISTER_LUAF( go_to_game_state );
		REGISTER_LUAF( load_everything );
		REGISTER_LUAF( destroy_everything );
		REGISTER_LUAF( show_dinamic_help );

		REGISTER_LUAF( print );
		REGISTER_LUAF( warn );
		REGISTER_LUAF( reload_textures );
		REGISTER_LUAF( reload_shaders );
		REGISTER_LUAF( reload_planetmaps );
		REGISTER_LUAF( reload_roa_resources );
		REGISTER_LUAF( reload_animations );
		REGISTER_LUAF( reload_enemies );
	}
}
