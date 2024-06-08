#ifndef PLANE_CONTROLLER_H
#define PLANE_CONTROLLER_H

#include <vector>
#include "Directx/Directx.h"
#include "PlayerControllers/IPlayerController.h"

class TLocation;

// Se compone de 2 partes separadas: ROTA UNOS GRADOS, AVANZAR EN UN DIRECCION
class XZController {
protected:
	// ROTA UNOS GRADOS
	float target_rot_speed;
	float angle;

	// AVANZAR EN UN DIRECCION
	// Variables
	float speed;
	float target_speed;
	float acceleration;  // Only can be positive

	D3DXVECTOR3 direction;

public:
	XZController( )
		: target_rot_speed( 0.0f )
		, angle( 0.0f )
		, speed( 0.0f )
		, target_speed( 0.0f )
		, acceleration( 0.0f )
		, direction(0,0,1)
	{}

	// ROTA UNOS GRADOS
	void setTargetRotSpeed( float atarget_rot_speed ) { target_rot_speed = atarget_rot_speed; }
	void setAngle( float aangle ) { angle = aangle; }
	void updateRotation( TLocation &loc, float deltaTime );

	// AVANZAR EN UN DIRECCION
	void setDirection( const D3DXVECTOR3 &adirection ) { direction = adirection; }
	void setTargetSpeed( float atarget_speed ) { target_speed = atarget_speed; }
	void setAcceleration( float aacceleration ) { acceleration = aacceleration; }
	void updatePosition( TLocation &loc, float deltaTime );
};

class PlaneController : public IPlayerController
{
protected:
	XZController xz_controller;

	// XML
	const float walk_speed;
	const float run_speed;

	const float rot_speed;
	const float acceleration; // Only can be positive

	// Status
	enum Status {
		STOPED = 0
		, WALKING
		, RUNNING
	};
	Status status;
	Status prev_status;

public:
	PlaneController();
	~PlaneController();

	const char *getControllerType () const { return "PlaneController"; }

	void update(Player &object, float deltaTime);
};

#endif //PLANE_CONTROLLER_H
