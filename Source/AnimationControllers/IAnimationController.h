#ifndef IANIMATION_CONTROLLER_H
#define IANIMATION_CONTROLLER_H

class AnimatedObject;

/**
* Interface of Animation Controller for animated objects
*/

class IAnimationController
{
public:
	virtual ~IAnimationController( ) {}

	virtual void update( AnimatedObject &object, float deltaTime ) = 0;
	virtual bool isFinished() const { return false; }

	virtual const char * getControllerType () const = 0;
};

#endif // IANIMATION_CONTROLLER_H
