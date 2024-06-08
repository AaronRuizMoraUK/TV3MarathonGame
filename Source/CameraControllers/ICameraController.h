#ifndef ICAMERA_CONTROLLER_H
#define ICAMERA_CONTROLLER_H

class Camera3D;

/**
* Interface of Camera Controller
*/

class ICameraController {
public:
	virtual ~ICameraController( ) {}

	virtual void update( Camera3D &camera, float deltaTime ) = 0;
	virtual bool isFinished() { return false; }
};

#endif //ICAMERA_CONTROLLER_H
