#include "TwisterChase.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "Bass/SoundPlayer.h"
#include "Logic/Logic.h"
#include "Logic/FourthLevelLogic.h"
#include "GameState/IGameState.h"
#include "Managers/BrainManager.h"

TwisterChase::TwisterChase(
	float aradius_chase
	, float aradius_touch_player
	, float awalk_speed
	, float arun_speed
	, float amax_think_time
	, float arot_speed
	, float aacceleration
	, float apoints_hit_player )
: spherical_controller( arot_speed, aacceleration )
, radius_chase( aradius_chase )
, radius_touch_player( aradius_touch_player )
, walk_speed( awalk_speed )
, run_speed( arun_speed )
, points_hit_player( apoints_hit_player )
, max_think_time( amax_think_time )
, curr_max_think_time( 0.0f )
, think_time( 0.0f )
, status( STOPED )
, prev_status( STOPED )
, dead( false )
, sound( 0 )
{
}

TwisterChase::~TwisterChase() {
	printDebugString( "Destroying TwisterChase...\n" );
}


bool TwisterChase::update(IntelligentObject &object, float deltaTime) {

	// Think
	think( object, deltaTime );

	// STATUS
	switch( status ) {
		case STOPED: {
			if( prev_status != status ) {
				// Esto se ejecuta sólo una vez, cuando hay cambio de estado
				spherical_controller.setAngle( 0.0f );
				spherical_controller.setTargetSpeed( 0.0f );
			}

			// Ir de frente
			spherical_controller.setDirection( object.loc.getLocalFront( ) );
			break; }

		case WALKING: {
			if( prev_status != status ) {
				// Esto se ejecuta sólo una vez, cuando hay cambio de estado
				spherical_controller.setTargetSpeed( walk_speed );
			}

			// Ir de frente
			spherical_controller.setDirection( object.loc.getLocalFront( ) );
			break; }

		case HUNTING: {
			if( prev_status != status ) {
				// Esto se ejecuta sólo una vez, cuando hay cambio de estado
				spherical_controller.setTargetSpeed( run_speed );
			}

			// Rotar para llegar al player
			float angle = getRotationToFollowPlayer( object );
			spherical_controller.setAngle( angle );

			// Ir de frente
			spherical_controller.setDirection( object.loc.getLocalFront( ) );
			break; }

		default:
			assert( !"This should not happend!" );
			return false;
			break;
	}

	// ----------------------------------------------------------------

	// Update Rotation
	spherical_controller.updateRotation( object.loc, deltaTime );

	// Update Position
	spherical_controller.updatePosition( object.loc, deltaTime );

	// Comprobar si he golpeado al player
	checkHitPlayer( object );

	prev_status = status;
	return dead;
}

float TwisterChase::getRotationToFollowPlayer( IntelligentObject &object ) const {
	const D3DXVECTOR3 &player_pos = Global::player->loc.getPosition( );
	const D3DXVECTOR3 &obj_pos = object.loc.getPosition( );
	if( player_pos == obj_pos )
		return 0.0f;

	D3DXVECTOR3 dir_to_player = player_pos - obj_pos;
	normalizeVector3( dir_to_player, dir_to_player );

	// Get angle
	float angle = D3DXVec3Dot( &object.loc.getLocalFront( ), &dir_to_player );
	angle = acos( angle );
	if( angle == 0.0f )
		return 0.0f;

	// Get sign of angle with a plane test
	// Ax + By + Cz + D = 0
	// (A,B,C) -> getLocalLeft
	// (x,y,z) -> dir_to_player
	const D3DXVECTOR3 &obj_left = object.loc.getLocalLeft( );
	float sign = obj_left.x * dir_to_player.x + obj_left.y * dir_to_player.y + obj_left.z * dir_to_player.z;

	return ( sign < 0.0f ) ? -angle : angle;
}

