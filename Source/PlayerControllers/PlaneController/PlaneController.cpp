#include <cassert>
#include "PlaneController.h"
#include "Global/Rendering.h"
#include "IO/IOStatus.h"
#include "Object/AnimatedObject/Player/Player.h"
#include "Global/Geometry.h"

void XZController::updateRotation( TLocation &loc, float deltaTime ) {
	if( angle == 0.0f )
		return;

	float rot_speed = deltaTime * target_rot_speed;
	if( angle > 0.0f )
		rot_speed = -rot_speed;

	D3DXMATRIX new_transform;
	rotateOnAxis( new_transform, loc.getTransform( ), loc.getLocalUp( ), rot_speed );
	loc.setTransform( new_transform );
}

void XZController::updatePosition( TLocation &loc, float deltaTime ) {
	float sign = target_speed - speed;
	speed += (sign>0.0f) ? acceleration * deltaTime : -acceleration * deltaTime;
	speed = clamp( speed, 0.0f, target_speed );

	D3DXVECTOR3 new_position = loc.getPosition( ) + direction * deltaTime * speed;
	loc.setPosition( new_position );
}

// -------------------------------------------------------------------------------

PlaneController::PlaneController()
: walk_speed( 1.2f )
, run_speed( 4.0f )
, rot_speed( 2.0f )
, acceleration( 20.0f )
, status( STOPED )
, prev_status( STOPED )
{
}

PlaneController::~PlaneController() {
	printDebugString( "Destroying PlaneController...\n" );
}


void PlaneController::update( Player &object, float deltaTime ) {
	static bool simula_empezar = true;
	if( simula_empezar ) {
		simula_empezar = false;
		xz_controller.setTargetRotSpeed( rot_speed );
		xz_controller.setAcceleration( acceleration );
	}


	// ROTACION Y STATUS
	const D3DXVECTOR2 &input_dir = ioStatus.walk.getDirection( );
	if( D3DXVec2LengthSq( &input_dir ) != 0.0f ) {
		xz_controller.setAngle( atan2f( input_dir.x, input_dir.y ) );
		if( ioStatus.walkSlow.isPressed( ) )
			status = WALKING;
		else
			status = RUNNING;
	}
	else
		status = STOPED;

	if( ioStatus.kick.becomesPressed( ) ) {
		object.getCalAnimModel( )->playAction( "kick" );
	}

	// POSICION
	switch( status ) {
		case STOPED: {
			if( prev_status != status ) {
				xz_controller.setAngle( 0.0f );
				xz_controller.setTargetSpeed( 0.0f );
				object.getCalAnimModel( )->playCycle( "idle" );
			}
			xz_controller.updateRotation( object.loc, deltaTime );
			xz_controller.setDirection( object.loc.getLocalFront( ) );
			xz_controller.updatePosition( object.loc, deltaTime );

			break; }

		case WALKING: {
			if( prev_status != status ) {
				xz_controller.setTargetSpeed( walk_speed );
				object.getCalAnimModel( )->playCycle( "walk" );
			}
			xz_controller.updateRotation( object.loc, deltaTime );
			xz_controller.setDirection( object.loc.getLocalFront( ) );
			xz_controller.updatePosition( object.loc, deltaTime );

			break; }

		case RUNNING: {
			if( prev_status != status ) {
				xz_controller.setTargetSpeed( run_speed );
				object.getCalAnimModel( )->playCycle( "jog" );
			}
			xz_controller.updateRotation( object.loc, deltaTime );
			xz_controller.setDirection( object.loc.getLocalFront( ) );
			xz_controller.updatePosition( object.loc, deltaTime );

			break; }

		default:
			assert( !"This should not happend!" );
			return;
			break;
	}

	prev_status = status;
}
