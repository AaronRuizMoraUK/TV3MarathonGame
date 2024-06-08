#include "Camera3D.h"
#include "Game/Game.h"
#include "Global/Rendering.h"
#include "CameraControllers/ICameraController.h"
#include "Global/Geometry.h"
#include "Global/Variables.h"

Camera3D::Camera3D () 
: fov(D3DX_PI/4)
, cameraController(NULL)
{
}

void Camera3D::initiate() {
	zFar = 3500.0f;

	// Initiate Projection Matrix in RH (Right Hand)
	D3DXMatrixPerspectiveFovRH( &matProj, fov, Game::configOptions.resolution.aspect_ratio, 1.0f, zFar );
}

Camera3D::~Camera3D() {
	printDebugString("Destroying Camera...\n");

	// If camera is destroied, his controller is destroied
	if(cameraController)
		delete cameraController, cameraController=NULL;
}

void Camera3D::lookAt(const D3DXVECTOR3 &pPosition, const D3DXVECTOR3 &pTarget) {
	position=pPosition;
	target=pTarget;

	// Update View Matrix in RH (Right Hand)
	D3DXMatrixLookAtRH( &matView, &position, &target, &upVector);

	// Update camera local axis from View Matrix
	localLeftVector = D3DXVECTOR3( -matView(0,0), -matView(1,0), -matView(2,0));
	localUpVector = D3DXVECTOR3(  matView(0,1),  matView(1,1),  matView(2,1));
	localFrontVector = D3DXVECTOR3( -matView(0,2), -matView(1,2), -matView(2,2));

	// Update yaw and pitch for localFrontVector
	//vectorToAngles(localFrontVector, yaw, pitch);
}

void Camera3D::update(float deltaTime) {
	if(cameraController)
		cameraController->update(*this, deltaTime);
}

void Camera3D::setCameraController( ICameraController * acameraController ) {
	if(cameraController)
		delete cameraController, cameraController=NULL;
	cameraController=acameraController;
}

// Activate the camera in the device
void Camera3D::activeOnDevice() const {
#ifndef NDEBUG
	// Set View Matrix to the Device
	Global::device->SetTransform( D3DTS_VIEW, &matView );

	// Set Projection Matrix to the Device
	Global::device->SetTransform( D3DTS_PROJECTION, &matProj );
#endif
}
