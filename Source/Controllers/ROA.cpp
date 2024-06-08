#include <cassert>
#include "ROA.h"
#include "ROAResource.h"
#include "Global/Geometry.h"
#include "Global/Rendering.h"

ROA::ROA( const RigidAnimationResource * pRigidAnimationResource, bool ploop )
: rigidAnimationResource( pRigidAnimationResource )
, currentTime( 0.0f )
, timeScale( 1.0f )
, position(0,0,0)
, rotation(0,0,0,1)
, positionSync(0,0,0)
, rotationSync(0,0,0,1)
, loop( ploop )
{
	assert( rigidAnimationResource != NULL );

	// Obtain sample of position and rotation in current time (0) said by animation
	D3DXVECTOR3 positionFrameCurrentTime;
	D3DXQUATERNION rotationFrameCurrentTime;
	rigidAnimationResource->getPosition( currentTime, positionFrameCurrentTime );
	rigidAnimationResource->getRotation( currentTime, rotationFrameCurrentTime );

	// Update position and rotation
	rotation = rotationFrameCurrentTime * rotationSync;
	D3DXVECTOR3 positionFrameCurrentTimeRotateSync;
	rotateVector3Quaternion(positionFrameCurrentTimeRotateSync, positionFrameCurrentTime, rotationSync);
	position = positionSync + positionFrameCurrentTimeRotateSync;
}

ROA::ROA( const RigidAnimationResource * pRigidAnimationResource
				 , const D3DXVECTOR3 &initialPosition
				 , const D3DXQUATERNION &initialRotation
				 , bool ploop )
				 : rigidAnimationResource( pRigidAnimationResource )
				 , currentTime( 0.0f )
				 , timeScale( 1.0f )
				 , position(0,0,0)
				 , rotation(0,0,0,1)
				 , positionSync(0,0,0)
				 , rotationSync(0,0,0,1)
				 , loop( ploop )
{
	assert( rigidAnimationResource != NULL );
	restart(initialPosition, initialRotation);
}

ROA::~ROA() {
	printDebugString("Destroying Key Framed ROA controller...\n");

	// KeyFramedROA does not have to delete RigidAnimationResource 
};

void ROA::setPercentage( float percentage ) {
	assert( percentage >= 0.0f && percentage <= 1.0f );
	currentTime = rigidAnimationResource->getDuration( ) * percentage;

	// Obtain sample of position and rotation in current time (0) said by animation
	D3DXVECTOR3 positionFrameCurrentTime;
	D3DXQUATERNION rotationFrameCurrentTime;
	rigidAnimationResource->getPosition( currentTime, positionFrameCurrentTime );
	rigidAnimationResource->getRotation( currentTime, rotationFrameCurrentTime );

	// Update position and rotation
	rotation = rotationFrameCurrentTime * rotationSync;
	D3DXVECTOR3 positionFrameCurrentTimeRotateSync;
	rotateVector3Quaternion(positionFrameCurrentTimeRotateSync, positionFrameCurrentTime, rotationSync);
	position = positionSync + positionFrameCurrentTimeRotateSync;
}

void ROA::update( float deltaTime )
{
	if( isFinished( ) )
		return;

	// Obtain sample of position and rotation in current time said by animation
	currentTime += deltaTime*timeScale;
	D3DXVECTOR3 positionFrameCurrentTime;
	D3DXQUATERNION rotationFrameCurrentTime;
	rigidAnimationResource->getPosition( currentTime, positionFrameCurrentTime );
	rigidAnimationResource->getRotation( currentTime, rotationFrameCurrentTime );

	// Update position and rotation
	rotation = rotationFrameCurrentTime * rotationSync;
	D3DXVECTOR3 positionFrameCurrentTimeRotateSync;
	rotateVector3Quaternion(positionFrameCurrentTimeRotateSync, positionFrameCurrentTime, rotationSync);
	position = positionSync + positionFrameCurrentTimeRotateSync;

	// --------------------------
	//D3DXMATRIX newRotation;
	//D3DXMatrixRotationQuaternion(&newRotation, &rotation);
	//object.setTransformMatrix(newRotation);
	//object.setPosition(position);

	// Update object
	if( isFinished() && loop )
		restart( position, rotation );

	//printfDebugString("Position: (%1.2f,%1.2f,%1.2f) Target: (%1.2f,%1.2f,%1.2f)\n"
	//	, position.x, position.y, position.z, target.x, target.y, target.z);
}

void ROA::restart( const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation )
{
	// Sample start time
	currentTime = 0;
	D3DXVECTOR3 positionFrame0;
	D3DXQUATERNION rotationFrame0;

	rigidAnimationResource->getPosition( 0, positionFrame0 );
	rigidAnimationResource->getRotation( 0, rotationFrame0 );

	// Compute corrections so that:
	// r(t) = rs*r(t)      => r(0) = r0 = rs*R(0)   => rs = r0*R(0)^-1
	// p(t) = ps + rs*P(t) => p(0) = p0 = ps + rs*P(0) => ps = p0-rs*P(0)
	D3DXQUATERNION inverseRotationFrame0;
	D3DXQuaternionInverse( &inverseRotationFrame0, &rotationFrame0);
	rotationSync = inverseRotationFrame0 * initialRotation;
	D3DXVECTOR3 positionFrame0RotateSync;
	rotateVector3Quaternion(positionFrame0RotateSync, positionFrame0, rotationSync);
	positionSync = initialPosition - positionFrame0RotateSync;

	// Init cached state
	position = initialPosition;
	rotation = initialRotation;
}

bool ROA::isFinished() const { 
	return ( currentTime >= rigidAnimationResource->getDuration() );
}

TLocation &ROA::getLocation() {
	D3DXMATRIX transform;
	D3DXMatrixRotationQuaternion(&transform, &rotation);
	loc.setTransform( transform );
	loc.setPosition( position );

	return loc;
}
