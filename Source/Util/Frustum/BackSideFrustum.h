#ifndef BACK_SIDE_FRUSTUM_H
#define BACK_SIDE_FRUSTUM_H

#include "Directx/Directx.h"

struct BoundingSphere;

/**
* Frustum to avoid drawing anything that is
* in the back side of the planet.
*/

class BackSideFrustum {
protected:
	float frustum[6][4];

public:	
	BackSideFrustum();
	~BackSideFrustum();

	void update();
	bool isBoundingVisible( const BoundingSphere &bsphere ) const;

	void renderDebug() const;
	
private:
	void generateBackSideMatView(D3DXMATRIX &out) const;
	void generateBackSideMatProj(D3DXMATRIX &out) const;

	void pointInPlane(D3DXVECTOR3 &newPoint, const D3DXVECTOR3 &point, const D3DXVECTOR4 &plane) const;
};

#endif //BACK_SIDE_FRUSTUM_H
