#ifndef SPHERICAL_CONTROLLER_H
#define SPHERICAL_CONTROLLER_H

#include "Directx/Directx.h"

class TLocation;

// Se compone de 2 partes separadas: ROTA UNOS GRADOS, AVANZAR EN UN DIRECCION
class SphericalController {
protected:
	// ROTA UNOS GRADOS
	float target_rot_speed;
	float angle;
	float angle_rotated;

	// AVANZAR EN UN DIRECCION
	// Variables
	float speed;
	float target_speed;
	float acceleration;  // Only can be positive

	D3DXVECTOR3 direction;

public:
	SphericalController( float atarget_rot_speed, float aacceleration );
	~SphericalController();

	// ROTA UNOS GRADOS
	void setAngle( float aangle ) { angle=aangle; angle_rotated=0.0f; } // (-PI, PI]. Desde el front hacia la izq positivo.
	void updateRotation( TLocation &loc, float deltaTime );

	// AVANZAR EN UN DIRECCION
	void setDirection( const D3DXVECTOR3 &adirection ) { direction = adirection; }
	void setTargetSpeed( float atarget_speed ) { target_speed = atarget_speed; }
	void updatePosition( TLocation &loc, float deltaTime );

	// Esta funcion la necesita sólo el sphere player controller
	float getAngleRotated( ) const { return angle_rotated; }

	inline float getCurrentSpeed() const {return speed;}
};

#endif // SPHERICAL_CONTROLLER_H
