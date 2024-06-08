#ifndef TRACKER_H
#define TRACKER_H

#include "Directx/Directx.h"

class TLocation;

struct TTracker {
private:
	D3DXVECTOR3      fixed_position;
	D3DXMATRIX       fixed_transform;
	const TLocation *pointer_loc;

	enum Type {
		FIXED_TRANSFORM
		, POINTER_LOC
	};
	Type type;

public:
	TTracker( );
	TTracker( const D3DXVECTOR3 &afixed_position );
	TTracker( const D3DXMATRIX &afixed_transform );
	TTracker( const TLocation  *apointer_loc );

	const D3DXMATRIX &getTransform( ) const;
	const D3DXVECTOR3 &getPosition( ) const;
};

#endif
