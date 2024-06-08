#include "DopaminaBrain4Controller.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "SoundPlayer/SoundPlayer.h"
#include "Managers/BrainManager.h"
#include "Particles/PSManager.h"
#include "Util/Tracker/Tracker.h"

DopaminaBrain4Controller::DopaminaBrain4Controller(
	Type atype
	, float amax_speed
	, float amax_think_time
	, float arot_speed
	, float aacceleration )
	: type( atype )
	, spherical_controller( arot_speed, aacceleration )
	, max_speed( amax_speed )
	, max_think_time( amax_think_time )
	, curr_max_think_time( 0.0f )
	, think_time( 0.0f )
	, dead( false )
	, int_obj( NULL )
{
}

DopaminaBrain4Controller::~DopaminaBrain4Controller() {
	printDebugString( "Destroying DopaminaBrain4Controller...\n" );
}


bool DopaminaBrain4Controller::update(IntelligentObject &object, float deltaTime) {
	// Me guardo el objecto por si recibo una señal
	int_obj = &object;

	// Pensar?
	think_time += deltaTime;
	if( think_time >= curr_max_think_time ) {
		think_time -= curr_max_think_time; // No empieza desde 0.0f, el think_time puede que se haya pasado un poco
		curr_max_think_time = obtainRandom( 0.0f, max_think_time );

		// Pensar. En este caso sólo es pensar cuánto rotar, y cambiar la velocidad objetivo.
		spherical_controller.setAngle( obtainRandom( -D3DX_PI+0.1f, D3DX_PI-0.1f ) );
		spherical_controller.setTargetSpeed( obtainRandom( max_speed*0.5f, max_speed ) );
	}

	// Siempre ir al frente
	spherical_controller.setDirection( object.loc.getLocalFront( ) );

	// ----------------------------------------------------------------

	// Update Rotation
	spherical_controller.updateRotation( object.loc, deltaTime );

	// Update Position
	spherical_controller.updatePosition( object.loc, deltaTime );

	return dead;
}

void DopaminaBrain4Controller::collision( IntelligentObject &int_object, const Object &object, CollisionType collision_type, const D3DXVECTOR3 &normal ) {
	// Si colisiona con el player, muerte
	if( &object == Global::player ) {

		// Sonido
		int randomDopNum = (int)floor(obtainRandom( 1.0f, 5.5f ));
		randomDopNum = clamp( randomDopNum, 1, 5 );
		char dop[ 128 ];
		sprintf_s(dop, sizeof(dop), "dopamine_hit_received_%d", randomDopNum);
		SoundPlayer::get( )->playSample( dop );

		dead = true;
		return;
	}

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
