#include "SphericalController.h"
#include "Object/Location.h"
#include "Global/Geometry.h"
#include "Global/Variables.h"
#include "Global/Rendering.h"
#include "Managers/BrainManager.h"

SphericalController::SphericalController( float atarget_rot_speed, float aacceleration )
: target_rot_speed( atarget_rot_speed )
, angle( 0.0f )
, angle_rotated( 0.0f )
, speed( 0.0f )
, target_speed( 0.0f )
, acceleration( aacceleration )
, direction(0,0,1)
{}

SphericalController::~SphericalController() {
	printDebugString("Destroying SphericalController...\n");
}

/*
float distanceAngles( float angle1, float angle2 ) {
	if( angle1 < 0.0f )
		angle1 += 2.0f*D3DX_PI;
	if( angle2 < 0.0f )
		angle2 += 2.0f*D3DX_PI;

	float diff = fabs( angle2 - angle1 );
	if( diff > D3DX_PI )
		return 2.0f*D3DX_PI - diff;
	else
		return diff;
}
*/

void SphericalController::updateRotation( TLocation &loc, float deltaTime ) {
	// Nothing to rotate?
	if( angle == 0.0f ) {
		angle_rotated = 0.0f;
		return;
	}

	// We already rotated the amount we want?
	if( fabs( angle_rotated - angle ) < 0.01f ) {
		angle = 0.0f;
		angle_rotated = 0.0f;
		return;
	}

	// Amount of angle to rotate
	float amount = deltaTime * target_rot_speed;
	if( angle < 0.0f )
		amount = -amount;

	// Si me he pasado por negativo o por positivo
	float next_step = angle_rotated + amount;
	if( (angle < 0.0f && next_step < angle) || (angle > 0.0f && next_step > angle) )
		amount = angle - angle_rotated;

	// Sumarle la cantidad
	angle_rotated += amount;

	D3DXMATRIX new_transform;
	rotateOnAxis( new_transform, loc.getTransform( ), loc.getLocalUp( ), amount );
	loc.setTransform( new_transform );
}

void SphericalController::updatePosition( TLocation &loc, float deltaTime ) {
	// Obtener la velocidad actual
	float sign = target_speed - speed;
	speed += (sign>0.0f) ? acceleration * deltaTime : -acceleration * deltaTime;
	speed = clamp( speed, 0.0f, target_speed );

	// New position (not a point in the sphere) with inertia
	D3DXVECTOR3 new_position = loc.getPosition() + direction * deltaTime * speed;
	if( new_position == loc.getPosition( ) )
		return;

	// New alpha y delta on planet
	float alpha, delta;
	BrainManager::get( )->getCurrentBrain( ).getPolarAngles( new_position, alpha, delta );
	loc.setAlpha( alpha );
	loc.setDelta( delta );
	BrainManager::get( )->getCurrentBrain( ).getPointOnPlanet( new_position, alpha, delta );

	// Meterle inercia al cambio de altura
	static const float changeHeightSpeed = 20.0f;
	float height_new = BrainManager::get( )->getCurrentBrain( ).getHeight( alpha, delta );
	float height_old = distance( loc.getPosition( ), BrainManager::get( )->getCurrentBrain( ).getCenter( ) );
	D3DXVECTOR3 center_to_new_point = new_position - BrainManager::get( )->getCurrentBrain( ).getCenter( );
	if( D3DXVec3Length( &center_to_new_point ) < 2.0f )
		return;

	normalizeVector3(center_to_new_point, center_to_new_point);
	D3DXVECTOR3 new_point_with_height_old = BrainManager::get( )->getCurrentBrain( ).getCenter( ) + center_to_new_point * height_old;
	new_position = new_point_with_height_old + center_to_new_point*(height_new-height_old)*deltaTime*changeHeightSpeed;

	// Obtain Player's new up vector
	D3DXVECTOR3 newLocalUp;
	BrainManager::get( )->getCurrentBrain( ).getNormal( newLocalUp, alpha, delta );

	// Change to planet normal with an inertia
	static const float changeUpSpeed = 1.3f;
	static const float umbralChangeUp = 0.01f;
	D3DXVECTOR3 directionNewUp = newLocalUp - loc.getLocalUp();

	// Ver si merece la pena cambiar o no la normal
	if( D3DXVec3LengthSq( &directionNewUp ) > umbralChangeUp ) {
		normalizeVector3(directionNewUp, directionNewUp);
		newLocalUp = loc.getLocalUp() + directionNewUp * deltaTime * changeUpSpeed;
		normalizeVector3(newLocalUp, newLocalUp);
	}
	else {
		loc.setPosition( new_position );
		return;
	}

	// Obtain Player's new left vector
	D3DXVECTOR3 newLocalLeft;
	D3DXVec3Cross( &newLocalLeft, &newLocalUp, &loc.getLocalFront( ) );
	normalizeVector3( newLocalLeft, newLocalLeft );

	// Obtain Player's new front vector
	D3DXVECTOR3 newLocalFront;
	D3DXVec3Cross( &newLocalFront, &newLocalLeft, &newLocalUp );

	// Set the new transform matrix
	D3DXMATRIX newTransform;
	D3DXMatrixIdentity( &newTransform );
	newTransform(0,0) = newLocalLeft.x;
	newTransform(0,1) = newLocalLeft.y;
	newTransform(0,2) = newLocalLeft.z;
	newTransform(1,0) = newLocalUp.x;
	newTransform(1,1) = newLocalUp.y;
	newTransform(1,2) = newLocalUp.z;
	newTransform(2,0) = newLocalFront.x;
	newTransform(2,1) = newLocalFront.y;
	newTransform(2,2) = newLocalFront.z;
	newTransform(3,0) = new_position.x;
	newTransform(3,1) = new_position.y;
	newTransform(3,2) = new_position.z;

	// Set new transform matrix and new position
	loc.setTransform( newTransform );
}
