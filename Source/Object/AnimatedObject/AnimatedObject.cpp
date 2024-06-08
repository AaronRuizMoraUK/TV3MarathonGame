#include <cassert>
#include "Global/Rendering.h"
#include "AnimatedObject.h"
#include "AnimationControllers/IAnimationController.h"
#include "AnimationControllers/ROA/ROAObject.h"
#include "AnimationControllers/YAxisRotation/YAxisRotation.h"

bool AnimatedObject::animation_enabled = true;

// Default constructor
AnimatedObject::AnimatedObject()
: animationController(NULL)
{
}

// Override copy constructor
AnimatedObject::AnimatedObject(const AnimatedObject &source)
: Object(source)
, animationController(NULL)
{
	copy(source);
}

// Destructor
AnimatedObject::~AnimatedObject() {
	printDebugString("Destroying AnimatedObject...\n");
	destroy();
}

// Override Operator '='
void AnimatedObject::operator=(const AnimatedObject &source) {
	Object::operator=(source);
	copy(source);
}

void AnimatedObject::destroy() {
	// Destroy Animation controller
	if(animationController)
		delete animationController, animationController=NULL;
}

void AnimatedObject::copy(const AnimatedObject &source) {
	destroy( );

	if( source.animationController != NULL ) {
		// Copy animation controller
		const char *controller_type = source.animationController->getControllerType();

		if( strcmp( controller_type, "ROAObject" ) == 0 )
			animationController = new ROAObject( *((ROAObject *) source.animationController) );
		else if( strcmp( controller_type, "YAxisRotation" ) == 0 )
			animationController = new YAxisRotation( *((YAxisRotation *) source.animationController) );
		else {
			assert( !"Not type of animation controller allowed in AnimatedObject" );
			return;
		}
	}
}

void AnimatedObject::updateAnimation(float deltaTime) {
	if (animationController && animation_enabled)
		animationController->update(*this, deltaTime);
}

void AnimatedObject::setAnimationController(IAnimationController *pAnimationController)
{
	if(animationController)
		delete animationController, animationController=NULL;
	animationController = pAnimationController;
}
