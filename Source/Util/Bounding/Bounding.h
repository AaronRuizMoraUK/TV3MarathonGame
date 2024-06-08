#ifndef BOUNDING_H
#define BOUNDING_H

#include "Directx/Directx.h"

struct BoundingBox {
	D3DXVECTOR3 min_point;
	D3DXVECTOR3 max_point;

	BoundingBox( ) : min_point(0,0,0), max_point(0,0,0) {}
	BoundingBox( const D3DXVECTOR3 &amin_point, const D3DXVECTOR3 &amax_point )
		: min_point( amin_point ), max_point( amax_point ) {}
	virtual ~BoundingBox( ) {}

	bool isPointInside( const D3DXVECTOR3 &point ) const;

	void renderDebug( ) const;
};

struct BoundingSphere {
	D3DXVECTOR3 center;
	float       radius;

	BoundingSphere( ) : center(0,0,0), radius(0.0f) {}
	BoundingSphere( const D3DXVECTOR3 &acenter, float aradius ) : center( acenter ), radius( aradius ) {}
	virtual ~BoundingSphere( ) {}

	bool isPointInside( const D3DXVECTOR3 &point ) const;

	void renderDebug( ) const;
};

#endif // BOUNDING_H
