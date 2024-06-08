#include "Expenders.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "AIControllers/ExpenderController/ExpenderController.h"
#include "Managers/BrainManager.h"
#include "Factories/TextureFactory.h"
#include "Factories/MeshFactory.h"
#include "Factories/CollisionShapeFactory.h"
#include "Util/Bounding/Bounding.h"
#include "Mesh/IMesh.h"

void Expender::reset( ) {
	// Reset controller
	ExpenderController *expender_controller = (ExpenderController *)object.getAIController( );
	expender_controller->reset( object );
	
	// Destroy its animation controller
	object.setAnimationController( NULL );

	// Restore its initial transform always, because the roa can change its position
	float alpha, delta;
	D3DXMATRIX new_transform;
	Planet &curr_planet = BrainManager::get( )->getCurrentBrain( );
	curr_planet.getPolarAngles( initial_position, alpha, delta );
	curr_planet.getTransformOnPlanet( new_transform, yaw, alpha, delta );
	object.loc.setTransform( new_transform );
	object.loc.setAlpha( alpha );
	object.loc.setDelta( delta );
	expender_controller->setInitialTransform( new_transform );
}

// ----------------------------------------------------------------------
Expenders::~Expenders( ) {
	printDebugString( "Destroying Expenders...\n" );

	for( size_t i=0; i<size( ); ++i ) {
		Expender *expender = (*this)[i];
		if( expender )
			delete expender, expender = NULL;
	}
	clear( );
}

void Expenders::load( ) {
	TextureFactory::get( )->createTexture( "expender_dopamina_green.dds" );
	TextureFactory::get( )->createTexture( "expender_dopamina_yellow.dds" );
	TextureFactory::get( )->createTexture( "expender_dopamina_red.dds" );
	TextureFactory::get( )->createTexture( "expender_noradrenalina_green.dds" );
	TextureFactory::get( )->createTexture( "expender_noradrenalina_yellow.dds" );
	TextureFactory::get( )->createTexture( "expender_noradrenalina_red.dds" );
	TextureFactory::get( )->createTexture( "dopamina_red.dds" );

	const std::string &xmlPath = Global::data_locator.getPath( "xml" );
	bool ok = xmlParseFile( xmlPath + "expenders_fourth_level.xml" );
	assert( ok );
}

void Expenders::onStartElement (const std::string &elem, MKeyValue &atts) {
	if( elem == "expender" ) {
		Expender *expender = new Expender( );
		assert( expender );

		// Mesh
		const IMesh *mesh = MeshFactory::get( )->createMesh( atts["mesh"] );
		assert( mesh );
		expender->object.setMesh( mesh );
	
		// Material
		expender->object.SETMATERIAL( atts["texture"], atts["shader"], atts["technique"] );

		// Collision
		const std::string &shape_type_name = atts[ "collision_shape" ];
		float bias = atts.getFloat( "collision_bias", 1.0f );
		CollisionShapeFactory::TypeShape shape_type = CollisionShapeFactory::getTypeShape( shape_type_name.c_str( ) );
		const ShapeHandle *shape_handle = CollisionShapeFactory::get( )->createCollisionShape( expender->object.getMesh( ), shape_type, bias );
		assert( shape_handle );
		expender->object.loc.createCollisionObject( shape_handle );

		// Position
		const std::string &position_text = atts[ "position" ];
		D3DXVECTOR3 position;
		sscanf_s( position_text.c_str(), "%f %f %f", &position.x, &position.y, &position.z );
		expender->object.loc.setPosition( position );
		// NOTE: Hay que arreglar la posicion luego en init

		// Controller
		const std::string &enemy_type           = atts[ "enemy_type" ];
		int num_enemies_can_expel               = atts.getInt( "num_enemies_can_expel", 5 );
		float recover_time                      = atts.getFloat( "recover_time", 10.0f );
		float kick_radius                       = atts.getFloat( "kick_radius", 5.0f );
		int points_score_up                     = atts.getInt( "points_score_up", 100 );
		assert( num_enemies_can_expel > 1 );
		assert( recover_time > 0.0f );
		assert( kick_radius > 0.0f );
		assert( points_score_up >= 0 );
		ExpenderController *expender_controller = new ExpenderController( enemy_type, num_enemies_can_expel, recover_time, kick_radius, points_score_up );
		expender->object.setAIController( expender_controller );

		// Yaw
		float yaw_degrees = atts.getFloat( "yaw_degrees", 0.0f );
		expender->yaw = deg2rad( yaw_degrees );
		expender->initial_position = position;

		// Add it
		push_back( expender );
	}
}

void Expenders::init( ) {
	for( size_t i=0; i<size( ); ++i ) {
		// Reset
		Expender *expender = (*this)[i];
		assert( expender );
		expender->reset( ); // Aqui se arregla la posicion

		Global::player->addSignalListener( &expender->object );   // Add to player listeners
		Global::spatial.addSpatialFixObject( &expender->object ); // Add to spatial
	}
}

void Expenders::destroy( ) {
	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject *object = &(*this)[i]->object;
		Global::player->removeSignalListener( object ); // Remove to player listeners
		Global::spatial.removeObject( object );         // Remove to spatial
	}
}

void Expenders::updateAI( float deltaTime ) {
	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject &object = (*this)[i]->object;
		object.updateAI( deltaTime );
	}
}

void Expenders::updateAnimation( float deltaTime ) {
	for( size_t i=0; i<size( ); ++i )
		(*this)[i]->object.updateAnimation( deltaTime );
}

void Expenders::render( ) {
	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject &object = (*this)[i]->object;

		BoundingSphere bsphere = object.getMesh( )->getBoundingSphere( );
		bsphere.center += object.loc.getPosition();
		if( Global::frustum.isBoundingVisible( bsphere ) && Global::back_side_frustum.isBoundingVisible( bsphere ) )
			object.render();
	}
}
