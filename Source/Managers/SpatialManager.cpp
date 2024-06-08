#include <cassert>
#include "SpatialManager.h"
#include "Global/Rendering.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"

SpatialManager::SpatialID::SpatialID( )
: type( NON_SPATIAL_FIX )
, grid( 0 )
{
}

SpatialManager::SpatialID::SpatialID( Type atype, size_t agrid )
: type( atype )
, grid( agrid )
{
	assert( type != NON_SPATIAL_FIX );
	assert( grid < num_grids );
}

// ---------------------------------------------------------------------------------
const float SpatialManager::max_time_update_spatial = 0.3f;
const float SpatialManager::alpha_divisor = 2.0f * D3DX_PI / (float)num_alpha_divisions;

SpatialManager::SpatialManager( )
: time_update_spatial( 0.0f )
{
	movable_objects_grids.resize( num_grids );
	spatial_fix_objects_grids.resize( num_grids );
}

SpatialManager::~SpatialManager() {
	printDebugString("Destroying SpatialManager...\n");
	// No es necesario llamar a clear ya que no hay ningun puntero
}

void SpatialManager::clear( ) {
	movable_objects_grids.resize( 0 );
	movable_objects_grids.resize( num_grids );

	spatial_fix_objects_grids.resize( 0 );
	spatial_fix_objects_grids.resize( num_grids );

	non_spatial_fix_objects.clear( );
	all_objects.clear( );
}

void SpatialManager::update( float deltaTime ) {
	assert( movable_objects_grids.size( )     == num_grids );
	assert( spatial_fix_objects_grids.size( ) == num_grids );

	time_update_spatial += deltaTime;
	if( time_update_spatial >= max_time_update_spatial ) {
		time_update_spatial -= max_time_update_spatial;

		// Update Movable Objects
		typedef std::map< IntelligentObject *, size_t > Changes;
		Changes changes;

		// Each Grid
		MovableObjectsGrids::iterator itGrid = movable_objects_grids.begin( );
		size_t idx = 0;
		while( itGrid != movable_objects_grids.end( ) ) {

			// Each Movable Object
			MovableObjects::iterator itObject = itGrid->begin( );
			while( itObject != itGrid->end( ) ) {
				IntelligentObject *movable_object = *itObject;
				size_t new_grid = getGridOfObject( movable_object );

				// Si hemos cambiado de grid
				if( idx != new_grid ) {
					changes[ movable_object ] = new_grid;          // Guardar el cambio para añadirlo luego al grid nuevo
					itObject = itGrid->erase( itObject );          // Borrarlo del grid

					//changeColor( movable_object, new_grid );
				}
				else
					++itObject;
			}

			++itGrid;
			++idx;
		}

		// Aplicar cambios
		Changes::iterator itChange = changes.begin( );
		while( itChange != changes.end( ) ) {
			IntelligentObject *movable_object = itChange->first;
			size_t new_grid= itChange->second;
			movable_objects_grids[ new_grid ].insert( movable_object ); // Aplicar cambio

			assert( all_objects[ movable_object ].type == SpatialID::MOVABLE );
			all_objects[ movable_object ].grid = new_grid; // Aplicar cambio en all_objects

			++itChange;
		}
		// changes.clear( ); // No es necesario, ya que es local
	}
}

void SpatialManager::addMovableObject( IntelligentObject *movable_object ) {
	size_t grid = getGridOfObject( movable_object );
	movable_objects_grids[ grid ].insert( movable_object );
	all_objects[ movable_object ] = SpatialID( SpatialID::MOVABLE, grid );

	//changeColor( movable_object, grid );
}

void SpatialManager::addSpatialFixObject( Object *object ) {
	size_t grid = getGridOfObject( object );
	spatial_fix_objects_grids[ grid ].insert( object );
	all_objects[ object ] = SpatialID( SpatialID::SPATIAL_FIX, grid );

	//changeColor( object, grid );
}

void SpatialManager::addNonSpatialFixObject( Object *object ) {
	non_spatial_fix_objects.insert( object );
	all_objects[ object ] = SpatialID( );
}

void SpatialManager::removeObject( Object *object ) {
	AllObjects::iterator it = all_objects.find( object );
	if( it == all_objects.end( ) )
		return;

	const SpatialID &spatial_id = it->second;
	switch( spatial_id.type ) {
		case SpatialID::MOVABLE:
			movable_objects_grids[ spatial_id.grid ].erase( (IntelligentObject *)object );
			break;

		case SpatialID::SPATIAL_FIX:
			spatial_fix_objects_grids[ spatial_id.grid ].erase( object );
			break;

		case SpatialID::NON_SPATIAL_FIX:
			non_spatial_fix_objects.erase( object );
			break;

		default:
			assert( !"This should not happend" );
			return;
	}

	all_objects.erase( it );
}

size_t SpatialManager::getGridOfObject( const Object *object ) const {
	assert( object );
	float alpha = object->loc.getAlpha( );
	float delta = object->loc.getDelta( );

	// Segun Delta
	size_t delta_grid = ( delta < 0 ) ? 0 : num_alpha_divisions;

	// Segun Alpha
	size_t alpha_grid = (size_t) (alpha / alpha_divisor);
	if( alpha_grid == num_alpha_divisions ) // Caso justo en que alpha == 360
		return delta_grid;

	size_t grid = delta_grid + alpha_grid;
	assert( grid < num_grids );
	return grid;
}

SpatialManager::MovableObjects &SpatialManager::getMovableObjects( size_t grid ) {
	assert( grid < num_grids );
	return movable_objects_grids[grid];
}

SpatialManager::SpatialFixObjects &SpatialManager::getSpatialFixObjects( size_t grid ) {
	assert( grid < num_grids );
	return spatial_fix_objects_grids[grid];
}

size_t SpatialManager::sizeMovableObjects( size_t grid ) const {
	assert( grid < num_grids );
	return movable_objects_grids[grid].size( );
}

size_t SpatialManager::sizeSpatialFixObjects( size_t grid ) const {
	assert( grid < num_grids );
	return spatial_fix_objects_grids[grid].size( );
}

void SpatialManager::changeColor( Object *object, size_t grid ) {
	if( grid == 0 )
		object->getMaterial( )->enableAdditionalColor( 0xffff0000 );
	else if( grid == 1 )
		object->getMaterial( )->enableAdditionalColor( 0xff00ff00 );
	else if( grid == 2 )
		object->getMaterial( )->enableAdditionalColor( 0xff0000ff );
	else if( grid == 3 )
		object->getMaterial( )->enableAdditionalColor( 0xffffff00 );
	else if( grid == 4 )
		object->getMaterial( )->enableAdditionalColor( 0xff00ffff );
	else if( grid == 5 )
		object->getMaterial( )->enableAdditionalColor( 0xffff00ff );
	else if( grid == 6 )
		object->getMaterial( )->enableAdditionalColor( 0xff888888 );
	else if( grid == 7 )
		object->getMaterial( )->enableAdditionalColor( 0xffffffff );
	else if( grid == 8 )
		object->getMaterial( )->enableAdditionalColor( 0xff000000 );
	else if( grid == 9 )
		object->getMaterial( )->enableAdditionalColor( 0xffff3377 );
}
