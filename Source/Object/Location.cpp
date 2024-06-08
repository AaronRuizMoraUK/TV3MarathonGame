#include <cassert>
#include "Location.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Factories/CollisionShapeFactory.h"

TLocation::TLocation()
: position(0,0,0)
, localFront(0,0,1)
, localLeft(1,0,0)
, localUp(0,1,0)
, rotation(0,0,0,1)
, alpha( 0.0f )
, delta( 0.0f )
, parent_loc( NULL )
{
	D3DXMatrixIdentity( &transform );
	D3DXMatrixIdentity( &local_transform );
}

TLocation::~TLocation() {
	printDebugString("Destroying TLocation...\n");
}

void TLocation::setPosition(const D3DXVECTOR3 &a_position) {
	position = a_position;
	transform(3,0)=position.x;
	transform(3,1)=position.y;
	transform(3,2)=position.z;
}

const D3DXQUATERNION &TLocation::getRotation() {
	D3DXQuaternionRotationMatrix( &rotation, &transform );
	return rotation;
}

void TLocation::setTransform(const D3DXMATRIX &a_transform) {
	transform = a_transform;
	updateVariablesFromTransform();
}

const D3DXMATRIX & TLocation::getInverseTransform() {
	D3DXMatrixInverse(&inverse_transform, NULL, &transform);
	return inverse_transform;
}

void TLocation::updateVariablesFromTransform() {
	localLeft.x  = transform(0,0);
	localLeft.y  = transform(0,1);
	localLeft.z  = transform(0,2);
	localUp.x    = transform(1,0);
	localUp.y    = transform(1,1);
	localUp.z    = transform(1,2);
	localFront.x = transform(2,0);
	localFront.y = transform(2,1);
	localFront.z = transform(2,2);
	position.x   = transform(3,0);
	position.y   = transform(3,1);
	position.z   = transform(3,2);
}

void TLocation::updateTransformWithParent( ) {
	assert( parent_loc );
	D3DXMATRIX new_transform;
	D3DXMatrixMultiply( &new_transform, &local_transform, &parent_loc->getTransform( ) );
	setTransform( new_transform );
}

// ---------------------------------------------------------------------

LocationCollisionable::LocationCollisionable()
: TLocation( )
, collisionShapeHandle(NULL)
, collisionObjectHandle(NULL)
, prev_transform( transform )
{
}

LocationCollisionable::LocationCollisionable(const LocationCollisionable &source)
: TLocation(source)
, collisionShapeHandle(NULL)
, collisionObjectHandle(NULL)
, prev_transform( source.prev_transform )
{
	copy( source );
}

LocationCollisionable::~LocationCollisionable() {
	printDebugString("Destroying LocationCollisionable...\n");
	destroy();
}

void LocationCollisionable::operator=(const LocationCollisionable &source) {
	TLocation::operator=( source );
	copy( source );
}

//const DT_ShapeHandle LocationCollisionable::getCollisionShape() const {
//	return ( collisionShapeHandle == NULL ) ? NULL : collisionShapeHandle->shapeHandle;
//}

void LocationCollisionable::createCollisionObject(const ShapeHandle *shapeHandle) {
	collisionShapeHandle=shapeHandle;

	// Destroy and create the new object with the new shape
	if( collisionObjectHandle != NULL )
		DT_DestroyObject(collisionObjectHandle), collisionObjectHandle=NULL;

	if( collisionShapeHandle ) {
		collisionObjectHandle = DT_CreateObject(this, collisionShapeHandle->shapeHandle);
		assert( collisionObjectHandle );
	}
}

void LocationCollisionable::setPosition(const D3DXVECTOR3 &a_position) {
	// Update Previous transform if we have collisions
	if( collisionObjectHandle != NULL ) {
		prev_transform(3,0) = transform(3,0);
		prev_transform(3,1) = transform(3,1);
		prev_transform(3,2) = transform(3,2);
	}

	// Set new position
	TLocation::setPosition(a_position);

	// Update collision shape
	setCollisionPosition();
}

void LocationCollisionable::setTransform(const D3DXMATRIX &a_transform) {
	// Update Previous transform if we have collisions
	if( collisionObjectHandle != NULL )
		prev_transform = transform;

	// Set new transform
	TLocation::setTransform(a_transform);

	// Update collision shape
	setCollisionTransform();
}

void LocationCollisionable::setCollisionPosition() {
	if( collisionObjectHandle != NULL ) {
		assert( collisionShapeHandle );
		D3DXVECTOR3 collisionPosition = position + collisionShapeHandle->collisionFixY * localUp;
		DT_SetPosition(collisionObjectHandle, collisionPosition);
	}
}

void LocationCollisionable::setCollisionTransform() {
	if( collisionObjectHandle!=NULL ) {
		DT_SetMatrixf(collisionObjectHandle, transform);
		setCollisionPosition();
	}
}

void LocationCollisionable::destroy( ) {
	// LocationCollisionable does not destroy its Collision shape, CollisionShapeFactory does!!.
	collisionShapeHandle = NULL;

	// Destroy the collision object because each instance has its own collision object
	if( collisionObjectHandle != NULL )
		DT_DestroyObject(collisionObjectHandle), collisionObjectHandle=NULL;
}

void LocationCollisionable::copy( const LocationCollisionable &source ) {
	destroy( );

	createCollisionObject( source.collisionShapeHandle );

	// Copy the transform of source
	setTransform( source.transform );
	prev_transform = source.prev_transform;
}
