#include "SpherePlayerController.h"
#include "Global/Variables.h"
#include "IO/IOStatus.h"
#include "Object/AnimatedObject/Player/Player.h"
#include "Bass/SoundPlayer.h"
#include "Global/Rendering.h"
#include "Managers/BrainManager.h"
#include "Particles/ParticleGroup.h"
#include "Particles/PSManager.h"

SpherePlayerController::SpherePlayerController(
	float awalk_speed
	, float arun_speed
	, float ahit_speed
	, float arot_speed
	, float aacceleration )
	: spherical_controller( arot_speed, aacceleration )
	, walk_speed( awalk_speed )
	, run_speed( arun_speed )
	, hit_speed( ahit_speed )
	, angle_desired( 0.0f )
	, curr_angle( 0.0f )
	, status( STOPED )
	, prev_status( STOPED )
	, is_player_doing_an_action( false )
	, max_action_time( 0.0f )
	, action_time( 0.0f )
	, signal_kick_sent( false )
	, player_obj( NULL )
	, run_particle( NULL )
{
}

SpherePlayerController::~SpherePlayerController() {
	printDebugString( "Destroying SpherePlayerController...\n" );

	// Eliminar las particulas de correr
	if( run_particle ) {
		run_particle->setTracker( TTracker( run_particle->getTracker().getTransform( ) ) );
		run_particle->kill( true );
		run_particle = NULL;
	}
}

void SpherePlayerController::update( Player &object, float deltaTime ) {
	// Me guardo el objecto por si recibo una señal
	player_obj = &object;

	updateInput( );

	updateActions( object, deltaTime );

	// Eliminar las particulas de correr cuando se deja de correr
	if( status != RUNNING && prev_status == RUNNING ) {
		if( run_particle ) {
			run_particle->kill( true );
			run_particle = NULL;
		}
	}

	// POSICION
	switch( status ) {
		case STOPED: {
			if( prev_status != status ) {
				// Esto se ejecuta sólo una vez, cuando hay cambio de estado
				spherical_controller.setAngle( 0.0f );
				spherical_controller.setTargetSpeed( 0.0f );
				object.getCalAnimModel( )->playCycle( "idle" );
			}

			spherical_controller.setDirection( object.loc.getLocalFront( ) );
			break; }

		case WALKING: {
			if( prev_status != status ) {
				// Esto se ejecuta sólo una vez, cuando hay cambio de estado
				spherical_controller.setTargetSpeed( walk_speed );
				object.getCalAnimModel( )->playCycle( "walk" );
			}

			spherical_controller.setDirection( object.loc.getLocalFront( ) );
			break; }

		case RUNNING: {
			if( prev_status != status ) {
				// Esto se ejecuta sólo una vez, cuando hay cambio de estado
				spherical_controller.setTargetSpeed( run_speed );
				object.getCalAnimModel( )->playCycle( "jog" );

				// Añadir particula de correr
				assert( run_particle == NULL );
				run_particle = PSManager::get( )->addParticleGroup( "player_run", TTracker( &object.loc ) );
			}

			spherical_controller.setDirection( object.loc.getLocalFront( ) );
			break; }

		case HITTED: {
			if( prev_status != status ) {
				// Esto se ejecuta sólo una vez, cuando hay cambio de estado
				spherical_controller.setAngle( 0.0f );
				spherical_controller.setTargetSpeed( hit_speed );
				// El resto de cosas se han hecho al procesar la señal
			}			
			// La direccion ya fue fijada al procesar la señal y nadie la está cambiando
			break; }

		default:
			assert( !"This should not happend!" );
			return;
			break;
	}

	// Update Rotation
	spherical_controller.updateRotation( object.loc, deltaTime );
	updateAndFixCurrentAngle( );

	// Update Position
	spherical_controller.updatePosition( object.loc, deltaTime );

	prev_status = status;
}

void SpherePlayerController::updateInput( ) {
	// Si estoy en ciertos estado, no quiero entrada del input
	if( status == HITTED )
		return;

	// ROTACION Y STATUS
	const D3DXVECTOR2 &input_dir = ioStatus.walk.getDirection( );
	if( D3DXVec2LengthSq( &input_dir ) != 0.0f ) {
		// Set angle to rotate
		angle_desired = -atan2f( input_dir.x, input_dir.y );
		float angle_to_rotate = getAngleToRotateCloserToCurrAngle( );
		spherical_controller.setAngle( angle_to_rotate );

		if( ioStatus.walkSlow.isPressed( ) )
			status = WALKING;
		else
			status = RUNNING;
	}
	else
		status = STOPED;
}

