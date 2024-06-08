#include "CollisionManager.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"

CollisionManager::Collision::Collision(Object *aobject1, CollisionType aobject1_type
																			 , Object *aobject2, CollisionType aobject2_type
																			 , const D3DXVECTOR3 &apoint1, const D3DXVECTOR3 &apoint2)
																			 : object1(aobject1)
																			 , object1_type( aobject1_type )
																			 , object2(aobject2)
																			 , object2_type( aobject2_type )
																			 , point1(apoint1)
																			 , point2(apoint2)
{
}

// -------------------------------------------------------------
bool CollisionManager::collisions_enabled = true;

CollisionManager::~CollisionManager() {
	printDebugString("Destroying CollisionManager...\n");
	collisions.clear( );
}

void CollisionManager::update( float deltaTime ) {
	if( !collisions_enabled )
		return;

	detectCollisions( );
	treatCollisions( );
	collisions.clear( );
}

void CollisionManager::detectCollisions( ) {

	// Detect collisions between movable objects
	for( size_t i=0; i<SpatialManager::getNumGrids( ); ++i ) {
		SpatialManager::MovableObjects &movable_objects = Global::spatial.getMovableObjects( i );
		if( movable_objects.size( ) < 2 )
			continue;

		// Doble recorrido por los objectos del grid
		SpatialManager::MovableObjects::iterator it1   = movable_objects.begin( );
		SpatialManager::MovableObjects::iterator itEnd = movable_objects.end( );
		while( it1 != itEnd ) {
			// Si el objecto del mundo no es collisionable pasamos al siguiente
			DT_ObjectHandle it1_co = (*it1)->loc.getCollisionObject( );
			if( it1_co == NULL ) {
				++it1;
				continue;
			}

			SpatialManager::MovableObjects::iterator it2 = it1;
			++it2; // Next
			while( it2 != itEnd ) {
				// Si el objecto del mundo no es collisionable pasamos al siguiente
				DT_ObjectHandle it2_co = (*it2)->loc.getCollisionObject( );
				if( it2_co == NULL ) {
					++it2;
					continue;
				}

				// Detectar collision
				D3DXVECTOR3 point1;
				D3DXVECTOR3 point2;
				DT_Bool existCollision = DT_GetPenDepth( it1_co, it2_co, point1, point2 );
				if( existCollision )
					collisions.push_back( Collision( *it1, MOVABLE_OBJECT, *it2, MOVABLE_OBJECT, point1, point2 ) );

				++it2;
			}

			++it1;
		}
	}

	// -------------------------------------------------------------
	// Detect collisions between player and movable objects
	size_t player_grid = Global::spatial.getGridOfObject( Global::player );
	DT_ObjectHandle player_co = Global::player->loc.getCollisionObject( );
	assert( player_co );

	SpatialManager::MovableObjects &movable_objects_pg = Global::spatial.getMovableObjects( player_grid );
	SpatialManager::MovableObjects::iterator it_pg     = movable_objects_pg.begin( );
	while( it_pg != movable_objects_pg.end( ) ) {
		// Si el objecto del mundo no es collisionable pasamos al siguiente
		DT_ObjectHandle it_pg_co = (*it_pg)->loc.getCollisionObject( );
		if( it_pg_co == NULL ) {
			++it_pg;
			continue;
		}

		// Detectar collision
		D3DXVECTOR3 point1;
		D3DXVECTOR3 point2;
		DT_Bool existCollision = DT_GetPenDepth( player_co, it_pg_co, point1, point2 );
		if( existCollision )
			collisions.push_back( Collision( Global::player, MOVABLE_OBJECT, *it_pg, MOVABLE_OBJECT, point1, point2 ) );

		++it_pg;
	}

	// -------------------------------------------------------------
	// Detect collisions of player and movable objects with spatial fix objects
	for( size_t i=0; i<SpatialManager::getNumGrids( ); ++i ) {
		SpatialManager::SpatialFixObjects &spatial_fix_objects = Global::spatial.getSpatialFixObjects( i );
		if( spatial_fix_objects.empty( ) )
			continue;

		SpatialManager::MovableObjects &movable_objects = Global::spatial.getMovableObjects( i );

		SpatialManager::SpatialFixObjects::iterator it2 = spatial_fix_objects.begin( );
		while( it2 != spatial_fix_objects.end( ) ) {
			// Si el objecto del mundo no es collisionable pasamos al siguiente
			DT_ObjectHandle it2_co = (*it2)->loc.getCollisionObject( );
			if( it2_co == NULL ) {
				++it2;
				continue;
			}

			SpatialManager::MovableObjects::iterator it1 = movable_objects.begin( );
			while( it1 != movable_objects.end( ) ) {
				// Si el objecto del mundo no es collisionable pasamos al siguiente
				DT_ObjectHandle it1_co = (*it1)->loc.getCollisionObject( );
				if( it1_co == NULL ) {
					++it1;
					continue;
				}

				// Detectar collision
				D3DXVECTOR3 point1;
				D3DXVECTOR3 point2;
				DT_Bool existCollision = DT_GetPenDepth( it1_co, it2_co, point1, point2 );
				if( existCollision )
					collisions.push_back( Collision( *it1, MOVABLE_OBJECT, *it2, SPATIAL_FIX_OBJECT, point1, point2 ) );

				++it1;
			}

			// Player
			if( player_grid == i ) {
				// Detectar collision
				D3DXVECTOR3 point1;
				D3DXVECTOR3 point2;
				DT_Bool existCollision = DT_GetPenDepth( player_co, it2_co, point1, point2 );
				if( existCollision )
					collisions.push_back( Collision( Global::player, MOVABLE_OBJECT, *it2, SPATIAL_FIX_OBJECT, point1, point2 ) );
			}

			++it2;
		}
	}

	// -------------------------------------------------------------
	// Detect collisions of player and movable objects with non spatial fix objects
	SpatialManager::NonSpatialFixObjects &non_spatial_fix_objects = Global::spatial.getNonSpatialFixObjects( );
	SpatialManager::NonSpatialFixObjects::iterator it2_nsfo = non_spatial_fix_objects.begin( );
	while( it2_nsfo != non_spatial_fix_objects.end( ) ) {
		// Si el objecto del mundo no es collisionable pasamos al siguiente
		DT_ObjectHandle it2_nsfo_co = (*it2_nsfo)->loc.getCollisionObject( );
		if( it2_nsfo_co == NULL ) {
			++it2_nsfo;
			continue;
		}

		for( size_t i=0; i<SpatialManager::getNumGrids( ); ++i ) {
			SpatialManager::MovableObjects &movable_objects = Global::spatial.getMovableObjects( i );
			SpatialManager::MovableObjects::iterator it1 = movable_objects.begin( );
			while( it1 != movable_objects.end( ) ) {
				// Si el objecto del mundo no es collisionable pasamos al siguiente
				DT_ObjectHandle it1_co = (*it1)->loc.getCollisionObject( );
				if( it1_co == NULL ) {
					++it1;
					continue;
				}

				// Detectar collision
				D3DXVECTOR3 point1;
				D3DXVECTOR3 point2;
				DT_Bool existCollision = DT_GetPenDepth( it1_co, it2_nsfo_co, point1, point2 );
				if( existCollision )
					collisions.push_back( Collision( *it1, MOVABLE_OBJECT, *it2_nsfo, NON_SPATIAL_FIX_OBJECT, point1, point2 ) );

				++it1;
			}
		}

		// Detectar collision con Player
		D3DXVECTOR3 point1;
		D3DXVECTOR3 point2;
		DT_Bool existCollision = DT_GetPenDepth( player_co, it2_nsfo_co, point1, point2 );
		if( existCollision )
			collisions.push_back( Collision( Global::player, MOVABLE_OBJECT, *it2_nsfo, NON_SPATIAL_FIX_OBJECT, point1, point2 ) );

		++it2_nsfo;
	}
}

void CollisionManager::treatCollisions( ) {
	for( size_t i=0; i<collisions.size( ); ++i ) {
		Collision &collision = collisions[ i ];
		Object *obj1 = collision.object1;
		Object *obj2 = collision.object2;

		if( collision.point1 == collision.point2 )
			continue;

		// Obtener la normal
		D3DXVECTOR3 normal = collision.point2 - collision.point1;
		normalizeVector3( normal, normal );

		// Avisar object 1
		if( collision.object1_type == MOVABLE_OBJECT )
			obj1->collisionWith( *obj2, collision.object2_type, normal );

		// Avisar object 2
		if( collision.object2_type == MOVABLE_OBJECT )
			obj2->collisionWith( *obj1, collision.object1_type, -normal );
	}
}
