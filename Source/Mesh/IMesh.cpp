#include "IMesh.h"
#include "Global/Rendering.h"

IMesh::IMesh(int pNumTotalVertices, int pNumTotalTriangles)
:numTotalVertices(pNumTotalVertices)
,numTotalTriangles(pNumTotalTriangles)
{
	// TODO Not Initialice this way
	TStrip *strip = new TStrip();
	strip->firstVertex=0;
	strip->numTriangles=numTotalTriangles;
	stripVector.push_back(strip);
}

IMesh::~IMesh() {
	printDebugString("Destroying Mesh...\n");

	// Destroy strip vector
	while(!stripVector.empty()) {
		size_t size = stripVector.size()-1;
		TStrip * strip= stripVector[size];
		if(strip)
			delete strip, strip=NULL;
		stripVector.pop_back();
	}
}

void IMesh::fixBoundingBoxAndSphereForCal3D( ) {
	// Fix Bounding Box and Sphere, so rotate center -90º in X (For Cal3d objects)
	D3DXMATRIX fixCal3dMatrix;
	D3DXMatrixIdentity( &fixCal3dMatrix );
	D3DXMatrixRotationX( &fixCal3dMatrix, -D3DX_PI/2.0f);

	D3DXVec3TransformCoord( &boundingBox.min_point, &boundingBox.min_point, &fixCal3dMatrix );
	D3DXVec3TransformCoord( &boundingBox.max_point, &boundingBox.max_point, &fixCal3dMatrix );
	D3DXVec3TransformCoord( &boundingSphere.center, &boundingSphere.center, &fixCal3dMatrix );
}