void SpherePlayerController::updateActions( Player &object, float deltaTime ) {
	// Actions
	if( !is_player_doing_an_action ) {

		// Patada
		if( ioStatus.kick.becomesPressed( ) ) {
			action_time = 0.0f;
			is_player_doing_an_action = true;

			// Aqui se empieza a dar la patada visual (sonido, animación, particulas, etc.)
			SoundPlayer::get( )->playSample( "player_kicking" );
			max_action_time = object.getCalAnimModel( )->playAction( "kick" );
			signal_kick_sent = false;
			return;
		}
	}
	else {
		action_time += deltaTime;
		if( action_time >= max_action_time ) {
			is_player_doing_an_action = false;

			// Cuando se ha terminado de recibir la patada, pasamos a estar parados
			if( status == HITTED )
				status = STOPED;
		}
		else {
			if( status != HITTED ) {

				// Llegado el momento de dar la patada "logica"
				if( !signal_kick_sent && action_time > 0.1f ) {
					object.sendSignal( SIGNAL_PLAYER_DO_KICK );
					signal_kick_sent = true;
				}

			}
		}
	}
}

float SpherePlayerController::getAngleToRotateCloserToCurrAngle( ) const {
	// Fix angles
	float currentAngleFixed = curr_angle;
	float newAngleFixed     = angle_desired;
	if( fabs(angle_desired-curr_angle) > D3DX_PI ) {
		if( angle_desired < 0.0f )
			newAngleFixed += 2.0f * D3DX_PI;
		else
			currentAngleFixed += 2.0f * D3DX_PI;
	}
	float angle_to_rotate = newAngleFixed - currentAngleFixed;
	if( fabs(angle_to_rotate) > 0.01f ) // Cuidado con la sensibilidad del giro con el stick
		return angle_to_rotate;
	else
		return 0.0f;
}

void SpherePlayerController::updateAndFixCurrentAngle( ) {

	curr_angle += spherical_controller.getAngleRotated( );
	if( curr_angle <= -D3DX_PI )
		curr_angle += 2.0f * D3DX_PI;
	else if( curr_angle > D3DX_PI )
		curr_angle -= 2.0f * D3DX_PI;
}

void SpherePlayerController::processSignal( TSignal tag, const void *data, size_t num_bytes ) {
	// Si por alguna razon todavia no tengo el objeto inteligente, no proceso señales
	if( player_obj == NULL )
		return;

	switch( tag ) {
		// Recibo una señal de que me han dado un golpe
		case SIGNAL_HIT_PLAYER:
			{
				// Get Data
				assert( data );
				assert( num_bytes == sizeof( D3DXVECTOR3 ) );
				const D3DXVECTOR3 &hit_dir = *((const D3DXVECTOR3 *)data);

				// Direction and statsu
				spherical_controller.setDirection( hit_dir );
				status = HITTED;

				// Action
				action_time = 0.0f;
				is_player_doing_an_action = true;

				SoundPlayer::get( )->playSample( "player_hit" );
				player_obj->getCalAnimModel( )->clearAllActions( 0.1f ); // Quitar las acciones actuales
				max_action_time = player_obj->getCalAnimModel( )->playAction( "hit" );

				Global::lua_vm.executeCmd( "show_player_hurt()" );
			}
			break;

			// El resto de señales no me interesan
		default:
			return;
	}
}

void SpherePlayerController::collision( Player &player, const Object &object, CollisionType collision_type, const D3DXVECTOR3 &normal ) {
	const D3DXMATRIX &prev_transform = player.loc.getPrevTransform( );
	D3DXVECTOR3	prev_position( prev_transform(3,0), prev_transform(3,1), prev_transform(3,2) );

	const D3DXVECTOR3 &position = player.loc.getPosition( );
	D3DXVECTOR3 position_vector = position - prev_position;
	//normalizeVector3(position_vector, position_vector); // DO NOT NORMALIZE
	float position_normal_dot = D3DXVec3Dot(&position_vector, &normal);

	// If previous and current position are differents
	// and position vector and normal dot product is less than 0,
	// then correct position
	if( D3DXVec3Length( &position_vector ) > 0.001f && position_normal_dot < 0.0f ) {
		float e = 0.3f;
		D3DXVECTOR3 movement = (1.0f+e) * position_normal_dot * normal;

		// Obtain new position
		D3DXVECTOR3 new_position = prev_position + ( (position_vector - movement) /*+ additionalMovementInOut*/ );

		float alpha, delta;
		BrainManager::get( )->getCurrentBrain( ).getPolarAngles( new_position, alpha, delta );
		BrainManager::get( )->getCurrentBrain( ).getPointOnPlanet( new_position, alpha, delta );
		player.loc.setAlpha( alpha );
		player.loc.setDelta( delta );
		player.loc.setPosition( new_position );
	}
}
