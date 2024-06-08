#include "ThirdLevelLogic.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Particles/PSManager.h"
#include "Managers/BrainManager.h"

ThirdLevelLogic::ThirdLevelLogic()
: must_show_welcome_image( true )
, show_dh_first_time_show_planet_gameplay_msg( true )

// Variables que necesitan resetearse
, player_win( false )
, time_to_born_powerup( 0.0f )

// XML. Cargadas en el Load, no hay que resetearlas
, init_player_sync( 250.0f )
, syn_player_lose_per_second_on_safe( 0.5f )
, syn_player_lose_per_second_on_damage( 1.0f )
, time_to_show_planet_gameplay_msg( 1.0f )
, max_time_to_born_powerup( 1.0f )
{
	for( unsigned i=0; i<NUM_VALVES; ++i )
		show_dh_first_time_valve_hit[ i ] = true;
}

void ThirdLevelLogic::destroyLevel( ) {
	// Destroy Triggers, Spatial and particles
	Global::triggers->clear( );
	Global::spatial.clear( );
	PSManager::get( )->killAll( );

	// Destroy Third Level Things
	valves.destroy( );
	river.destroy( );
	power_ups.clear( );

	// Reset Variables
	player_win = false;
	time_to_born_powerup = 0.0f;
}

void ThirdLevelLogic::initLevel( ) {
	destroyLevel( );

	// Init syncronization
	player_sync = init_player_sync;

	// Init
	valves.init( );
	river.init( );

	// No player control for a while and dinamic help
	Global::triggers->addUserEvent( "no_player_control_for_a_while" );
	if( show_dh_first_time_show_planet_gameplay_msg ) {
		show_dh_first_time_show_planet_gameplay_msg = false;
		Global::triggers->addLogicTimer( "show_dinamic_help( \"PLANET3_FIRST_MESSAGE\" )", time_to_show_planet_gameplay_msg, LogicTimer::COMMAND );
	}
}

void ThirdLevelLogic::update( float deltaTime ) {
	// El player va perdiendo vida a medida que pasa el tiempo
	Planet::SurfaceType surface_type = BrainManager::get( )->getCurrentBrain( ).getSurfaceType( Global::player->loc.getAlpha( ), Global::player->loc.getDelta( ) );
	if( surface_type == Planet::DAMAGE ) {
		playerSyncDown( deltaTime * syn_player_lose_per_second_on_damage );
		Global::lua_vm.executeCmd( "show_player_hurt()" );
	}
	else
		playerSyncDown( deltaTime * syn_player_lose_per_second_on_safe );

	// PowerUps
	time_to_born_powerup += deltaTime;
	if( time_to_born_powerup >= max_time_to_born_powerup ) {
		time_to_born_powerup -= max_time_to_born_powerup;
		power_ups.add( 1 );
	}
}

void ThirdLevelLogic::loadLogic( MKeyValue &atts ) {
	init_player_sync                     = atts.getFloat( "init_player_sync", 250.0f );
	syn_player_lose_per_second_on_safe   = atts.getFloat( "syn_player_lose_per_second_on_safe", 0.5f );
	syn_player_lose_per_second_on_damage = atts.getFloat( "syn_player_lose_per_second_on_damage", 1.0f );
	time_to_show_planet_gameplay_msg     = atts.getFloat( "time_to_show_planet_gameplay_msg", 1.0f );

	// Valves
	valves.load( );
	river.load( );

	// Load powerups
	max_time_to_born_powerup                  = atts.getFloat( "max_time_to_born_powerup", 1.0f );
	const std::string &powerups_positions_xml = atts[ "powerups_positions_xml" ];
	const std::string &pathToXML              = Global::data_locator.getPath( "xml" );
	power_ups.xmlParseFile( pathToXML + powerups_positions_xml );
}

void ThirdLevelLogic::resetDinamicHelp( ) {
	must_show_welcome_image = true;
	show_dh_first_time_show_planet_gameplay_msg = true;
	for( unsigned i=0; i<NUM_VALVES; ++i )
		show_dh_first_time_valve_hit[ i ] = true;
}

/*
void ThirdLevelLogic::addInitialTriggers( ) const {
	size_t idx = 0;

	const LogicTimerManager &logic_timer_manager = initial_triggers.getLogicTimerManager( );
	LogicTimerManager::const_iterator itTM = logic_timer_manager.begin( );
	while( itTM != logic_timer_manager.end( ) ) {
		assert( *itTM );
		LogicTimer *new_logic_timer = new LogicTimer( *(*itTM) );
		assert( new_logic_timer );
		
		// Set Listener si hace falta
		//new_logic_timer->registerToPlayerListeners( LISTENER );

		Global::triggers->addLogicTimer( new_logic_timer );

		++itTM;
		++idx;
	}

	idx = 0;

	const LogicVolumeManager &logic_volume_manager = initial_triggers.getLogicVolumeManager( );
	LogicVolumeManager::const_iterator itVM = logic_volume_manager.begin( );
	while( itVM != logic_volume_manager.end( ) ) {
		assert( *itVM );
		LogicVolume *new_logic_volume = NULL;

		const char *logic_volume_type = (*itVM)->getType( );
		if( strcmp( logic_volume_type, "LogicSphere" ) == 0 ) {
			new_logic_volume = new LogicSphere( *((const LogicSphere *)*itVM) );
			assert( new_logic_volume );

			// Fix min and max points with planet transform
			Planet &curr_brain = BrainManager::get( )->getCurrentBrain( );
			const D3DXMATRIX &brain_transform = BrainManager::get( )->getCurrentBrain( ).loc.getTransform( );
			const D3DXVECTOR3 &center = ((LogicSphere *)new_logic_volume)->getCenter( );
			D3DXVECTOR3 new_center;
			D3DXVec3TransformCoord(&new_center, &center, &brain_transform); // New center with brain transform

			// Position the volume on brain surface
			float alpha, delta;
			curr_brain.getPolarAngles( new_center, alpha, delta );
			curr_brain.getPointOnPlanet( new_center, alpha, delta );
			((LogicSphere *)new_logic_volume)->setCenter( new_center );
		}
		else if( strcmp( logic_volume_type, "LogicBox" ) == 0 ) {
			new_logic_volume = new LogicBox( *((const LogicBox *)*itVM) );
			assert( new_logic_volume );

			// Fix center with planet transform
			const D3DXMATRIX &brain_transform = BrainManager::get( )->getCurrentBrain( ).loc.getTransform( );
			D3DXVECTOR3 &min_point = ((LogicBox *)new_logic_volume)->min_point;
			D3DXVECTOR3 &max_point = ((LogicBox *)new_logic_volume)->max_point;
			D3DXVec3TransformCoord(&min_point, &min_point, &brain_transform); // New min point with brain transform
			D3DXVec3TransformCoord(&max_point, &max_point, &brain_transform); // New max point with brain transform
			
			// Position the volume on brain surface
			// NOTE: LOS LOGICBOX NO SE VAN A PONER SOBRE LA SUPERFICIE, PONERLAS EN EL XML BIEN DONDE SE QUIERA
		}
		else {
			assert( !"This should not happend" );
			++itVM;
			++idx;
			continue;
		}

		// Set Listener si hace falta
		//new_logic_volume->registerToPlayerListeners( LISTENER );

		Global::triggers->addLogicVolume( new_logic_volume );

		++itVM;
		++idx;
	}
}
*/
