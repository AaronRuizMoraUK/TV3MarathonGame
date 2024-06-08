#ifndef QUAD_FACTORY_H
#define QUAD_FACTORY_H

#include <map>
#include "Util/MapComparers/MapComparers.h"

class IMesh;

/**
* Factory of quads.
*
* This is a singleton.
*/

class QuadFactory
{
protected:
	typedef std::map<std::vector<float>, const IMesh *, CompareFloatVector> QuadMeshMap;
	QuadMeshMap quadMeshMap;

public:
	~QuadFactory();
	
	static QuadFactory * get();

	const IMesh * createQuad(float radiusSideX, float radiusSideY, int numQuadVertexPerAxis=2);

private:
	static QuadFactory * quadFactory;
	QuadFactory() {};

	const IMesh * createQuadMesh(float radiusSideX, float radiusSideY, unsigned int numQuadVertexPerAxis) const;
};

#endif //QUAD_FACTORY_H
