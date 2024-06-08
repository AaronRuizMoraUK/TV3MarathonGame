#include "ROACamera.h"
#include "Global/Rendering.h"
#include "Camera/Camera3D.h"

ROACamera::ROACamera( const RigidAnimationResource * pRigidAnimationResource, bool loop )
: roa( pRigidAnimationResource, loop )
{}

ROACamera::ROACamera( const RigidAnimationResource * pRigidAnimationResource
					, const D3DXVECTOR3 &initialPosition
					, const D3DXQUATERNION &initialRotation
					, bool loop )
					: roa( pRigidAnimationResource, initialPosition, initialRotation, loop )
{}

ROACamera::~ROACamera( ) {
	printDebugString( "Destroying ROACamera...\n" );
}

void ROACamera::update( Camera3D &camera, float deltaTime ) {
	if( isFinished( ) )
		return;

	roa.update( deltaTime );
	const TLocation &loc = roa.getLocation( );
	camera.setUpVector( loc.getLocalUp( ) );
	camera.lookAt( loc.getPosition( ), loc.getPosition( ) + loc.getLocalFront( ) );
}
