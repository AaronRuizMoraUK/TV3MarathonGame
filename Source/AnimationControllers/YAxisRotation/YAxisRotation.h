#ifndef Y_AXIS_ROTATION_H
#define Y_AXIS_ROTATION_H

#include "AnimationControllers/IAnimationController.h"
#include "Controllers/ROA.h"

/**
* Animation Controller that rotate in local Y Axis
*/

class YAxisRotation: public IAnimationController
{
protected:
	float speed;

public:
	YAxisRotation( float aspeed );
	~YAxisRotation( );

	void update( AnimatedObject &object, float deltaTime );

	const char * getControllerType () const { return "YAxisRotation"; }
};

#endif // Y_AXIS_ROTATION_H
