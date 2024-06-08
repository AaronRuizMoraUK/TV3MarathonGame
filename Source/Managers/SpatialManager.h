#ifndef SPATIAL_MANAGER_H
#define SPATIAL_MANAGER_H

#include <set>
#include <vector>
#include <map>

class Object;
class IntelligentObject;

/**
* Manager with the spatial division of object in planet.
*/

class SpatialManager
{
public:
	typedef std::set< IntelligentObject * > MovableObjects;
	typedef std::set< Object * >            SpatialFixObjects;
	typedef std::set< Object * >            NonSpatialFixObjects;

protected:
	// Update Time
	static const float max_time_update_spatial;
	float              time_update_spatial;
	
	// Divisions
	static const size_t num_alpha_divisions = 5;
	static const size_t num_grids = num_alpha_divisions * 2; // Delta is going to do one division
	static const float  alpha_divisor;

	// Spatial ID
	struct SpatialID {
		enum Type {
			MOVABLE = 10
			, SPATIAL_FIX
			, NON_SPATIAL_FIX
		};

		Type     type;
		size_t grid;

		SpatialID( );
		SpatialID( Type atype, size_t agrid );
	};

	// Movable Objects
	typedef std::vector< MovableObjects > MovableObjectsGrids;
	MovableObjectsGrids movable_objects_grids;

	// Spatial Fix Objects
	typedef std::vector< SpatialFixObjects > SpatialFixObjectsGrids;
	SpatialFixObjectsGrids spatial_fix_objects_grids;

	// Non-Spatial Fix Objects
	NonSpatialFixObjects non_spatial_fix_objects;

	// All objects in spatial
	typedef std::map< Object *, SpatialID> AllObjects;
	AllObjects all_objects;

public:
	SpatialManager( );
	~SpatialManager( );
	void clear( );

	void update( float deltaTime );

	void addMovableObject( IntelligentObject *movable_object );
	void addSpatialFixObject( Object *object );
	void addNonSpatialFixObject( Object *object );
	void removeObject( Object *object );

	size_t getGridOfObject( const Object *object ) const;

	MovableObjects       &getMovableObjects( size_t grid );
	SpatialFixObjects    &getSpatialFixObjects( size_t grid );
	NonSpatialFixObjects &getNonSpatialFixObjects( ) { return non_spatial_fix_objects; }

	inline static size_t getNumGrids( ) { return num_grids; }
	inline size_t sizeNonSpatialFixObjects( ) const { return non_spatial_fix_objects.size( ); }
	inline size_t sizeAllObjects( ) const { return all_objects.size( ); }
	size_t sizeMovableObjects( size_t grid ) const;
	size_t sizeSpatialFixObjects( size_t grid ) const;

private:
	void changeColor( Object *object, size_t grid );
};

#endif //SPATIAL_MANAGER_H
