#ifndef ICAMERA_H
#define ICAMERA_H

/**
* Camera interface
*/

#include "Directx/Directx.h"
#include "Object/Location.h"

class ICamera {

protected:
	// View Matrix
	D3DXMATRIX matView;

	// Projection Matrix
	D3DXMATRIX matProj;

	// Parameters of View Matrix
	D3DXVECTOR3 position; // Position of the camera in the world
	D3DXVECTOR3 target; // Target point of the camera in the world
	D3DXVECTOR3 upVector; // Vector looking up in the world

	D3DXVECTOR3 localUpVector; // Local camera up vector
	D3DXVECTOR3 localLeftVector; // Local camera left vector
	D3DXVECTOR3 localFrontVector; // Local camera front vector

	// Axis Angles
	//float yaw, pitch;

	// Profundity of vision
	float zFar;

	TLocation loc; // Se calcula cuando se llama a getLocation()

public:
	ICamera( );
	virtual ~ICamera();

	const D3DXMATRIX & getMatView() const { return matView; }
	const D3DXMATRIX & getMatProj() const { return matProj; }
	const D3DXVECTOR3 & getPosition() const { return position; }
	const D3DXVECTOR3 & getTarget() const { return target; }
	const D3DXVECTOR3 & getLocalUpVector() const { return localUpVector; }
	const D3DXVECTOR3 & getLocalLeftVector() const { return localLeftVector; }
	const D3DXVECTOR3 & getLocalFrontVector() const { return localFrontVector; }
	const D3DXVECTOR3 & getUpVector() const { return upVector; }
	TLocation &getLocation();

	void setUpVector(const D3DXVECTOR3 &pUpVector) { upVector = pUpVector; }

	//const float & getYaw() const { return yaw; }
	//const float & getPitch() const { return pitch; }
	const float & getZFar() const { return zFar; }

	virtual void initiate() = 0;
};

class Camera3D;
class CameraOrtho;

ICamera * getCurrentCamera();
Camera3D & getCamera3D();
CameraOrtho & getCameraOrtho();
void activeCamera3D();
void activeCameraOrtho();

#endif //ICAMERA_H
