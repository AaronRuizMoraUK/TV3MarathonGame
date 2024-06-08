#ifndef CAMERA_ORTHO_H
#define CAMERA_ORTHO_H

#include "ICamera.h"

/**
* Orthogonal Camera of the scene for UI
*/

class CameraOrtho : public ICamera {

public:
	~CameraOrtho( );

	void initiate( );
};

#endif //CAMERA_ORTHO_H
