#ifndef CAMERA_3D_H
#define CAMERA_3D_H

#include "ICamera.h"

class ICameraController;

/**
* 3D Camera of the scene
*/

class Camera3D : public ICamera {

protected:
	// Parameters of Projection Matrix
	float fov; // Field of View for camera (angle in radians)

	// Camera controller
	ICameraController * cameraController;

public:
	Camera3D( );
	~Camera3D( );

	void initiate();

	void lookAt(const D3DXVECTOR3 &pPosition, const D3DXVECTOR3 &pTarget);
	void update(float deltaTime);

	ICameraController *getCameraController( ) const {return cameraController;}
	void setCameraController( ICameraController * acameraController );

	void activeOnDevice() const;
};

#endif //CAMERA_3D_H
