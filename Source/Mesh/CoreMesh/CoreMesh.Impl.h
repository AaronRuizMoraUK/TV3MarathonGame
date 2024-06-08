#ifndef COREMESHIMPL_H
#define COREMESHIMPL_H

#include "Global/Variables.h"
#include "Global/Geometry.h"

template<class VertexType, class IndexType>
CoreMesh<VertexType,IndexType>::CoreMesh(int pNumTotalVertices, int pNumTotalTriangles) 
: IMesh(pNumTotalVertices, pNumTotalTriangles)
, vertexBuffer( NULL )
, indexBuffer( NULL )
, systemVertices( NULL )
, systemIndexes( NULL )
{
	allocate(numTotalVertices, numTotalTriangles);
}

template<class VertexType, class IndexType>
CoreMesh<VertexType,IndexType>::~CoreMesh()
{
	printDebugString("Destroying CoreMesh...\n");

	freeVRAM( );
	freeSystemMemory( );
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::constructBoundingSphere()
{
	const D3DXVECTOR3 &minPoint = boundingBox.min_point;
	const D3DXVECTOR3 &maxPoint = boundingBox.max_point;

	// Get the center of the Sphere
	D3DXVECTOR3 center( (minPoint + maxPoint) * 0.5f );

	// Get the radius (max distance to vertices)
	D3DXVECTOR3 systemVertexPosition( systemVertices[0].x, systemVertices[0].y, systemVertices[0].z );
	float distanceCenterPoint = distanceSq(center, systemVertexPosition); 
	for(int i=1; i<numTotalVertices; ++i) {
		D3DXVECTOR3 systemVertexPositionTemp (systemVertices[i].x, systemVertices[i].y, systemVertices[i].z);
		float distanceCenterPointTemp = distanceSq(center, systemVertexPositionTemp);

		if (distanceCenterPointTemp>distanceCenterPoint)
			distanceCenterPoint=distanceCenterPointTemp;
	}
	float radius = sqrtf(distanceCenterPoint);

	boundingSphere.center = center;
	boundingSphere.radius = radius;
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::render(int firstVertex, int numTriangles) const {
	assert( vertexBuffer && indexBuffer );

	// Set index buffer in device
	Global::device->SetIndices( indexBuffer );

	// Activate Stream and Vertex Declaration (FVF NOT USED)
	activateStream( 0 );
	Global::device->SetVertexDeclaration( VertexType::getVertexDeclaration() );

	int numTrianglesToRender = numTriangles;
	if(numTriangles==-1)
		numTrianglesToRender=numTotalTriangles;

	// Draw
	HRESULT hr = Global::device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST
		, 0
		, 0
		, numTotalVertices
		, firstVertex
		, numTrianglesToRender);

	assert( hr == D3D_OK );
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::renderStrip(int index) const {
	assert( index >=0 && index < (int)stripVector.size( ) );
	render(stripVector[index]->firstVertex, stripVector[index]->numTriangles);
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::renderStripTriangles(int index, int numTriangles) const {
	assert( index >=0 && index < (int)stripVector.size( ) );
	assert( numTriangles <= stripVector[index]->numTriangles );
	assert( numTriangles > 0 );
	render(stripVector[index]->firstVertex, numTriangles);
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::allocate( int pNumTotalVertices, int pNumTotalTriangles ) {
	numTotalVertices = pNumTotalVertices;
	numTotalTriangles = pNumTotalTriangles;

	freeSystemMemory( ); 

	systemVertices = new VertexType[ numTotalVertices ];
	systemIndexes = new IndexType[ 3*numTotalTriangles ];
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::freeSystemMemory( ) {
	if( systemVertices )
		delete [] systemVertices, systemVertices = NULL;
	if( systemIndexes )
		delete [] systemIndexes, systemIndexes = NULL;
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::loadToVRAM( ) {

	// Tri Stripper -> Do not strip anything
	/*triangle_stripper::indices indexVector;
	for(int i=0; i<3*numTotalTriangles; ++i){
		indexVector.push_back(systemIndexes[i]);
	}
	triangle_stripper::tri_stripper stripper(indexVector);
	stripper.SetCacheSize(24);

	triangle_stripper::primitive_vector primVector;
	stripper.Strip(&primVector);

	if(systemIndexes)
		delete[] systemIndexes, systemIndexes=NULL;
	triangle_stripper::indices iVec = primVector[0].Indices;
	numTotalTriangles = ((int)iVec.size())/3;
	systemIndexes = new IndexType[3*numTotalTriangles];
	for(int i=0; i<3*numTotalTriangles; ++i) {
		systemIndexes[i]=(IndexType)iVec[i];
	}*/



	// Free vertex and index buffer
	freeVRAM();

	// Create the vertex buffer.
	if( FAILED( Global::device->CreateVertexBuffer( numTotalVertices*sizeof(VertexType),
		D3DUSAGE_WRITEONLY,
		0, // FVF NOT USED
		D3DPOOL_DEFAULT, &vertexBuffer, NULL ) ) )
	{
		return;
	}

	// WARNING
	// if IndexType is short -> D3DFMT_INDEX16
	// if it is int or long  -> D3DFMT_INDEX32

	// Create the index buffer.
	if( FAILED( Global::device->CreateIndexBuffer( 3*numTotalTriangles*sizeof(IndexType),
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, 
		&indexBuffer, NULL ) ) )
	{
		return;
	}

	assert( systemVertices );

	// Fill the vertex buffer.
	VertexType* pVertices;
	if( FAILED( vertexBuffer->Lock( 0,		// Fill from start of the buffer
		numTotalVertices*sizeof(VertexType),// Size of the data to load
		(void**)&pVertices,					// Returned index data
		0 ) ) )								// Send default flags to the lock
	{
		return;
	}
	memcpy( pVertices, systemVertices, numTotalVertices*sizeof(VertexType) );
	vertexBuffer->Unlock();

	assert( systemIndexes );

	// Fill the index buffer.
	IndexType* pIndices;
	if( FAILED( indexBuffer->Lock( 0,			// Fill from start of the buffer
		3*numTotalTriangles*sizeof(IndexType),	// Size of the data to load
		(void**)&pIndices,						// Returned index data
		0 ) ) )									// Send default flags to the lock
	{
		return;
	}
	memcpy( pIndices, systemIndexes, 3*numTotalTriangles*sizeof(IndexType) );
	indexBuffer->Unlock();



	// Strip the system memory mesh
	/*SetCacheSize(CACHESIZE_GEFORCE3);
	PrimitiveGroup* primGroups = NULL;
	PrimitiveGroup* primGroups2 = NULL;

	// Generate Strips
	unsigned short numGroups = 0;
	bool correct = GenerateStrips(systemIndexes, 3*numTotalTriangles, &primGroups2, &numGroups);

	if(correct) {
		// Remap indices
		//RemapIndices(primGroups, numGroups, numTotalVertices, &primGroups2);

		// Update number of triangles (strip triangles is 2 less than indices)
		numTotalTriangles = primGroups2[0].numIndices - 2;
		stripVector[0]->numTriangles=numTotalTriangles;

		// Delete and set the new indices
		if(systemIndexes)
			delete[] systemIndexes, systemIndexes=NULL;
		systemIndexes = new IndexType[ primGroups2[0].numIndices ];

		for(unsigned int i=0; i<primGroups2[0].numIndices; ++i)
			systemIndexes[i] = (primGroups2[0].indices)[i];
	}

	// Delete information
	if(primGroups)
		delete[] primGroups, primGroups=NULL;
	if(primGroups2)
		delete[] primGroups2, primGroups2=NULL;

	int numDegeneratedTriangles=0;
	for(int i=1; i<numTotalTriangles+2; ++i)
		if( systemIndexes[i] == systemIndexes[i-1] )
			numDegeneratedTriangles++;
	char msg[ 128 ];
	sprintf_s(msg, sizeof(msg), "Number of Degenerated Triangles: %d\n", numDegeneratedTriangles);
	printDebugString(msg);

	// Free vertex and index buffer
	freeVRAM();

	// Create the vertex buffer.
	if( FAILED( Global::device->CreateVertexBuffer( numTotalVertices*sizeof(VertexType),
		D3DUSAGE_WRITEONLY,
		0, // FVF NOT USED
		D3DPOOL_DEFAULT, &vertexBuffer, NULL ) ) )
	{
		return;
	}

	// WARNING
	// if IndexType is short -> D3DFMT_INDEX16
	// if it is int or long -> D3DFMT_INDEX32

	// Create the index buffer.
	if( FAILED( Global::device->CreateIndexBuffer( (numTotalTriangles+2)*sizeof(IndexType),
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, 
		&indexBuffer, NULL ) ) )
	{
		return;
	}

	assert( systemVertices );

	// Fill the vertex buffer.
	VertexType* pVertices;
	if( FAILED( vertexBuffer->Lock( 0,		// Fill from start of the buffer
		numTotalVertices*sizeof(VertexType),// Size of the data to load
		(void**)&pVertices,					// Returned index data
		0 ) ) )								// Send default flags to the lock
	{
		return;
	}
	memcpy( pVertices, systemVertices, numTotalVertices*sizeof(VertexType) );
	vertexBuffer->Unlock();

	assert( systemIndexes );

	// Fill the index buffer.
	IndexType* pIndices;
	if( FAILED( indexBuffer->Lock( 0,			// Fill from start of the buffer
		(numTotalTriangles+2)*sizeof(IndexType),	// Size of the data to load
		(void**)&pIndices,						// Returned index data
		0 ) ) )									// Send default flags to the lock
	{
		return;
	}
	memcpy( pIndices, systemIndexes, (numTotalTriangles+2)*sizeof(IndexType) );
	indexBuffer->Unlock();*/
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::freeVRAM( ) {
	if( vertexBuffer )
		vertexBuffer->Release( ), vertexBuffer = NULL;
	if( indexBuffer )
		indexBuffer->Release( ), indexBuffer = NULL;
}

template<class VertexType, class IndexType>
void * CoreMesh<VertexType,IndexType>::getRawVertices( ) const { 
	assert( systemVertices );
	return systemVertices;
}

template<class VertexType, class IndexType>
void * CoreMesh<VertexType,IndexType>::getRawIndexes() const {
	assert( systemIndexes );
	return systemIndexes;
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::activateStream( int streamNumber ) const {
	Global::device->SetStreamSource( streamNumber, vertexBuffer, 0, sizeof(VertexType) );
}

#endif //COREMESHIMPL_H
