#include "FirstLevelLogic.h"
#include "Global/Rendering.h"
#include "Global/Geometry.h"
#include "Global/Variables.h"
#include "XMLParser/XMLParser.h"
#include "Particles/PSManager.h"
#include "Particles/ParticleGroup.h"
#include "Util/Tracker/Tracker.h"
#include "Managers/BrainManager.h"
#include "GameState/IGameState.h"
#include "SoundPlayer/SoundPlayer.h"

FirstLevelLogic::FirstLevelLogic()
: must_show_welcome_image( true )
, show_dh_first_time_inside_interval_to_win( true )
, show_dh_first_time_outside_interval_to_win( true )
, show_dh_first_time_player_kick_dopamina( true )
, show_dh_first_time_player_is_hit_by_alucinacion( true )
, show_dh_first_time_show_planet_gameplay_msg( true )

// Variables que necesitan resetearse
, dopamina_excess( 0.0f )
, prev_dopamina_excess( 0.0f )
, time_dopamina_born( 0.0f )
, time_to_born_powerup( 0.0f )
, player_win( false )
, inside_interval_to_win( false )
, portal_particle( NULL )

// XML. Cargadas en el Load, no hay que resetearlas
, init_player_sync( 250.0f )
, init_num_alucinaciones( 1 )
, init_num_dopaminas( 40 )
, min_num_dopaminas( 20 )
, max_num_dopaminas( 60 )
, max_num_alucinaciones( 5 )
, max_time_dopamina_born( 3.0f )
, syn_player_lose_per_second( 0.5f )
, start_right_percentage( 0.2f )
, end_right_percentage( 0.25f )
, portal_particle_name( "" )
, time_to_show_planet_gameplay_msg( 1.0f )
, max_time_to_born_powerup( 1.0f )
, portal_radius( 1.0f )
{
}

void FirstLevelLogic::destroyLevel( ) {
	// Destroy Triggers, Spatial and particles
	Global::triggers->clear( );
	Global::spatial.clear( );
	PSManager::get( )->killAll( );

	// Destroy First Level Things
	dopaminas.clear( );
	alucinaciones.clear( );
	power_ups.clear( );

	// Reset Variables
	dopamina_excess = 0.0f;
	prev_dopamina_excess = 0.0f;
	time_dopamina_born = 0.0f;
	time_to_born_powerup = 0.0f;

	player_win = false;

	inside_interval_to_win = false;
	portal_particle = NULL;
}

void FirstLevelLogic::initLevel( ) {
	destroyLevel( );

	// Init syncronization
	player_sync = init_player_sync;

	// Init enemies
	dopaminas.add( init_num_dopaminas );
	alucinaciones.add( init_num_alucinaciones );

	updateDopaminaExcess( );

	// No player control for a while and dinamic help
	Global::triggers->addUserEvent( "no_player_control_for_a_while" );
	if( show_dh_first_time_show_planet_gameplay_msg ) {
		show_dh_first_time_show_planet_gameplay_msg = false;
		Global::triggers->addLogicTimer( "show_dinamic_help( \"PLANET1_FIRST_MESSAGE\" )", time_to_show_planet_gameplay_msg, LogicTimer::COMMAND );
	}
}

void FirstLevelLogic::update( float deltaTime ) {
	// El player va perdiendo vida a medida que pasa el tiempo
	playerSyncDown( deltaTime * syn_player_lose_per_second );

	// Born dopamina (no pueden nacer más bicho si ya he llegado al 100%
	if( dopamina_excess < 1.0f ) {
		// Siempre tiene que haber un minimo de dopaminas en pantalla (min_num_dopaminas).
		if( (int)dopaminas.size( ) < min_num_dopaminas ) {
			dopaminas.add( min_num_dopaminas - (int)dopaminas.size( ) );
			time_dopamina_born = 0.0f;
		}

		time_dopamina_born += deltaTime;
		if( time_dopamina_born >= max_time_dopamina_born ) {
			time_dopamina_born -= max_time_dopamina_born;
			dopaminas.add( 1 );
		}
	}

	// PowerUps
	time_to_born_powerup += deltaTime;
	if( time_to_born_powerup >= max_time_to_born_powerup ) {
		time_to_born_powerup -= max_time_to_born_powerup;
		power_ups.add( 1 );
	}

	// Actualizar la barra de exceso de dopamina
	updateDopaminaExcess( );

	// Actualizar estados si entramos/salimos de los porcentajes correctos
	updateRightInterval( );

	// Born alucinaciones
	if( mustBornAlucinacion( ) )
		alucinaciones.add( 2 );
}

