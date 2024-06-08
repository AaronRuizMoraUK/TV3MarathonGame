#include "Valves.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Util/Bounding/Bounding.h"
#include "Mesh/IMesh.h"
#include "Factories/MeshFactory.h"
#include "Factories/CollisionShapeFactory.h"
#include "Managers/BrainManager.h"
#include "Logic/ThirdLevelLogic.h"
#include "Factories/TextureFactory.h"

void Valve::reset( ) {
	// Reset controller
	ValveController *valve_controller = (ValveController *)object.getAIController( );
	valve_controller->reset( object );
	
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
	valve_controller->setInitialTransform( new_transform );

	// Iniciar el color del godray
	godray.CHANGETEXTURE( "valve_godray_red.dds" );
}

// ----------------------------------------------------------------------
Valves::~Valves( ) {
	printDebugString( "Destroying Valves...\n" );

	for( size_t i=0; i<size( ); ++i ) {
		Valve *valve = (*this)[i];
		if( valve )
			delete valve, valve = NULL;
	}
	clear( );

	for( size_t i=0; i<carteles.size( ); ++i ) {
		Object *cartel = carteles[i];
		if( cartel )
			delete cartel, cartel = NULL;
	}
	carteles.clear( );
}

void Valves::load( ) {
	TextureFactory::get( )->createTexture( "valve_green.dds" );
	TextureFactory::get( )->createTexture( "valve_yellow.dds" );
	TextureFactory::get( )->createTexture( "valve_red.dds" );

	const std::string &xmlPath = Global::data_locator.getPath( "xml" );
	bool ok = xmlParseFile( xmlPath + "valves_third_level.xml" );
	assert( ok );
}

void Valves::onStartElement (const std::string &elem, MKeyValue &atts) {
	if( elem == "valve" ) {
		Valve *valve = new Valve( );
		assert( valve );

		// Mesh
		const IMesh *mesh = MeshFactory::get( )->createMesh( atts["mesh"] );
		assert( mesh );
		valve->object.setMesh( mesh );
	
		// Material
		valve->object.SETMATERIAL( atts["texture"], atts["shader"], atts["technique"] );

		// Collision
		const std::string &shape_type_name = atts[ "collision_shape" ];
		float bias = atts.getFloat( "collision_bias", 1.0f );
		CollisionShapeFactory::TypeShape shape_type = CollisionShapeFactory::getTypeShape( shape_type_name.c_str( ) );
		const ShapeHandle *shape_handle = CollisionShapeFactory::get( )->createCollisionShape( valve->object.getMesh( ), shape_type, bias );
		assert( shape_handle );
		valve->object.loc.createCollisionObject( shape_handle );

		// Position
		const std::string &position_text = atts[ "position" ];
		D3DXVECTOR3 position;
		sscanf_s( position_text.c_str(), "%f %f %f", &position.x, &position.y, &position.z );
		valve->object.loc.setPosition( position );
		// NOTE: Hay que arreglar la posicion luego en init

		// Controller
		const std::string &initial_status_text = atts["initial_status"];
		const std::string &good_status_text    = atts["good_status"];

		ValveController::Status initial_status = ValveController::getStatusFromText( initial_status_text );
		ValveController::Status good_status    = ValveController::getStatusFromText( good_status_text );
		float time_to_desynchronize            = atts.getFloat( "time_to_desynchronize", 10.0f );
		float kick_radius                      = atts.getFloat( "kick_radius", 5.0f );
		int points_score_up                    = atts.getInt( "points_score_up", 100 );
		ValveController *valve_controller      = new ValveController( size( ), initial_status, good_status, time_to_desynchronize, kick_radius, points_score_up );
		valve->object.setAIController( valve_controller );

		// Yaw
		float yaw_degrees = atts.getFloat( "yaw_degrees", 0.0f );
		valve->yaw = deg2rad( yaw_degrees );
		valve->initial_position = position;

		// Godray
		const IMesh *godray_mesh = MeshFactory::get( )->createMesh( "valve_godray.mesh" );
		assert( godray_mesh );
		valve->godray.setMesh( godray_mesh );
		valve->godray.SETMATERIAL( "valve_godray_red.dds", "shader.fx", "dummy_tech" );
		TextureFactory::get( )->createTexture( "valve_godray_green.dds" ); // Cargar ya tambien la verde
		valve->godray.loc.setParentLocation( &valve->object.loc );
		valve->godray.loc.updateTransformWithParent();

		// Add it
		push_back( valve );

		// Create valve
		valves_status.push_back( ValveStatus( ) );
	}
	else if( elem == "cartel" ) {
		Object *cartel = new Object( );
		assert( cartel );

		// Mesh
		const IMesh *mesh = MeshFactory::get( )->createMesh( atts["mesh"] );
		assert( mesh );
		cartel->setMesh( mesh );
	
		// Material
		cartel->SETMATERIAL( atts["texture"], atts["shader"], atts["technique"] );

		// Transform
		const std::string &transform_text = atts["transform"];
		D3DXMATRIX transform;
		D3DXMatrixIdentity(&transform);
		sscanf_s( transform_text.c_str(), "%f %f %f %f %f %f %f %f %f %f %f %f",
			&transform(0,0), &transform(0,1), &transform(0,2),
			&transform(1,0), &transform(1,1), &transform(1,2),
			&transform(2,0), &transform(2,1), &transform(2,2),
			&transform(3,0), &transform(3,1), &transform(3,2) );
		cartel->loc.setTransform( transform );

		// Add it
		carteles.push_back( cartel );
	}
}