void TwisterChase::think( IntelligentObject &object, float deltaTime ) {
	float distance_with_player = distance( object.loc.getPosition( ), Global::player->loc.getPosition( ) );
	bool inside_hunting_radius = distance_with_player < radius_chase;

	// Si estoy HUNTING no hace falta pensar. Lo que hay que hacer es ver si se ha alejado el player.
	if( status == HUNTING && inside_hunting_radius )
		return;

	// Comprobar que el player ha entrado en el radio de persecución
	if( inside_hunting_radius ) {
		// Lanzamos un sonido
		assert( sound == 0 );
		sound = SoundPlayer::get( )->playSample( "twister_hit_player", true );
		status = HUNTING;
		return;
	}

	// Pensar?
	think_time += deltaTime;
	if( think_time >= curr_max_think_time ) {
		think_time -= curr_max_think_time; // No empieza desde 0.0f, el think_time puede que se haya pasado un poco
		curr_max_think_time = obtainRandom( 0.0f, max_think_time );

		// Aqui voy a dejar de perseguir
		if( status == HUNTING ) {
			// Parar el sonido
			SoundPlayer::get( )->stopSample( sound, true );
			sound = 0;
		}

		// Pensar. En este caso sólo es pensar cuánto rotar, y cambiar la velocidad objetivo.
		float random = obtainRandom( 0.0f, 1.0f );
		if( random > 0.1f ) {
			status = WALKING;
			spherical_controller.setAngle( obtainRandom( -D3DX_PI+0.1f, D3DX_PI-0.1f ) );
		}
		else {
			status = STOPED;
		}
	}
}

void TwisterChase::checkHitPlayer( IntelligentObject &object ) {
	// Si no estoy en estado de persiguiendo no compruebo
	if( status != HUNTING )
		return;

	float distance_to_player = distance( object.loc.getPosition( ), Global::player->loc.getPosition( ) );
	if( distance_to_player <= radius_touch_player ) {
		// La alucinacion muere
		dead = true;

		// Parar el sonido
		SoundPlayer::get( )->stopSample( sound, true );
		sound = 0;

		// Le mandamos una señal al player
		D3DXVECTOR3 hit_dir;
		const D3DXVECTOR3 &player_pos = Global::player->loc.getPosition( );
		const D3DXVECTOR3 &obj_pos = object.loc.getPosition( );
		if( player_pos == obj_pos )
			hit_dir = object.loc.getLocalFront( );
		else {
			hit_dir = player_pos - obj_pos;
			normalizeVector3( hit_dir, hit_dir );
		}
		Global::player->processSignal( SIGNAL_HIT_PLAYER, &hit_dir, sizeof( hit_dir ) );

		// Ayuda dinámica
		if( getFourthLevelLogic().show_dh_first_time_player_is_hit_by_twister ) {
			getFourthLevelLogic().show_dh_first_time_player_is_hit_by_twister = false;
			IGameState::showDinamicHelp( DinamicHelp::PLANET4_PLAYER_IS_HIT_BY_TWISTER );
		}

		// Actualizamos la lógica
		Logic::playerSyncDown( points_hit_player );
	}
}

void TwisterChase::collision( IntelligentObject &int_object, const Object &object, CollisionType collision_type, const D3DXVECTOR3 &normal ) {
	const D3DXMATRIX &prev_transform = int_object.loc.getPrevTransform( );
	D3DXVECTOR3	prev_position( prev_transform(3,0), prev_transform(3,1), prev_transform(3,2) );

	const D3DXVECTOR3 &position = int_object.loc.getPosition( );
	D3DXVECTOR3 position_vector = position - prev_position;
	//normalizeVector3(position_vector, position_vector); // DO NOT NORMALIZE
	float position_normal_dot = D3DXVec3Dot(&position_vector, &normal);

	// If previous and current position are differents
	// and position vector and normal dot product is less than 0,
	// then correct position
	if( D3DXVec3Length( &position_vector ) > 0.001f && position_normal_dot < 0.0f ) {
		float e = 0.0f;
		D3DXVECTOR3 movement = (1.0f+e) * position_normal_dot * normal;

		// Obtain new position
		D3DXVECTOR3 new_position = prev_position + ( (position_vector - movement) /*+ additionalMovementInOut*/ );
		
		float alpha, delta;
		BrainManager::get( )->getCurrentBrain( ).getPolarAngles( new_position, alpha, delta );
		BrainManager::get( )->getCurrentBrain( ).getPointOnPlanet( new_position, alpha, delta );
		int_object.loc.setAlpha( alpha );
		int_object.loc.setDelta( delta );
		int_object.loc.setPosition( new_position );
	}
}
