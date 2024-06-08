#include <cassert>
#include "Tracker.h"
#include "Object/Location.h"

TTracker::TTracker( )
: fixed_position(0,0,0)
, pointer_loc( NULL )
, type( FIXED_TRANSFORM )
{
	D3DXMatrixIdentity( &fixed_transform );
}

TTracker::TTracker( const D3DXVECTOR3 &afixed_position )
: fixed_position(afixed_position)
, pointer_loc( NULL )
, type( FIXED_TRANSFORM )
{
	D3DXMatrixTranslation( &fixed_transform, fixed_position.x, fixed_position.y, fixed_position.z );
}

TTracker::TTracker( const D3DXMATRIX &afixed_transform )
: fixed_position( afixed_transform(3,0), afixed_transform(3,1), afixed_transform(3,2) )
, fixed_transform( afixed_transform )
, pointer_loc( NULL )
, type( FIXED_TRANSFORM )
{
}

TTracker::TTracker( const TLocation *apointer_loc )
: fixed_position(0,0,0)
, pointer_loc( apointer_loc )
, type( POINTER_LOC )
{
	assert( pointer_loc );
}

const D3DXMATRIX &TTracker::getTransform( ) const {
	switch( type ) {
		case FIXED_TRANSFORM:
			return fixed_transform;
			break;

		case POINTER_LOC:
			assert( pointer_loc );
			return pointer_loc->getTransform( );
			break;

		default:
			assert( !"This should not happend!" );
			return fixed_transform;
			break;
	}
}

const D3DXVECTOR3 &TTracker::getPosition( ) const {
	switch( type ) {
		case FIXED_TRANSFORM:
			return fixed_position;
			break;

		case POINTER_LOC:
			assert( pointer_loc );
			return pointer_loc->getPosition( );
			break;

		default:
			assert( !"This should not happend!" );
			return fixed_position;
			break;
	}
}
