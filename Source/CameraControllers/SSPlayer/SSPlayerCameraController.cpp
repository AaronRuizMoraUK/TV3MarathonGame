#include "SSPlayerCameraController.h"
#include "Global/Rendering.h"
#include "Camera/Camera3D.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"

const float SSPlayerCameraController::low_vib_amount = 0.1f;
const float SSPlayerCameraController::low_vib_speed = 84.0f;
const float SSPlayerCameraController::medium_vib_amount = 0.07f;
const float SSPlayerCameraController::medium_vib_speed = 84.0f;
const float SSPlayerCameraController::high_vib_amount = 0.23f;
const float SSPlayerCameraController::high_vib_speed = 84.0f;

SSPlayerCameraController::SSPlayerCameraController(float pTargetSpeed, float pPositionSpeed, float pDistance)
: targetSpeed(pTargetSpeed)
, positionSpeed(pPositionSpeed)
, distance_player(pDistance)
, vib_intensity( MEDIUM )
, max_vib_time( 0.0f )
, vib_time( 0.0f )
, vib_enabled( false )
{}

SSPlayerCameraController::~SSPlayerCameraController() {
	printDebugString("Destroying SSPlayer camera controller...\n");
};

void SSPlayerCameraController::update(Camera3D &camera, float deltaTime) {

	// Get the last position and target of the camera
	const D3DXVECTOR3 &prevTarget = camera.getTarget();
	const D3DXVECTOR3 &prevPosition = camera.getPosition();

	// Obtain Player position
	const D3DXVECTOR3 &playerPosition = Global::player->loc.getPosition();
	const D3DXVECTOR3 &playerLocalUpVector = Global::player->loc.getLocalUp();

	// Obtain new target and position with inertia
	D3DXVECTOR3 deltaTarget = playerPosition-prevTarget;
	D3DXVECTOR3 deltaPosition = (playerPosition+distance_player*playerLocalUpVector)-prevPosition;

	D3DXVECTOR3 newTarget = prevTarget + deltaTarget*deltaTime*targetSpeed;
	D3DXVECTOR3 newPosition = prevPosition + deltaPosition*deltaTime*positionSpeed;

	// Vibration
	if( vib_enabled )
		vibrateCamera( newTarget, camera, deltaTime );

	setUpVector( camera, deltaTime );

	// Update where camera is looking at
	camera.lookAt(newPosition, newTarget);
}

void SSPlayerCameraController::setUpVector( Camera3D &camera, float deltaTime ) {

	const D3DXVECTOR3 &old_up = camera.getLocalUpVector();

	D3DXVECTOR3 new_up;
	bool ok = obtainFrontPlayerUpScreen( new_up );

	if( ok && distance( new_up, old_up ) > 0.1f ) {
		D3DXVECTOR3 sync_dir = new_up - old_up;
		normalizeVector3( sync_dir, sync_dir );

		static const float sync_speed = 0.1f;
		new_up = old_up + sync_dir * deltaTime * sync_speed;
	}
	else {
		// Si algo fallase, la camara se realizará con el up anterior
		new_up = old_up;
	}

	camera.setUpVector( new_up );
}

void SSPlayerCameraController::vibrateDuring( float seconds, VibrationIntensity avib_intensity ) {
	max_vib_time = seconds;
	vib_intensity = avib_intensity;
	vib_enabled = true;
}

void SSPlayerCameraController::vibrateCamera( D3DXVECTOR3 &new_target, const Camera3D &camera, float deltaTime ) {

	// Update vibration time
	vib_time += deltaTime;
	if( vib_time >= max_vib_time ) {
		vib_time = 0.0f;
		vib_enabled = false;
		return;
	}

	// Select the amount and the speed of vibration
	float vib_amount = 0.0f;
	float vib_speed = 0.0f;
	if( vib_intensity == LOW ) {
		vib_amount = low_vib_amount;
		vib_speed = low_vib_speed;
	}
	else if( vib_intensity == MEDIUM ) {
		vib_amount = medium_vib_amount;
		vib_speed = medium_vib_speed;
	}
	else {
		vib_amount = high_vib_amount;
		vib_speed = high_vib_speed;
	}

	// Apply vibration
	float variation = vib_amount * sinf( vib_speed * vib_time );
	new_target += variation * ( camera.getLocalLeftVector() + camera.getLocalUpVector() +
		camera.getLocalFrontVector() );

	/*
	if( Global::isPushed( '1' ) ) {
	static float amount = 0.2f;
	static float speed = 10.0f;

	if( Global::isPushed( '2' ) )
	amount += 0.002f;
	if( Global::isPushed( '3' ) )
	amount -= 0.002f;
	if( Global::isPushed( '4' ) )
	speed += 0.2f;
	if( Global::isPushed( '5' ) )
	speed -= 0.2f;

	if( amount < 0.0f )
	amount = 0.0f;
	if( speed < 0.0f )
	speed = 0.0f;

	float variation = amount*sinf( speed*Global::time );
	newTarget += variation * ( camera.getLocalLeftVector() + camera.getLocalUpVector() + camera.getLocalFrontVector() );

	if( Global::isPushed( 'Q' ) ) {
	char msg[ 128 ];
	sprintf_s(msg, sizeof(msg), "amount %f - speed %f\n", amount, speed);
	Global::printDebugString(msg);
	}
	}

	if( Global::isPushed( '6' ) ) {
	static float amount = 0.2f;
	static float speed = 10.0f;

	if( amount < 0.0f )
	amount = 0.0f;
	if( speed < 0.0f )
	speed = 0.0f;

	if( Global::isPushed( '7' ) )
	amount += 0.002f;
	if( Global::isPushed( '8' ) )
	amount -= 0.002f;
	if( Global::isPushed( '9' ) )
	speed += 0.1f;
	if( Global::isPushed( '0' ) )
	speed -= 0.1f;

	float variation = amount*sinf( speed*Global::time );
	newPosition += camera.getLocalLeftVector()*variation;
	}
	*/
}
