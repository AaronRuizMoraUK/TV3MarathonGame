#include <cassert>
#include "ExpenderController.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "SoundPlayer/SoundPlayer.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "Particles/ParticleGroup.h"
#include "Particles/PSManager.h"
#include "AnimationControllers/ROA/ROAObject.h"
#include "Managers/ROAResourceManager.h"
#include "Logic/FourthLevelLogic.h"
#include "GameState/IGameState.h"

ExpenderController::ExpenderController( const std::string &aenemy_type, int anum_enemies_can_expel, float amax_recover_time, float akick_radius, int apoints_score_up )
: enemy_type( aenemy_type )
, num_enemies_can_expel( anum_enemies_can_expel )
, max_recover_time( amax_recover_time )
, kick_radius( akick_radius )
, points_score_up( apoints_score_up )
, enemies_counter( 0 )
, recover_time( 0.0f )
, overheated( false )
, overheated_particles( NULL )
, int_obj( NULL )
, roa_resource( NULL )
, initial_position( 0,0,0 )
, initial_rotation( 0,0,0,1 )
{
	D3DXMatrixIdentity( &initial_transform );
}

ExpenderController::~ExpenderController() {
	printDebugString( "Destroying ExpenderController...\n" );
	// NO DESTRUIR LA PARTICULA, YA QUE AQUI EL PUNTERO YA ESTARA MAL
}

void ExpenderController::reset( IntelligentObject &object ) {
	enemies_counter       = 0;
	recover_time          = 0.0f;
	overheated            = false;
	overheated_particles  = NULL;
	// NO DESTRUIR LA PARTICULA, YA QUE AQUI EL PUNTERO YA ESTARA, la logica ya habrá liberado las particulas
	changeTexture( object );
}

bool ExpenderController::update(IntelligentObject &object, float deltaTime) {
	// Me guardo el objecto por si recibo una señal
	int_obj = &object;

	if( overheated ) {
		recover_time += deltaTime;
		if( recover_time >= max_recover_time ) {
			// Expender deja de estar overheated
			enemies_counter = 0;
			recover_time = 0.0f;
			overheated = false;

			if( overheated_particles ) {
				overheated_particles->kill( true );
				overheated_particles = NULL;
			}

			SoundPlayer::get( )->playSample( "lever_activated" );
			changeTexture( object );
		}
	}

	return false;
}

void ExpenderController::processSignal( TSignal tag, const void *data, size_t num_bytes ) {
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
				if( getFourthLevelLogic().show_dh_first_time_expender_hit ) {
					getFourthLevelLogic().show_dh_first_time_expender_hit = false;
					IGameState::showDinamicHelp( DinamicHelp::PLANET4_EXPENDER_HIT );
				}

				// Lanzar sonido y actualizar la lógica
				SoundPlayer::get( )->playSample( "kick_lever" );
				expenderKicked( );
			}
			break;

			// El resto de señales no me interesan
		default:
			return;
	}
}

void ExpenderController::expenderKicked( ) {
	assert( int_obj );

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

	if( overheated ) {
		SoundPlayer::get( )->playSample( "lever4brain_deactivated" ); // Sound of not working
		return;
	}

	//Logic::scoreUp( points_score_up );
	SoundPlayer::get( )->playSample( "lever4brain_activated" ); // Sound of working

	// Que nazca un enemigo
	if( enemy_type == "dopamina_brain4" ) {
		if( getFourthLevelLogic( ).dopaminas.size( ) < (size_t)getFourthLevelLogic( ).getMaxNumDopaminas( ) )
			getFourthLevelLogic( ).dopaminas.add( 1, enemy_type.c_str( ) );
	}
	else if( enemy_type == "noradrenalina_brain4" ) {
		if( getFourthLevelLogic( ).noradrenalinas.size( ) < (size_t)getFourthLevelLogic( ).getMaxNumNoradrenalinas( ) )
			getFourthLevelLogic( ).noradrenalinas.add( 1, enemy_type.c_str( ) );
	}
	else {
		assert( !"This should not happend" );
	}
	++enemies_counter;

	changeTexture( *int_obj );
}

void ExpenderController::changeTexture( IntelligentObject &object ) {
	// Change texture
	if( enemies_counter < num_enemies_can_expel / 2 ) {
		const char *new_texture = (enemy_type=="dopamina_brain4") ? "expender_dopamina_green.dds" : "expender_noradrenalina_green.dds";
		object.CHANGETEXTURE( new_texture );
	}
	else if( enemies_counter >= num_enemies_can_expel && !overheated ) {
		const char *new_texture = (enemy_type=="dopamina_brain4") ? "expender_dopamina_red.dds" : "expender_noradrenalina_red.dds";
		object.CHANGETEXTURE( new_texture );

		// Overheated
		overheated = true;
		SoundPlayer::get( )->playSample( "lever_deactivated" );
		overheated_particles = PSManager::get( )->addParticleGroup( "expender_overheat", TTracker( initial_transform ) );
	}
	else if( !overheated ) {
		const char *new_texture = (enemy_type=="dopamina_brain4") ? "expender_dopamina_yellow.dds" : "expender_noradrenalina_yellow.dds";
		object.CHANGETEXTURE( new_texture );
	}
}

bool ExpenderController::hasPlayerKickValve( ) const {
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
