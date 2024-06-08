#ifndef ANIMATED_OBJECT_H
#define ANIMATED_OBJECT_H

#include "Object/Object.h"

class IAnimationController;

/**
* A animated object in scene.
*/

class AnimatedObject : public Object
{
public:
	static bool animation_enabled;

protected:
	IAnimationController *animationController;

public:
	AnimatedObject();
	AnimatedObject(const AnimatedObject &source);
	virtual ~AnimatedObject();

	virtual void operator=(const AnimatedObject &source);

	virtual void updateAnimation(float deltaTime);

	void setAnimationController(IAnimationController *pAnimationController);

	inline IAnimationController * getAnimationController() const { return animationController; }

private:
	void destroy();
	void copy(const AnimatedObject &source);
};

#endif //ANIMATED_OBJECT_H
