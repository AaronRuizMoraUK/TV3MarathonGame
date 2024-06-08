#include <cassert>
#include "BackSideFrustum.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Camera/ICamera.h"
#include "Util/Bounding/Bounding.h"
#include "Managers/BrainManager.h"

BackSideFrustum::BackSideFrustum() {
	memset( frustum, 0x00, sizeof( frustum ) );
}

BackSideFrustum::~BackSideFrustum() {
	printDebugString("Destroying BackSideFrustum...\n");
}

void BackSideFrustum::update() 
{
	// Obtain clip matrix (view*projection)
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	generateBackSideMatView( matView );
	generateBackSideMatProj( matProj );

	D3DXMATRIX clip;
	D3DXMatrixMultiply(&clip, &matView, &matProj);

	// Extract the numbers for the RIGHT plane 
	frustum[0][0] = clip._14 - clip._11;
	frustum[0][1] = clip._24 - clip._21;
	frustum[0][2] = clip._34 - clip._31;
	frustum[0][3] = clip._44 - clip._41;

	// Normalize the result 
	float t = 1.0f / sqrtf( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
	frustum[0][0] *= t;
	frustum[0][1] *= t;
	frustum[0][2] *= t;
	frustum[0][3] *= t;

	// Extract the numbers for the LEFT plane 
	frustum[1][0] = clip._14 + clip._11;
	frustum[1][1] = clip._24 + clip._21;
	frustum[1][2] = clip._34 + clip._31;
	frustum[1][3] = clip._44 + clip._41;

	// Normalize the result 
	t = 1.0f / sqrtf( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
	frustum[1][0] *= t;
	frustum[1][1] *= t;
	frustum[1][2] *= t;
	frustum[1][3] *= t;

	// Extract the BOTTOM plane 
	frustum[2][0] = clip._14 + clip._12;
	frustum[2][1] = clip._24 + clip._22;
	frustum[2][2] = clip._34 + clip._32;
	frustum[2][3] = clip._44 + clip._42;

	// Normalize the result 
	t = 1.0f / sqrtf( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
	frustum[2][0] *= t;
	frustum[2][1] *= t;
	frustum[2][2] *= t;
	frustum[2][3] *= t;

	// Extract the TOP plane 
	frustum[3][0] = clip._14 - clip._12;
	frustum[3][1] = clip._24 - clip._22;
	frustum[3][2] = clip._34 - clip._32;
	frustum[3][3] = clip._44 - clip._42;

	// Normalize the result 
	t = 1.0f / sqrtf( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
	frustum[3][0] *= t;
	frustum[3][1] *= t;
	frustum[3][2] *= t;
	frustum[3][3] *= t;

	// Extract the FAR plane 
	frustum[4][0] = clip._14 - clip._13;
	frustum[4][1] = clip._24 - clip._23;
	frustum[4][2] = clip._34 - clip._33;
	frustum[4][3] = clip._44 - clip._43;

	// Normalize the result 
	t = 1.0f / sqrtf( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
	frustum[4][0] *= t;
	frustum[4][1] *= t;
	frustum[4][2] *= t;
	frustum[4][3] *= t;

	// Extract the NEAR plane 
	//frustum[5][0] = clip._14 + clip._13;
	//frustum[5][1] = clip._24 + clip._23;
	//frustum[5][2] = clip._34 + clip._33;
	//frustum[5][3] = clip._44 + clip._43;
	frustum[5][0] = clip._13;
	frustum[5][1] = clip._23;
	frustum[5][2] = clip._33;
	frustum[5][3] = clip._43;

	// Normalize the result 
	t = 1.0f / sqrtf( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
	frustum[5][0] *= t;
	frustum[5][1] *= t;
	frustum[5][2] *= t;
	frustum[5][3] *= t;
}

bool BackSideFrustum::isBoundingVisible(const BoundingSphere &bsphere) const 
{
	const D3DXVECTOR3 &center = bsphere.center;

	for (int i=0;i<6;++i)	{
		float dst = frustum[i][0]*center.x + frustum[i][1]*center.y +
			frustum[i][2]*center.z + frustum[i][3];
		if (dst <= bsphere.radius)
			return true;
	}
	return false;
}

void BackSideFrustum::renderDebug() const {
#ifndef NDEBUG
	D3DXVECTOR4 planes[6];
	for(int i=0; i<6; ++i) {
		planes[i].x=frustum[i][0];
		planes[i].y=frustum[i][1];
		planes[i].z=frustum[i][2];
		planes[i].w=frustum[i][3];
	}

	// Current planet center
	const D3DXVECTOR3 &planetCenter = BrainManager::get( )->getCurrentBrain( ).getCenter();

	//float distanceToPlanetCenter = distance(getCurrentCamera( )->getPosition(), planetCenter);
	//printf(0,70, 0xff0000ff, "Planet distance %f", distanceToPlanetCenter);

	//printf(0,100, 0xff0000ff, "Near plane (%f, %f, %f, %f)", planes[5].x,  planes[5].y,  planes[5].z,  planes[5].w);
	//printf(0,110, 0xff0000ff, "Far plane (%f, %f, %f, %f)", planes[4].x,  planes[4].y,  planes[4].z,  planes[4].w);
	//printf(0,120, 0xff0000ff, "Right plane (%f, %f, %f, %f)", planes[0].x,  planes[0].y,  planes[0].z,  planes[0].w);
	//printf(0,130, 0xff0000ff, "Left plane (%f, %f, %f, %f)", planes[1].x,  planes[1].y,  planes[1].z,  planes[1].w);
	//printf(0,140, 0xff0000ff, "Top plane (%f, %f, %f, %f)", planes[3].x,  planes[3].y,  planes[3].z,  planes[3].w);
	//printf(0,150, 0xff0000ff, "Bottom plane (%f, %f, %f, %f)", planes[2].x,  planes[2].y,  planes[2].z,  planes[2].w);

	// Point in Near plane
	D3DXVECTOR3 nearPoint;
	pointInPlane(nearPoint, planetCenter, planes[5]);
	//printf(0,80, 0xff0000ff, "Near point (%f, %f, %f)", nearPoint.x, nearPoint.y, nearPoint.z);

	// Point in Far plane
	D3DXVECTOR3 farPoint;
	pointInPlane(farPoint, planetCenter, planes[4]);
	//printf(0,90, 0xff0000ff, "Far point (%f, %f, %f)", farPoint.x, farPoint.y, farPoint.z);

	// Points of right and left planes in near and far planes
	D3DXVECTOR3 pointsInPlanes[4];
	for(int i=0; i<2; ++i) {
		pointInPlane(pointsInPlanes[i], nearPoint, planes[i]);
		pointInPlane(pointsInPlanes[i+2], farPoint, planes[i]);
	}

	// Obtain the 8 points OK for drawing
	D3DXVECTOR3 pointsInPlanesOK[8];
	pointInPlane(pointsInPlanesOK[0], pointsInPlanes[0], planes[3]);
	pointInPlane(pointsInPlanesOK[1], pointsInPlanes[1], planes[3]);
	pointInPlane(pointsInPlanesOK[2], pointsInPlanes[1], planes[2]);
	pointInPlane(pointsInPlanesOK[3], pointsInPlanes[0], planes[2]);
	pointInPlane(pointsInPlanesOK[4], pointsInPlanes[2], planes[3]);
	pointInPlane(pointsInPlanesOK[5], pointsInPlanes[3], planes[3]);
	pointInPlane(pointsInPlanesOK[6], pointsInPlanes[3], planes[2]);
	pointInPlane(pointsInPlanesOK[7], pointsInPlanes[2], planes[2]);
	//printf(0,160, 0xff0000ff, "Near point rigth up   (%f, %f, %f)", pointsInPlanesOK[0].x, pointsInPlanesOK[0].y, pointsInPlanesOK[0].z);
	//printf(0,170, 0xff0000ff, "Near point left up    (%f, %f, %f)", pointsInPlanesOK[1].x, pointsInPlanesOK[1].y, pointsInPlanesOK[1].z);
	//printf(0,180, 0xff0000ff, "Near point rigth down (%f, %f, %f)", pointsInPlanesOK[2].x, pointsInPlanesOK[2].y, pointsInPlanesOK[2].z);
	//printf(0,190, 0xff0000ff, "Near point left down  (%f, %f, %f)", pointsInPlanesOK[3].x, pointsInPlanesOK[3].y, pointsInPlanesOK[3].z);
	//printf(0,200, 0xff0000ff, "Far point rigth up   (%f, %f, %f)", pointsInPlanesOK[4].x, pointsInPlanesOK[4].y, pointsInPlanesOK[4].z);
	//printf(0,210, 0xff0000ff, "Far point left up    (%f, %f, %f)", pointsInPlanesOK[5].x, pointsInPlanesOK[5].y, pointsInPlanesOK[5].z);
	//printf(0,220, 0xff0000ff, "Far point rigth down (%f, %f, %f)", pointsInPlanesOK[6].x, pointsInPlanesOK[6].y, pointsInPlanesOK[6].z);
	//printf(0,230, 0xff0000ff, "Far point left down  (%f, %f, %f)", pointsInPlanesOK[7].x, pointsInPlanesOK[7].y, pointsInPlanesOK[7].z);

	// Print real near point
	drawLine(nearPoint, nearPoint+5*getCurrentCamera( )->getUpVector(), 0xFFFF0000);

	// Print near and far lines
	for(int i=0; i<4; ++i) {
		drawLine(pointsInPlanesOK[i], pointsInPlanesOK[(i+1)%4], 0xFFFF0000);
		drawLine(pointsInPlanesOK[i+4], pointsInPlanesOK[((i+1)%4)+4], 0xFFFF0000);
	}

	// Print lines to unit near and far points
	for(int i=0; i<4; ++i)
		drawLine(pointsInPlanesOK[i], pointsInPlanesOK[i+4], 0xFFFF0000);
#endif
}

void BackSideFrustum::generateBackSideMatView(D3DXMATRIX &out) const
{
	const D3DXVECTOR3 &cameraPosition = getCurrentCamera( )->getPosition();
	const D3DXVECTOR3 &cameraUp = getCurrentCamera( )->getLocalUpVector();
	const D3DXVECTOR3 &planetCenter = BrainManager::get( )->getCurrentBrain( ).getCenter();

	D3DXMatrixLookAtRH(&out, &cameraPosition, &planetCenter, &cameraUp);
}

void BackSideFrustum::generateBackSideMatProj(D3DXMATRIX &out) const
{
	const D3DXVECTOR3 &cameraPosition = getCurrentCamera( )->getPosition();

	// Current planet center and radius
	const D3DXVECTOR3 &planetCenter = BrainManager::get( )->getCurrentBrain( ).getCenter();
	float planetRadius = BrainManager::get( )->getCurrentBrain( ).getBackSideFrustumRadius( );

	float frustumProfundity = planetRadius + getCurrentCamera( )->getZFar();
	float aspectRatio = 1.0f;

	float distanceToPlanetCenter = distance(cameraPosition, planetCenter);
	float upPart = planetRadius/sqrtf(2.0f);
	float fov = 2.0f * atanf( upPart / distanceToPlanetCenter );
	float zn = distanceToPlanetCenter;
	float zf = distanceToPlanetCenter + frustumProfundity;

	D3DXMatrixPerspectiveFovRH(&out, fov, aspectRatio, zn, zf );
}

void BackSideFrustum::pointInPlane(D3DXVECTOR3 &newPoint, const D3DXVECTOR3 &point, 
								   const D3DXVECTOR4 &plane) const
{
	// t value
	assert( (plane.x*plane.x + plane.y*plane.y + plane.z*plane.z) != 0.0f );
	float t = -(plane.x*point.x + plane.y*point.y + plane.z*point.z + plane.w) / (plane.x*plane.x + plane.y*plane.y + plane.z*plane.z);

	// Point in plane
	newPoint.x = point.x + t*plane.x;
	newPoint.y = point.y + t*plane.y;
	newPoint.z = point.z + t*plane.z;
}
