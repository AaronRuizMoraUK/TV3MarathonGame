#include <cassert>
#include "ValveController.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "SoundPlayer/SoundPlayer.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "Logic/ThirdLevelLogic.h"
#include "Particles/PSManager.h"
#include "Particles/ParticleGroup.h"
#include "AnimationControllers/ROA/ROAObject.h"
#include "Managers/ROAResourceManager.h"
#include "Managers/BrainManager.h"

const char *ValveController::status_chars[ NUM_STATUS ] = {
	"UNDEF"
	, "LOW"
	, "MEDIUM"
	, "HIGH"
};

ValveController::Status ValveController::getStatusFromText( const std::string &status_text ) {
	for( size_t i=0; i<NUM_STATUS; ++i ) {
		if( status_text == status_chars[ i ] )
			return (Status)i;
	}

	return UNDEF;
}

ValveController::ValveController( size_t anum_valve, Status ainitial_status, Status agood_status, float atime_to_desynchronize, float akick_radius, int apoints_score_up )
: initial_status( ainitial_status )
, good_status( agood_status )
, time_to_desynchronize( atime_to_desynchronize )
, kick_radius( akick_radius )
, points_score_up( apoints_score_up )
, status( ainitial_status )
, prev_status( ainitial_status )
, time( 0.0f )
, good_status_particles( NULL )
, int_obj( NULL )
, num_valve( anum_valve )
, roa_resource( NULL )
, initial_position( 0,0,0 )
, initial_rotation( 0,0,0,1 )
{
	D3DXMatrixIdentity( &initial_transform );
}

ValveController::~ValveController() {
	printDebugString( "Destroying ValveController...\n" );
	// NO DESTRUIR LA PARTICULA, YA QUE AQUI EL PUNTERO YA ESTARA MAL
}

void ValveController::reset( IntelligentObject &object ) {
	status                = initial_status;
	prev_status           = initial_status;
	time                  = 0.0f;
	good_status_particles = NULL;
	// NO DESTRUIR LA PARTICULA, YA QUE AQUI EL PUNTERO YA ESTARA, la logica ya habrá liberado las particulas
	changeTexture( object );
}

bool ValveController::update(IntelligentObject &object, float deltaTime) {
	// Me guardo el objecto por si recibo una señal
	int_obj = &object;

	// Si estamos en el estado bueno hay un tiempo para 
	if( status == good_status ) {
		time += deltaTime;
		if( time >= time_to_desynchronize ) {
			// Se nos acabo el tiempo en esta valvula
			SoundPlayer::get( )->playSample( "lever_deactivated" );
			nextStatus( );
		}
	}

	return false;
}

void ValveController::processSignal( TSignal tag, const void *data, size_t num_bytes ) {
	// Si por alguna razon todavia no tengo el objeto inteligente, no proceso señales
	if( int_obj == NULL )
		return;

	switch( tag ) {
		// Recibo una señal de que el player ha dado una patada
		case SIGNAL_PLAYER_DO_KICK:
			{
				bool player_kick_valve = hasPlayerKickValve( );
				if( !player_kick_valve )
					return;

				// Ayuda dinámica
				if( getThirdLevelLogic().show_dh_first_time_valve_hit[ num_valve ] ) {
					getThirdLevelLogic().show_dh_first_time_valve_hit[ num_valve ] = false;
					Global::lua_vm.executeCmd( "show_dinamic_help( \"PLANET3_VALVE%d_HIT\" )", num_valve+1 );
				}

				// Lanzar sonido y actualizar la lógica
				SoundPlayer::get( )->playSample( "kick_lever" );
				nextStatus( );
			}
			break;

			// El resto de señales no me interesan
		default:
			return;
	}
}

void ValveController::nextStatus( ) {
	// Siguiente estado
	prev_status = status;
	status = (Status)( ( status + 1 ) % NUM_STATUS );
	if( status == UNDEF )
		status = LOW;

	// Particles and logic
	if( status == good_status ) {
		SoundPlayer::get( )->playSample( "lever_activated" );

		assert( good_status_particles == NULL );
		good_status_particles = PSManager::get( )->addParticleGroup( "valve_hit", TTracker( initial_transform ) );

		//Logic::scoreUp( points_score_up );
		getThirdLevelLogic( ).river.nextRiverSpeed( );
	}
	else {
		if( prev_status == good_status )
			getThirdLevelLogic( ).river.prevRiverSpeed( );

		time = 0.0f;
		if( good_status_particles ) {
			good_status_particles->kill( true );
			good_status_particles = NULL;
		}
	}

	changeTexture( *int_obj );

	// Lanzar ROA si no existe ya o ya ha acabado
	IAnimationController *roa = int_obj->getAnimationController( );
	if( roa == NULL || roa->isFinished( ) ) {
		if( roa_resource == NULL ) {
			roa_resource = ROAResourceManager::get( )->getResource( "valve_hit.xml" );

			initial_position.x = initial_transform(3,0);
			initial_position.y = initial_transform(3,1);
			initial_position.z = initial_transform(3,2);
			D3DXQuaternionRotationMatrix( &initial_rotation, &initial_transform );
		}

		IAnimationController *new_roa = new ROAObject( &roa_resource->rigidAnimationResource, initial_position, initial_rotation );
		int_obj->setAnimationController( new_roa );
	}
}

void ValveController::changeTexture( IntelligentObject &object ) {
	// Change texture
	switch( status ) {
		case LOW:
			object.CHANGETEXTURE( "valve_green.dds" );
			break;

		case MEDIUM:
			object.CHANGETEXTURE( "valve_yellow.dds" );
			break;

		case HIGH:
			object.CHANGETEXTURE( "valve_red.dds" );
			break;

		default:
			assert( !"This should not happend!" );
			return;
	}
}

bool ValveController::hasPlayerKickValve( ) const {
	assert( int_obj );

	const D3DXVECTOR3 &playerPosition = Global::player->loc.getPosition();
	const D3DXVECTOR3 &valvePosition = int_obj->loc.getPosition();

	if( valvePosition == playerPosition )
		return true;

	D3DXVECTOR3 player_valve_vector = valvePosition - playerPosition;
	normalizeVector3( player_valve_vector, player_valve_vector );
	float cos_angle = D3DXVec3Dot( &player_valve_vector, &Global::player->loc.getLocalFront( ) );
	float angle = acosf( cos_angle );
	if( fabs( angle ) > D3DX_PI/2.0f )
		return false;

	float player_distance = distance(playerPosition, valvePosition);
	return player_distance <= kick_radius;
}
