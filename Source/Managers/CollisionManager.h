#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include <vector>
#include "Directx/Directx.h"

class Object;

/**
* Manager of collision.
*/

enum CollisionType {
	MOVABLE_OBJECT = 0
	, SPATIAL_FIX_OBJECT
	, NON_SPATIAL_FIX_OBJECT
};

class CollisionManager
{
public:
	static bool collisions_enabled;

protected:
	struct Collision {
		Object *object1;
		CollisionType object1_type;

		Object *object2;
		CollisionType object2_type;

		D3DXVECTOR3 point1;
		D3DXVECTOR3 point2;

		Collision(Object *aobject1, CollisionType aobject1_type
			, Object *aobject2, CollisionType aobject2_type
			, const D3DXVECTOR3 &apoint1, const D3DXVECTOR3 &apoint2);
	};

	typedef std::vector<Collision> Collisions;
	Collisions collisions;

public:
	CollisionManager( ) {}
	~CollisionManager( );

	void update( float deltaTime );

protected:
	void detectCollisions( );
	void treatCollisions( );
};

#endif //COLLISION_MANAGER_H
