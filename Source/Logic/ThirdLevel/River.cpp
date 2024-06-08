#include "River.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Factories/MeshFactory.h"
#include "Managers/ROAResourceManager.h"
#include "Managers/BrainManager.h"
#include "AnimationControllers/ROA/ROAObject.h"
#include "Object/AnimatedObject/AnimatedObject.h"
#include "Util/Bounding/Bounding.h"
#include "Mesh/IMesh.h"
#include "Particles/PSManager.h"

River::River( )
: curr_speed_idx( 0 )
, river_particles( "" )
, start_river_point( 0,0,0 )
, end_river_point( 0,0,0 )
, first_time_loading( true )
{
}

River::~River( ) {
	printDebugString( "Destroying River...\n" );

	for( size_t i=0; i<river_elements.size( ); ++i ) {
		AnimatedObject *object = river_elements[i];
		if( object )
			delete object, object = NULL;
	}
	river_elements.clear( );
}

void River::load( ) {
	const std::string &xmlPath = Global::data_locator.getPath( "xml" );
	bool ok = xmlParseFile( xmlPath + "river_third_level.xml" );
	assert( ok );
}

void River::onStartElement (const std::string &elem, MKeyValue &atts) {
	if( elem == "speed" ) {
		float new_speed = atts.getFloat( "value", 0.0f );
		speeds.push_back( new_speed );
	}
	else if( elem == "river_elements" ) {
		// Mesh
		const IMesh *mesh = MeshFactory::get( )->createMesh( atts["mesh"] );
		assert( mesh );

		// Material
		const std::string &atts_texture   = atts["texture"];
		const std::string &atts_shader    = atts["shader"];
		const std::string &atts_technique = atts["technique"];

		// ROA Resource
		const std::string &roa_resource_name = atts["roa_resource"];
		const ROAResource *roa_resource = ROAResourceManager::get( )->getResource( roa_resource_name );

		int num_river_elements = atts.getInt( "num_river_elements", 0 );
		assert( num_river_elements > 0 );

		for( int i=0; i<num_river_elements; ++i ) {
			AnimatedObject *river_element = new AnimatedObject( );
			assert( river_element );

			// Mesh
			river_element->setMesh( mesh );

			// Material
			river_element->SETMATERIAL( atts_texture, atts_shader, atts_technique );

			// Animation Controller
			ROAObject *new_controller = new ROAObject( &roa_resource->rigidAnimationResource, true );
			river_element->setAnimationController( new_controller );

			// Add it
			river_elements.push_back( river_element );
		}
	}
	else if( elem == "particles" ) {
		river_particles = atts[ "name" ];

		// Start Position
		const std::string &start_point_text = atts[ "start_point" ];
		sscanf_s( start_point_text.c_str(), "%f %f %f", &start_river_point.x, &start_river_point.y, &start_river_point.z );

		// End Position
		const std::string &end_point_text = atts[ "end_point" ];
		sscanf_s( end_point_text.c_str(), "%f %f %f", &end_river_point.x, &end_river_point.y, &end_river_point.z );
	}
}

void River::init( ) {
	if( first_time_loading ) {
		first_time_loading = false;
		Planet &curr_brain = BrainManager::get( )->getCurrentBrain( );

		float start_alpha, start_delta;
		float end_alpha,   end_delta;
		D3DXMATRIX start_transform, end_transform;
		curr_brain.getPolarAngles( start_river_point, start_alpha, start_delta );
		curr_brain.getPolarAngles( end_river_point,   end_alpha,   end_delta );
		curr_brain.getTransformOnPlanet( start_transform, 0.0f, start_alpha, start_delta );
		curr_brain.getTransformOnPlanet( end_transform,   0.0f, end_alpha,   end_delta );
		start_river_tracker = TTracker( start_transform );
		end_river_tracker   = TTracker( end_transform );
	}

	// Crear las particulas de los puntos de nacimiento y muerte del rio
	PSManager::get( )->addParticleGroup( river_particles, start_river_tracker );
	PSManager::get( )->addParticleGroup( river_particles, end_river_tracker );

	// Move to a percentage of ROA
	for( size_t i=0; i<river_elements.size( ); ++i ) {
		((ROAObject *)river_elements[i]->getAnimationController( ))->setPercentage( i/(float)river_elements.size( ) );

		// Particles
		//ParticleGroup *pgroup = PSManager::get( )->addParticleGroup( "dopamina_particles", TTracker( &river_elements[i]->loc ) );
		//river_elements[i]->setParticleGroup( pgroup );
	}
	
	// Reset speed
	curr_speed_idx = 0;
	changeSpeed( );
}

void River::destroy( ) {
	// Remove to spatial
	for( size_t i=0; i<river_elements.size( ); ++i )
		Global::spatial.removeObject( river_elements[i] );
}

void River::nextRiverSpeed( ) {
	if( curr_speed_idx == speeds.size( )-1 )
		return;
	++curr_speed_idx;
	changeSpeed( );
}

void River::prevRiverSpeed( ) {
	if( curr_speed_idx == 0 )
		return;
	--curr_speed_idx;
	changeSpeed( );
}

void River::changeSpeed( ) {
	float curr_speed = getCurrentSpeed( );
	for( size_t i=0; i<river_elements.size( ); ++i )
		((ROAObject *)river_elements[i]->getAnimationController( ))->setTimeScale( curr_speed ); // Change ROA Speed
}

float River::getCurrentSpeed( ) const {
	assert( curr_speed_idx < speeds.size( ) );
	return speeds[curr_speed_idx];
}

void River::updateAnimation( float delta_time ) {
	for( size_t i=0; i<river_elements.size( ); ++i )
		river_elements[i]->updateAnimation( delta_time );
}

void River::render( ) {
	for( size_t i=0; i<river_elements.size( ); ++i ) {
		AnimatedObject *object = river_elements[i];
		assert( object );

		BoundingSphere bsphere = object->getMesh( )->getBoundingSphere( );
		bsphere.center += object->loc.getPosition();
		if( Global::frustum.isBoundingVisible( bsphere ) && Global::back_side_frustum.isBoundingVisible( bsphere ) )
			object->render();
	}
}
