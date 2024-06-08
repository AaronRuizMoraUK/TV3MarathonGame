#include "FourthLevelLogic.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Particles/PSManager.h"
#include "Particles/ParticleGroup.h"
#include "Managers/BrainManager.h"
#include "Util/Tracker/Tracker.h"
#include "GameState/IGameState.h"
#include "Bass/SoundPlayer.h"

FourthLevelLogic::FourthLevelLogic()
: must_show_welcome_image( true )
, show_dh_first_time_show_planet_gameplay_msg( true )
, show_dh_first_time_player_is_hit_by_twister( true )
, show_dh_first_time_expender_hit( true )
, show_dh_first_time_inside_interval_to_win( true )
, show_dh_first_time_outside_interval_to_win( true )

// Variables que necesitan resetearse
, time_twister_born( 0.0f )
, time_to_born_powerup( 0.0f )
, dopamina_level( 0.0f )
, noradrenalina_level( 0.0f )
, player_win( false )
, inside_interval_to_win( false )
, portal_particle( NULL )

// XML. Cargadas en el Load, no hay que resetearlas
, init_player_sync( 250.0f )
, init_num_dopaminas( 10 )
, init_num_noradrenalinas( 10 )
, init_num_twisters( 10 )
, min_num_dopaminas( 10 )
, max_num_dopaminas( 40 )
, min_num_noradrenalinas( 10 )
, max_num_noradrenalinas( 40 )
, max_num_twisters( 20 )
, max_time_twister_born( 10.0f )
, dopamina_start_right_percentage( 0.70f )
, dopamina_end_right_percentage( 0.75f )
, noradrenalina_start_right_percentage( 0.70f )
, noradrenalina_end_right_percentage( 0.75f )
, portal_particle_name( "" )
, syn_player_lose_per_second( 0.5f )
, time_to_show_planet_gameplay_msg( 1.0f )
, max_time_to_born_powerup( 1.0f )
, portal_radius( 1.0f )
{
}

void FourthLevelLogic::destroyLevel( ) {
	// Destroy Triggers, Spatial and particles
	Global::triggers->clear( );
	Global::spatial.clear( );
	PSManager::get( )->killAll( );

	// Destroy Fourth Level Things
	dopaminas.clear( );
	noradrenalinas.clear( );
	twisters.clear( );
	expenders.destroy( );
	power_ups.clear( );

	// Reset Variables
	time_twister_born = 0.0f;
	time_to_born_powerup = 0.0f;

	dopamina_level = 0.0f;
	noradrenalina_level = 0.0f;

	player_win = false;
	
	inside_interval_to_win = false;
	portal_particle = NULL;
}

void FourthLevelLogic::initLevel( ) {
	destroyLevel( );

	// Init syncronization
	player_sync = init_player_sync;

	// Init
	dopaminas.add( init_num_dopaminas, "dopamina_brain4" );
	noradrenalinas.add( init_num_noradrenalinas, "noradrenalina_brain4" );
	twisters.add( init_num_twisters );
	expenders.init( );

	updatePercentages( );

	// No player control for a while and dinamic help
	Global::triggers->addUserEvent( "no_player_control_for_a_while" );
	if( show_dh_first_time_show_planet_gameplay_msg ) {
		show_dh_first_time_show_planet_gameplay_msg = false;
		Global::triggers->addLogicTimer( "show_dinamic_help( \"PLANET4_FIRST_MESSAGE\" )", time_to_show_planet_gameplay_msg, LogicTimer::COMMAND );
	}
}

void FourthLevelLogic::update( float deltaTime ) {
	// El player va perdiendo vida a medida que pasa el tiempo
	playerSyncDown( deltaTime * syn_player_lose_per_second );

	// Born twisters
	time_twister_born += deltaTime;
	if( time_twister_born >= max_time_twister_born ) {
		time_twister_born -= max_time_twister_born;
		if( twisters.size( ) < (size_t)max_num_twisters )
			twisters.add( 1 );
	}

	// PowerUps
	time_to_born_powerup += deltaTime;
	if( time_to_born_powerup >= max_time_to_born_powerup ) {
		time_to_born_powerup -= max_time_to_born_powerup;
		power_ups.add( 1 );
	}

	// Min amount of dopaminas
	if( dopaminas.size( ) < (size_t)min_num_dopaminas )
		dopaminas.add( 1, "dopamina_brain4" );
	
	// Min amount of noradrenalinas
	if( noradrenalinas.size( ) < (size_t)min_num_dopaminas )
		noradrenalinas.add( 1, "noradrenalina_brain4" );

	// Actualizar porcentages
	updatePercentages( );

	// Actualizar estados si entramos/salimos de los porcentajes correctos
	updateRightInterval( );
}

