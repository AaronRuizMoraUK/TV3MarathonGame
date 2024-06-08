#include <cassert>
#include "QuadFactory.h"
#include "Mesh/IMesh.h"
#include "Global/Rendering.h"
#include "Mesh/MeshTypes.h"

QuadFactory * QuadFactory::quadFactory = NULL;

QuadFactory::~QuadFactory() {
	printDebugString("Destroying QuadFactory...\n");

	// Destroy quad mesh map
	QuadMeshMap::const_iterator it = quadMeshMap.begin();
	while(it !=  quadMeshMap.end()) {	
		const IMesh *mesh = (it->second);
		if(mesh)
			delete mesh, mesh=NULL;
		++it;
	}
	quadMeshMap.clear();
}

QuadFactory * QuadFactory::get() {
	if(quadFactory == NULL)
		quadFactory = new QuadFactory();

	return quadFactory;
}

const IMesh * QuadFactory::createQuad(float radiusSideX, float radiusSideY, int numQuadVertexPerAxis) {
	assert( radiusSideX > 0.0f && radiusSideY > 0.0f && numQuadVertexPerAxis >= 2 );

	// Create the key
	std::vector<float> key;
	key.push_back(radiusSideX);
	key.push_back(radiusSideY);
	key.push_back((float)numQuadVertexPerAxis);

	QuadMeshMap::const_iterator it = quadMeshMap.find(key);

	if(it != quadMeshMap.end())
		return it->second;
	else {
		const IMesh * mesh = createQuadMesh(radiusSideX, radiusSideY, numQuadVertexPerAxis);
		quadMeshMap[key]=mesh;
		return mesh;
	}
}

const IMesh * QuadFactory::createQuadMesh(float radiusSideX, float radiusSideY, unsigned int numQuadVertexPerAxis) const
{
	const float quadSizeX = radiusSideX*2.0f;
	const float quadSizeY = radiusSideY*2.0f;
	int numQuadVertices = numQuadVertexPerAxis * numQuadVertexPerAxis;
	int numQuadTriangles = ( numQuadVertexPerAxis - 1 ) * ( numQuadVertexPerAxis - 1 ) * 2;

	// Min and max vertex point
	D3DXVECTOR3 minPoint(0,0,0);
	D3DXVECTOR3 maxPoint(0,0,0);

	// Mesh
	IMesh *mesh = new CoreMeshTextured(numQuadVertices, numQuadTriangles);
	assert(mesh);

	// Vertices
	CUSTOMVERTEX_TEXTURE *quadVertexData = (CUSTOMVERTEX_TEXTURE *)mesh->getRawVertices();	// Vertices del objeto con coordenadas y textura

	for(unsigned int ix=0; ix<numQuadVertexPerAxis; ++ix) {

		float ux = (float) ix / (float) ( numQuadVertexPerAxis - 1 ); // ux = ix normalized

		CUSTOMVERTEX_TEXTURE *v = quadVertexData + ix * numQuadVertexPerAxis; // Vertex to fill

		for(unsigned int iy=0; iy<numQuadVertexPerAxis; ++iy) {
			float uy = (float) iy / (float) ( numQuadVertexPerAxis - 1 ); // uy = iy normalized

			// Fill vertex data
			v[ iy ].x = quadSizeX * ( ux - 0.5f );
			v[ iy ].y = quadSizeY * ( uy - 0.5f );
			v[ iy ].z = 0;
			v[ iy ].u1 = ux;
			v[ iy ].v1 = (1.0f-uy);

			// Update min and max point
			D3DXVECTOR3 vertex(v[iy].x, v[iy].y, v[iy].z);
			for(int i=0; i<3; ++i) {
				if(vertex[i]<minPoint[i])
					minPoint[i]=vertex[i];
				else if(vertex[i]>maxPoint[i])
					maxPoint[i]=vertex[i];
			}
		}
	}

	// Indexes
	unsigned short * quadIndexData = (unsigned short *)mesh->getRawIndexes();
	unsigned short *idx = quadIndexData; // Pointer for covering quadIndexData to fill it

	for(unsigned int x=0; x<numQuadVertexPerAxis-1; ++x) {
		for(unsigned int y=0; y<numQuadVertexPerAxis-1; ++y) {
			unsigned short top = x * numQuadVertexPerAxis + y;
			*idx++ = top;
			*idx++ = top+1;
			*idx++ = top+numQuadVertexPerAxis;

			*idx++ = top+1;
			*idx++ = top+1+numQuadVertexPerAxis;
			*idx++ = top+numQuadVertexPerAxis;
		}
	}

	mesh->loadToVRAM();
	mesh->setBoundingBox( BoundingBox( minPoint, maxPoint ) );
	mesh->constructBoundingSphere();
	mesh->freeSystemMemory();

	return mesh;
}
