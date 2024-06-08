#include "ROAObject.h"
#include "Global/Rendering.h"
#include "Object/AnimatedObject/AnimatedObject.h"

ROAObject::ROAObject( const RigidAnimationResource * pRigidAnimationResource, bool loop )
: roa( pRigidAnimationResource, loop )
{}

ROAObject::ROAObject( const RigidAnimationResource * pRigidAnimationResource
					, const D3DXVECTOR3 &initialPosition
					, const D3DXQUATERNION &initialRotation
					, bool loop )
					: roa( pRigidAnimationResource, initialPosition, initialRotation, loop )
{}

ROAObject::~ROAObject( ) {
	printDebugString( "Destroying ROAObject...\n" );
}

void ROAObject::update( AnimatedObject &object, float deltaTime ) {
	if( isFinished( ) )
		return;

	roa.update( deltaTime );
	const TLocation &loc = roa.getLocation( );
	object.loc.setTransform( loc.getTransform( ) );
}