bool FirstLevelLogic::mustBornAlucinacion( ) const {
	static const float dopamina_to_born_alucinacion = 0.6f;
	static const int dopamina_separation_to_born_alucinacion = 5;

	if( dopamina_excess < dopamina_to_born_alucinacion )
		return false;
	else if( prev_dopamina_excess < dopamina_to_born_alucinacion ) // Este es el caso de justo el 60%
		return alucinaciones.size() < max_num_alucinaciones;
	else if( dopamina_excess == 1.0f && prev_dopamina_excess != 1.0f ) // este es el caso del 100%
		return alucinaciones.size() < max_num_alucinaciones;

	int temp1 = (int) ((prev_dopamina_excess - dopamina_to_born_alucinacion) * 100.0f);
	int temp2 = (int) ((dopamina_excess      - dopamina_to_born_alucinacion) * 100.0f);
	temp1 /= dopamina_separation_to_born_alucinacion;
	temp2 /= dopamina_separation_to_born_alucinacion;

	if( temp1 < temp2 )
		return alucinaciones.size() < max_num_alucinaciones;
	else
		return false;
}

bool FirstLevelLogic::isDopaminaExcessInRightPercentages( ) const {
	return dopamina_excess <= end_right_percentage && dopamina_excess >= start_right_percentage;
}

void FirstLevelLogic::updateDopaminaExcess( ) {
	prev_dopamina_excess = dopamina_excess;

	normalize( dopamina_excess, (float)dopaminas.size( ), 0.0f, 1.0f, (float)min_num_dopaminas, (float)max_num_dopaminas );
	dopamina_excess = clamp( dopamina_excess, 0.0f, 1.0f );
}

void FirstLevelLogic::updateRightInterval( ) {
	// Si hemos entrado en la zona de ganar
	bool ok_perc = isDopaminaExcessInRightPercentages( );
	if( !inside_interval_to_win && ok_perc ) {
		inside_interval_to_win = true;

		// Resetear el tiempo con el que nacen las dopaminas
		// para evitar que enseguida se salga de los porcentajes correctos
		time_dopamina_born = 0.0f;

		// Crear un volumen lógico para ir hacia el para ganar
		D3DXVECTOR3 portal_pos;
		obtainPortalPosition( portal_pos );
		Global::triggers->addLogicVolume( "first_level_portal", portal_pos, portal_radius );
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
			IGameState::showDinamicHelp( DinamicHelp::PLANET1_ENTER_RIGHT_DOP_LEVEL );
		}
	}
	// Si hemos salida de la zona de ganar
	else if( inside_interval_to_win && !ok_perc ) {
		inside_interval_to_win = false;

		// Resetear el tiempo con el que nacen las dopaminas
		// para evitar que enseguida entre otra vez de los porcentajes correctos
		time_dopamina_born = 0.0f;

		// Destruir el volumen lógico para ganar
		Global::triggers->delLogicVolume( "first_level_portal" );

		// Destruir particula
		assert( portal_particle );
		portal_particle->kill( true );

		// Lanzar ayuda dinámica si es la primera vez que salimos del intervalo bueno
		if( show_dh_first_time_outside_interval_to_win ) {
			show_dh_first_time_outside_interval_to_win = false;
			IGameState::showDinamicHelp( DinamicHelp::PLANET1_EXIT_RIGHT_DOP_LEVEL );
		}
	}
}

void FirstLevelLogic::obtainPortalPosition( D3DXVECTOR3 &out_portal_pos ) {
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

void FirstLevelLogic::loadLogic( MKeyValue &atts ) {
	init_player_sync                 = atts.getFloat( "init_player_sync", 250.0f );
	init_num_alucinaciones           = atts.getInt( "init_num_alucinaciones", 1 );
	max_num_alucinaciones            = atts.getInt( "max_num_alucinaciones", 5 );
	init_num_dopaminas               = atts.getInt( "init_num_dopaminas", 40 );
	min_num_dopaminas                = atts.getInt( "min_num_dopaminas", 20 );
	max_num_dopaminas                = atts.getInt( "max_num_dopaminas", 60 );
	max_time_dopamina_born           = atts.getFloat( "max_time_dopamina_born", 3.0f );
	syn_player_lose_per_second       = atts.getFloat( "syn_player_lose_per_second", 1.0f );
	start_right_percentage           = atts.getFloat( "start_right_percentage", 20.0f ) / 100.0f;
	end_right_percentage             = atts.getFloat( "end_right_percentage", 25.0f ) / 100.0f;
	portal_particle_name             = atts[ "portal_particle_name" ];
	time_to_show_planet_gameplay_msg = atts.getFloat( "time_to_show_planet_gameplay_msg", 1.0f );
	portal_radius                    = atts.getFloat( "portal_radius", 1.0f );

	assert( portal_radius > 0.0f );

	// Load powerups
	max_time_to_born_powerup                  = atts.getFloat( "max_time_to_born_powerup", 1.0f );
	const std::string &powerups_positions_xml = atts[ "powerups_positions_xml" ];
	const std::string &pathToXML              = Global::data_locator.getPath( "xml" );
	power_ups.xmlParseFile( pathToXML + powerups_positions_xml );
}

void FirstLevelLogic::resetDinamicHelp( ) {
	must_show_welcome_image                         = true;
	show_dh_first_time_inside_interval_to_win       = true;
	show_dh_first_time_outside_interval_to_win      = true;
	show_dh_first_time_player_kick_dopamina         = true;
	show_dh_first_time_player_is_hit_by_alucinacion = true;
	show_dh_first_time_show_planet_gameplay_msg     = true;
}
