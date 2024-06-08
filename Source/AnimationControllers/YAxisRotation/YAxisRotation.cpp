#include "YAxisRotation.h"
#include "Global/Rendering.h"
#include "Global/Geometry.h"
#include "Object/AnimatedObject/AnimatedObject.h"

YAxisRotation::YAxisRotation( float aspeed )
: speed( aspeed )
{}

YAxisRotation::~YAxisRotation( ) {
	printDebugString( "Destroying YAxisRotation...\n" );
}

void YAxisRotation::update( AnimatedObject &object, float deltaTime ) {
	float angle = deltaTime * speed;

	D3DXMATRIX transform;
	rotateOnAxis( transform, object.loc.getTransform( ), object.loc.getLocalUp( ), angle );

	object.loc.setTransform( transform );
}