void FourthLevelLogic::updatePercentages( ) {
	normalize( dopamina_level, (float)dopaminas.size( ), 0.0f, 1.0f, (float)min_num_dopaminas, (float)max_num_dopaminas );
	dopamina_level = clamp( dopamina_level, 0.0f, 1.0f );

	normalize( noradrenalina_level, (float)noradrenalinas.size( ), 0.0f, 1.0f, (float)min_num_noradrenalinas, (float)max_num_noradrenalinas );
	noradrenalina_level = clamp( noradrenalina_level, 0.0f, 1.0f );
}

bool FourthLevelLogic::areLevelsInRightPercentages( ) {
	bool dopamina_level_right = dopamina_level <= dopamina_end_right_percentage
		&& dopamina_level >= dopamina_start_right_percentage;

	bool noradrenalina_level_right = noradrenalina_level <= noradrenalina_end_right_percentage
		&& noradrenalina_level >= noradrenalina_start_right_percentage;

	return dopamina_level_right && noradrenalina_level_right;
}

void FourthLevelLogic::updateRightInterval( ) {
	// Si hemos entrado en la zona de ganar
	bool ok_perc = areLevelsInRightPercentages( );
	if( !inside_interval_to_win && ok_perc ) {
		inside_interval_to_win = true;

		// Crear un volumen lógico para ir hacia el para ganar
		D3DXVECTOR3 portal_pos;
		obtainPortalPosition( portal_pos );
		Global::triggers->addLogicVolume( "fourth_level_portal", portal_pos, portal_radius );
		SoundPlayer::get( )->playSample( "new_portal" );

		// Add particle
		float alpha, delta;
		BrainManager::get( )->getCurrentBrain( ).getPolarAngles( portal_pos, alpha, delta );
		D3DXMATRIX portal_transform;
		BrainManager::get( )->getCurrentBrain( ).getTransformOnPlanet( portal_transform, 0, alpha, delta );
		portal_particle = PSManager::get( )->addParticleGroup( portal_particle_name, TTracker( portal_transform ) );

		// Lanzar ayuda dinámica si es la primera vez que entramos en el intervalo bueno
		if( show_dh_first_time_inside_interval_to_win ) {
			show_dh_first_time_inside_interval_to_win = false;
			IGameState::showDinamicHelp( DinamicHelp::PLANET4_ENTER_RIGHT_LEVELS );
		}
	}
	// Si hemos salida de la zona de ganar
	else if( inside_interval_to_win && !ok_perc ) {
		inside_interval_to_win = false;

		// Destruir el volumen lógico para ganar
		Global::triggers->delLogicVolume( "fourth_level_portal" );

		// Destruir particula
		assert( portal_particle );
		portal_particle->kill( true );

		// Lanzar ayuda dinámica si es la primera vez que salimos del intervalo bueno
		if( show_dh_first_time_outside_interval_to_win ) {
			show_dh_first_time_outside_interval_to_win = false;
			IGameState::showDinamicHelp( DinamicHelp::PLANET4_EXIT_RIGHT_LEVELS );
		}
	}
}

