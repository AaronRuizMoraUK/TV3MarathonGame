#ifndef IMESH_H
#define IMESH_H

#include <vector>
#include "Directx/Directx.h"
#include "Util/Bounding/Bounding.h"

/**
* Interface of a mesh for objects. This has number of total vertex, number of total
* triangles, bounding box and bounding sphere
* material vector.
*/

class IMesh
{
protected:
	struct TStrip {
		int firstVertex;
		int numTriangles;

		TStrip( ) : firstVertex( 0 ), numTriangles( 0 ) {}
	};
	typedef std::vector<TStrip *> StripVector;

	int numTotalVertices;
	int numTotalTriangles;
	StripVector stripVector;

	BoundingBox boundingBox;
	BoundingSphere boundingSphere;

public:
	IMesh() {};
	IMesh(int pNumTotalVertices, int pNumTotalTriangles);

	virtual ~IMesh();

	virtual void render(int firstVertex=0, int numTriangles=-1) const = 0;
	virtual void renderStrip(int index) const = 0;
	virtual void renderStripTriangles(int index, int numTriangles) const = 0;

	virtual void loadToVRAM( ) = 0;
	virtual void freeSystemMemory( ) = 0;
	virtual void *getRawVertices( ) const = 0;
	virtual void *getRawIndexes( ) const = 0;

	inline int getNumTotalVertices( ) const { return numTotalVertices; }
	inline int getNumTotalTriangles( ) const { return numTotalTriangles; }

	inline const BoundingBox &getBoundingBox( ) const { return boundingBox; }
	inline void setBoundingBox( const BoundingBox &bbox ) { boundingBox = bbox; }

	inline const BoundingSphere &getBoundingSphere( ) const { return boundingSphere; }
	virtual void constructBoundingSphere() = 0;

	void fixBoundingBoxAndSphereForCal3D( );
};

#endif //IMESH_H
