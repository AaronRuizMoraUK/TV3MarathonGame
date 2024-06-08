#ifndef CORE_MESH_H
#define CORE_MESH_H

#include "Mesh/IMesh.h"

/**
* Core mesh for objects with any type of vertex and index.
* This has vertex and index buffer both in system memory and
* video memory
*/

template<class VertexType, class IndexType = unsigned short> 
class CoreMesh : public IMesh {

protected:
	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;
	VertexType *systemVertices;
	IndexType  *systemIndexes;

public:
	CoreMesh(int pNumTotalVertices, int pNumTotalTriangles);
	~CoreMesh();

	void render(int firstVertex=0, int numTriangles=-1) const;
	void renderStripTriangles(int index, int numTriangles) const;
	void renderStrip(int index) const;
	void loadToVRAM( );
	void freeSystemMemory( );
	void *getRawVertices( ) const;
	void *getRawIndexes( ) const;

	void constructBoundingSphere();

	void allocate(int pNumTotalVertices, int pNumTotalTriangles);
	void freeVRAM();

	inline const VertexType *getVertexs( ) const { return systemVertices; }	
	inline const IndexType  *getIndexes( ) const { return systemIndexes; }

private:
	void activateStream( int streamNumber ) const;
};

#include "CoreMesh.Impl.h"

#endif //CORE_MESH_H