void FourthLevelLogic::loadLogic( MKeyValue &atts ) {
	init_player_sync                 = atts.getFloat( "init_player_sync", 250.0f );
	syn_player_lose_per_second       = atts.getFloat( "syn_player_lose_per_second", 1.0f );
	time_to_show_planet_gameplay_msg = atts.getFloat( "time_to_show_planet_gameplay_msg", 1.0f );
	init_num_dopaminas               = atts.getInt( "init_num_dopaminas", 10 );
	init_num_noradrenalinas          = atts.getInt( "init_num_noradrenalinas", 10 );
	init_num_twisters                = atts.getInt( "init_num_twisters", 10 );
	min_num_dopaminas                = atts.getInt( "min_num_dopaminas", 10 );
	max_num_dopaminas                = atts.getInt( "max_num_dopaminas", 40 );
	min_num_noradrenalinas           = atts.getInt( "min_num_noradrenalinas", 10 );
	max_num_noradrenalinas           = atts.getInt( "max_num_noradrenalinas", 40 );
	max_num_twisters                 = atts.getInt( "max_num_twisters", 20 );
	max_time_twister_born            = atts.getFloat( "max_time_twister_born", 10.0f );
	dopamina_start_right_percentage  = atts.getFloat( "dopamina_start_right_percentage", 70.0f ) / 100.0f;
	dopamina_end_right_percentage    = atts.getFloat( "dopamina_end_right_percentage", 75.0f ) / 100.0f;
	noradrenalina_start_right_percentage = atts.getFloat( "noradrenalina_start_right_percentage", 70.0f ) / 100.0f;
	noradrenalina_end_right_percentage   = atts.getFloat( "noradrenalina_end_right_percentage", 75.0f ) / 100.0f;
	portal_particle_name             = atts[ "portal_particle_name" ];
	portal_radius                    = atts.getFloat( "portal_radius", 1.0f );

	assert( init_player_sync > 0.0f );
	assert( syn_player_lose_per_second >= 0.0f );
	assert( time_to_show_planet_gameplay_msg > 0.0f );
	assert( init_num_dopaminas >= 0 );
	assert( init_num_noradrenalinas >= 0 );
	assert( init_num_twisters >= 0 );
	assert( min_num_dopaminas >= 0 );
	assert( max_num_dopaminas > 0 );
	assert( min_num_noradrenalinas >= 0 );
	assert( max_num_noradrenalinas > 0 );
	assert( max_num_twisters > 0 );
	assert( max_time_twister_born > 0.0f );

	assert( min_num_dopaminas <= init_num_dopaminas );
	assert( min_num_dopaminas <  max_num_dopaminas );
	assert( min_num_noradrenalinas <= init_num_noradrenalinas );
	assert( min_num_noradrenalinas <  max_num_noradrenalinas );
	assert( init_num_twisters <= max_num_twisters );

	assert( dopamina_start_right_percentage >= 0.0f && dopamina_start_right_percentage <= 1.0f );
	assert( dopamina_end_right_percentage >= 0.0f && dopamina_end_right_percentage <= 1.0f );
	assert( noradrenalina_start_right_percentage >= 0.0f && noradrenalina_start_right_percentage <= 1.0f );
	assert( noradrenalina_end_right_percentage >= 0.0f && noradrenalina_end_right_percentage <= 1.0f );

	assert( dopamina_start_right_percentage <= dopamina_end_right_percentage );
	assert( noradrenalina_start_right_percentage <= noradrenalina_end_right_percentage );

	assert( portal_radius > 0.0f );

	expenders.load( );

	// Load powerups
	max_time_to_born_powerup                  = atts.getFloat( "max_time_to_born_powerup", 1.0f );
	const std::string &powerups_positions_xml = atts[ "powerups_positions_xml" ];
	const std::string &pathToXML              = Global::data_locator.getPath( "xml" );
	power_ups.xmlParseFile( pathToXML + powerups_positions_xml );
}

void FourthLevelLogic::resetDinamicHelp( ) {
	must_show_welcome_image = true;
	show_dh_first_time_show_planet_gameplay_msg = true;
	show_dh_first_time_player_is_hit_by_twister = true;
	show_dh_first_time_expender_hit             = true;
	show_dh_first_time_inside_interval_to_win   = true;
	show_dh_first_time_outside_interval_to_win  = true;
}

void FourthLevelLogic::obtainPortalPosition( D3DXVECTOR3 &out_portal_pos ) {
	static const float portal_separation = 4.5f;

	D3DXVECTOR3 up;
	bool ok = obtainFrontPlayerUpScreen( up );
	if( !ok )
		up = Global::player->loc.getLocalLeft( );
	out_portal_pos = Global::player->loc.getPosition( ) + up * portal_separation;

	float alpha, delta;
	BrainManager::get( )->getCurrentBrain( ).getPolarAngles( out_portal_pos, alpha, delta );
	BrainManager::get( )->getCurrentBrain( ).getPointOnPlanet( out_portal_pos, alpha, delta );
}
