#include "CameraOrtho.h"
#include "Global/Rendering.h"

CameraOrtho::~CameraOrtho( ) {
	printDebugString("Destroying CameraOrtho...\n");
};

void CameraOrtho::initiate( ) {
	zFar = 1.0f;

	D3DXMatrixOrthoOffCenterRH(&matProj, -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, zFar);
}
