#ifndef COLLISION_SHAPE_FACTORY_H
#define COLLISION_SHAPE_FACTORY_H

#include <map>
#include "Solid/SOLID.h"

class IMesh;

/**
* Factory of collision shapes.
*
* This is a singleton.
*/

struct ShapeHandle {
	DT_ShapeHandle shapeHandle;
	float collisionFixY; // The shape's position is in its geometric center, but in some objects we want it in the base

	ShapeHandle();
	~ShapeHandle();
};

class CollisionShapeFactory
{
public:
	// Type of shapes
	enum TypeShape {
		UNDEF = 0
		, BOX_SHAPE
		, CONE_SHAPE
		, CYLINDER_SHAPE
		, SPHERE_SHAPE
		, NUM_TYPE_SHAPES
	};
	static const char *type_shape_names[NUM_TYPE_SHAPES];

protected:
	struct ShapeIdentifier {
		const IMesh *mesh;
		TypeShape    typeShape;
		float        bias;
		bool         fixY;

		ShapeIdentifier( const IMesh *amesh, TypeShape atypeShape, float abias, bool afixY )
			: mesh( amesh )
			, typeShape( atypeShape )
			, bias( abias )
			, fixY( afixY )
		{}

		bool operator<( const ShapeIdentifier &other ) const {
			if( mesh != other.mesh )
				return mesh < other.mesh;
			else if( typeShape != other.typeShape )
				return typeShape < other.typeShape;
			else if( bias != other.bias )
				return bias < other.bias;
			else if( fixY != other.fixY )
				return fixY < other.fixY;
			else
				return false;
		}
	};

	typedef std::map<ShapeIdentifier, ShapeHandle *> CollisionShapeMap;
	CollisionShapeMap collisionShapeMap;

public:
	~CollisionShapeFactory();

	static CollisionShapeFactory * get();
	static TypeShape getTypeShape( const char *type );

	const ShapeHandle * createCollisionShape(const IMesh *mesh, TypeShape typeShape, float bias=1.0f, bool fixY=true);

private:
	static CollisionShapeFactory * collisionShapeFactory;
	CollisionShapeFactory() {}

	ShapeHandle *createCylinderShape(const IMesh *mesh, bool fixY, float bias);
	ShapeHandle *createSphereShape(const IMesh *mesh, bool fixY, float bias);
	ShapeHandle *createBoxShape(const IMesh *mesh, bool fixY, float bias);
	ShapeHandle *createConeShape(const IMesh *mesh, bool fixY, float bias);
};

#endif //COLLISION_SHAPE_FACTORY_H