void Valves::init( ) {
	for( size_t i=0; i<size( ); ++i ) {
		// Reset
		Valve *valve = (*this)[i];
		assert( valve );
		valve->reset( ); // Aqui se arregla la posicion

		Global::player->addSignalListener( &valve->object );   // Add to player listeners
		Global::spatial.addSpatialFixObject( &valve->object ); // Add to spatial
		
		// Reset status for logic
		valves_status[i].status        = ((ValveController *)valve->object.getAIController( ))->getStatus( );
		valves_status[i].time_left     = 0.0f;
		valves_status[i].right_status  = false;
	}
}

void Valves::destroy( ) {
	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject *object = &(*this)[i]->object;
		Global::player->removeSignalListener( object ); // Remove to player listeners
		Global::spatial.removeObject( object );         // Remove to spatial
	}
}

void Valves::updateAI( float deltaTime ) {
	bool all_valves_in_right_status = true;

	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject &object = (*this)[i]->object;
		object.updateAI( deltaTime );

		const ValveController *valve_controller = (ValveController *)object.getAIController( );
		assert( valve_controller );
		valves_status[i].status       = valve_controller->getStatus( );
		valves_status[i].time_left    = valve_controller->getTimeToDesynchronize( ) - valve_controller->getTimeToExitGoodStatus( );
		bool prev_right_status = valves_status[i].right_status;
		valves_status[i].right_status = valve_controller->isOnRightStatus( );

		all_valves_in_right_status = all_valves_in_right_status && valves_status[i].right_status;

		if( !prev_right_status && valves_status[i].right_status )
			(*this)[i]->godray.CHANGETEXTURE( "valve_godray_green.dds" );
		else if( prev_right_status && !valves_status[i].right_status )
			(*this)[i]->godray.CHANGETEXTURE( "valve_godray_red.dds" );
	}

	// Si todas las valvulas estan en el estado correcto, hemos ganado el nivel
	if( all_valves_in_right_status )
		getThirdLevelLogic( ).winLevel( );
}

void Valves::updateAnimation( float deltaTime ) {
	for( size_t i=0; i<size( ); ++i ) {
		(*this)[i]->object.updateAnimation( deltaTime );
		(*this)[i]->godray.loc.updateTransformWithParent();
	}
}

void Valves::render( ) {
	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject &object = (*this)[i]->object;

		BoundingSphere bsphere = object.getMesh( )->getBoundingSphere( );
		bsphere.center += object.loc.getPosition();
		if( Global::frustum.isBoundingVisible( bsphere ) && Global::back_side_frustum.isBoundingVisible( bsphere ) )
			object.render();
	}

	for( size_t i=0; i<carteles.size( ); ++i ) {
		Object *cartel = carteles[i];
		assert( cartel );

		BoundingSphere bsphere = cartel->getMesh( )->getBoundingSphere( );
		bsphere.center += cartel->loc.getPosition();
		if( Global::frustum.isBoundingVisible( bsphere ) && Global::back_side_frustum.isBoundingVisible( bsphere ) )
			cartel->render();
	}
}

void Valves::renderGodrays( ) {
	for( size_t i=0; i<size( ); ++i ) {
		Object &object = (*this)[i]->godray;

		BoundingSphere bsphere = object.getMesh( )->getBoundingSphere( );
		bsphere.center += object.loc.getPosition();
		if( Global::frustum.isBoundingVisible( bsphere ) && Global::back_side_frustum.isBoundingVisible( bsphere ) )
			object.render();
	}
}
