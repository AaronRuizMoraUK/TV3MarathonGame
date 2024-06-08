#include <cassert>
#include "CollisionShapeFactory.h"
#include "Global/Rendering.h"
#include "Mesh/IMesh.h"

ShapeHandle::ShapeHandle()
: shapeHandle(NULL)
, collisionFixY(0.0f)
{
}

ShapeHandle::~ShapeHandle() {
	printDebugString("Destroying ShapeHandle...\n");

	if(shapeHandle!=NULL)
		DT_DeleteShape(shapeHandle) , shapeHandle=NULL;
}

// --------------------------------------------------------------------------------------

const char *CollisionShapeFactory::type_shape_names[NUM_TYPE_SHAPES] = {
	"UNDEF"
	, "BOX"
	, "CONE"
	, "CYLINDER"
	, "SPHERE"
};

CollisionShapeFactory * CollisionShapeFactory::collisionShapeFactory = NULL;

CollisionShapeFactory::~CollisionShapeFactory() {
	printDebugString("Destroying CollisionShapeFactory...\n");

	// Destroy collision shape map
	CollisionShapeMap::const_iterator it = collisionShapeMap.begin();
	while(it !=  collisionShapeMap.end()) {	
		ShapeHandle *collisionShape = (it->second);
		if(collisionShape)
			delete collisionShape, collisionShape=NULL;
		++it;
	}
	collisionShapeMap.clear();
}

CollisionShapeFactory * CollisionShapeFactory::get() {
	if(collisionShapeFactory == NULL)
		collisionShapeFactory = new CollisionShapeFactory();

	return collisionShapeFactory;
}

CollisionShapeFactory::TypeShape CollisionShapeFactory::getTypeShape( const char *type ) {
	for( int i=0; i<NUM_TYPE_SHAPES; ++i ) {
		if( strcmp( type, type_shape_names[ i ] ) == 0 )
			return (TypeShape)i;
	}
	return UNDEF;
}

const ShapeHandle * CollisionShapeFactory::createCollisionShape(const IMesh *mesh
																													, TypeShape typeShape
																													, float bias
																													, bool fixY)
{
	assert( mesh != NULL );
	assert( typeShape != UNDEF );
	ShapeIdentifier shape_identifier( mesh, typeShape, bias, fixY );

	CollisionShapeMap::const_iterator it = collisionShapeMap.find( shape_identifier );

	if(it != collisionShapeMap.end())
		return it->second;
	else {
		ShapeHandle *shape = NULL;

		if ( typeShape==BOX_SHAPE )
			shape = createBoxShape(mesh, fixY, bias);
		else if ( typeShape==CONE_SHAPE )
			shape = createConeShape(mesh, fixY, bias);
		else if( typeShape==CYLINDER_SHAPE )
			shape = createCylinderShape(mesh, fixY, bias);
		else if ( typeShape==SPHERE_SHAPE )
			shape = createSphereShape(mesh, fixY, bias);
		else {
			assert( !"Trying to create a collision shape that does not exist" );
			return NULL;
		}

		if(shape==NULL)
			return NULL;

		collisionShapeMap[shape_identifier]=shape;
		return shape;
	}
}

ShapeHandle * CollisionShapeFactory::createCylinderShape(const IMesh *mesh, bool fixY, float bias) {

	const D3DXVECTOR3 & minLocalPoint = mesh->getBoundingBox().min_point;
	const D3DXVECTOR3 & maxLocalPoint = mesh->getBoundingBox().max_point;

	// Get radius
	float radiusX = (maxLocalPoint.x - minLocalPoint.x)*0.5f;
	float radiusZ = (maxLocalPoint.z - minLocalPoint.z)*0.5f;
	float radius = max(radiusX, radiusZ) * bias; // With bias adjust we get a best radius for gameplay

	// Get Height
	float height = maxLocalPoint.y - minLocalPoint.y;

	// Create sphere shape
	ShapeHandle *collisionShapeHandle = new ShapeHandle();
	collisionShapeHandle->shapeHandle = DT_NewCylinder(radius, height);

	if( fixY )
		collisionShapeHandle->collisionFixY = height*0.5f;

	return collisionShapeHandle;
}

ShapeHandle * CollisionShapeFactory::createSphereShape(const IMesh *mesh, bool fixY, float bias) {
	// Get radius
	float radius = mesh->getBoundingSphere().radius;

	// Create spherical shape
	ShapeHandle *collisionShapeHandle = new ShapeHandle();
	collisionShapeHandle->shapeHandle = DT_NewSphere(radius*bias); // With bias adjust we get a best radius for gameplay

	if( fixY )
		collisionShapeHandle->collisionFixY = radius;

	return collisionShapeHandle;
}

ShapeHandle * CollisionShapeFactory::createBoxShape(const IMesh *mesh, bool fixY, float bias) {

	const D3DXVECTOR3 & minLocalPoint = mesh->getBoundingBox().min_point;
	const D3DXVECTOR3 & maxLocalPoint = mesh->getBoundingBox().max_point;

	float x = maxLocalPoint.x - minLocalPoint.x; 
	float y = maxLocalPoint.y - minLocalPoint.y;
	float z = maxLocalPoint.z - minLocalPoint.z;

	// Create box shape
	ShapeHandle *collisionShapeHandle = new ShapeHandle();
	collisionShapeHandle->shapeHandle = DT_NewBox(x*bias, y*bias, z*bias); // With bias adjust we get a best radius for gameplay

	if( fixY )
		collisionShapeHandle->collisionFixY = y;

	return collisionShapeHandle;
}

ShapeHandle * CollisionShapeFactory::createConeShape(const IMesh *mesh, bool fixY, float bias) {

	const D3DXVECTOR3 & minLocalPoint = mesh->getBoundingBox().min_point;
	const D3DXVECTOR3 & maxLocalPoint = mesh->getBoundingBox().max_point;

	// Get radius
	float radiusX = (maxLocalPoint.x - minLocalPoint.x)*0.5f;
	float radiusZ = (maxLocalPoint.z - minLocalPoint.z)*0.5f;
	float radius = max(radiusX, radiusZ);

	// Get Height
	float height = maxLocalPoint.y - minLocalPoint.y;

	// Create cone shape
	ShapeHandle *collisionShapeHandle = new ShapeHandle();
	collisionShapeHandle->shapeHandle = DT_NewCone(radius, height);

	if( fixY )
		collisionShapeHandle->collisionFixY = height*0.5f;

	return collisionShapeHandle;
}
