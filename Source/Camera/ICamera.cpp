#include <cassert>
#include "ICamera.h"
#include "Camera3D.h"
#include "CameraOrtho.h"
#include "Global/Rendering.h"

ICamera::ICamera( )
: position(0,0,0)
, target(0,0,0)
, upVector(0,1,0)
, localUpVector(0,1,0)
, localLeftVector(1,0,0)
, localFrontVector(0,0,1)
//, yaw(0.0f)
//, pitch(0.0f)
, zFar(0.0f)
{
	D3DXMatrixIdentity( &matView );
	D3DXMatrixIdentity( &matProj );
};

ICamera::~ICamera( ) {
	printDebugString("Destroying ICamera...\n");
}

TLocation & ICamera::getLocation() {
	D3DXMATRIX transform;
	D3DXMatrixIdentity( &transform );
	transform(0,0) = localLeftVector.x;
	transform(0,1) = localLeftVector.y;
	transform(0,2) = localLeftVector.z;
	transform(1,0) = localUpVector.x;
	transform(1,1) = localUpVector.y;
	transform(1,2) = localUpVector.z;
	transform(2,0) = localFrontVector.x;
	transform(2,1) = localFrontVector.y;
	transform(2,2) = localFrontVector.z;
	transform(3,0) = position.x;
	transform(3,1) = position.y;
	transform(3,2) = position.z;

	loc.setTransform( transform );
	return loc;
}

// ------------------------------------------------------

// These are the real cameras
// The pointer to them "camera" is changed
// They are declarated here so nobody can see them
Camera3D    camera3D;
CameraOrtho cameraOrtho;
ICamera *   camera = NULL;

ICamera * getCurrentCamera() {
	assert( camera );
	return camera;
}

Camera3D & getCamera3D() {
	return camera3D;
}

CameraOrtho & getCameraOrtho() {
	return cameraOrtho;
}

void activeCamera3D() {
	camera = &camera3D;
}

void activeCameraOrtho() {
	camera = &cameraOrtho;
}
