#include "Frustum.h"
#include "Global/Rendering.h"
#include "Camera/Camera3D.h"
#include "Util/Bounding/Bounding.h"

Frustum::Frustum() {
	memset( frustum, 0x00, sizeof( frustum ) );
}

Frustum::~Frustum() {
	printDebugString("Destroying Frustum...\n");
}

void Frustum::update() {
	// Obtain clip matrix (view*projection)
	D3DXMATRIX clip;
	const D3DXMATRIX &matView = getCamera3D( ).getMatView();
	const D3DXMATRIX &matProj = getCamera3D( ).getMatProj();
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

bool Frustum::isBoundingVisible( const BoundingSphere &bsphere ) const {
	const D3DXVECTOR3 &center = bsphere.center;

	for (int i=0;i<6;++i)	{
		float dst = frustum[i][0]*center.x + frustum[i][1]*center.y	+ 
			frustum[i][2]*center.z + frustum[i][3];

		if ( dst <= -bsphere.radius)
			return false;
	}

	return true;
}
