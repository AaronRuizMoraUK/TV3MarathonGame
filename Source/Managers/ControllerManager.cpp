#include "ControllerManager.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"

#include "AIControllers/SphereRandomWalkController/SphereRandomWalkController.h"
#include "AIControllers/AlucinacionSphereChase/AlucinacionSphereChase.h"
#include "AIControllers/Follower/Follower.h"
#include "AIControllers/TwisterChase/TwisterChase.h"
#include "AIControllers/DopaminaBrain4Controller/DopaminaBrain4Controller.h"
#include "AIControllers/PowerUpController/PowerUpController.h"
#include "AnimationControllers/YAxisRotation/YAxisRotation.h"
#include "PlayerControllers/SpherePlayerController/SpherePlayerController.h"
#include "CameraControllers/SSPlayer/SSPlayerCameraController.h"

IAIController *ControllerManager::AIControllers::loadController( const std::string &name, MKeyValue &atts ) {
	if( name == "dopamina_sphere_random_walk" ) {
		float max_speed      = atts.getFloat( "max_speed", 3.0f );
    float max_think_time = atts.getFloat( "max_think_time", 10.0f );
    float rot_speed      = atts.getFloat( "rot_speed", 2.0f );
    float acceleration   = atts.getFloat( "acceleration", 2.0f );
    float kick_distance  = atts.getFloat( "kick_distance", 1.6f );
    float kick_angle     = deg2rad( atts.getFloat( "kick_angle", 120.0f ) );
    float kick_radius    = atts.getFloat( "kick_radius", 1.0f );
    int points_score_up  = atts.getInt( "points_score_up", 10 );
		return new SphereRandomWalkController( max_speed, max_think_time, rot_speed, acceleration, kick_distance, kick_angle, kick_radius, points_score_up );
	}
	else if( name == "alucinacion_sphere_chase" ) {
		float radius_chase        = atts.getFloat( "radius_chase", 8.0f );
		float radius_touch_player = atts.getFloat( "radius_touch_player", 0.6f );
		float walk_speed          = atts.getFloat( "walk_speed", 3.5f );
		float run_speed           = atts.getFloat( "run_speed", 7.0f );
    float max_think_time      = atts.getFloat( "max_think_time", 10.0f );
    float rot_speed           = atts.getFloat( "rot_speed", 2.0f );
    float acceleration        = atts.getFloat( "acceleration", 2.0f );
    float points_hit_player   = atts.getFloat( "points_hit_player", 10.0f );
		return new AlucinacionSphereChase( radius_chase, radius_touch_player, walk_speed, run_speed, max_think_time, rot_speed, acceleration, points_hit_player );
	}
	else if (name == "follower" ) {
		float distance_player	    = atts.getFloat( "distance_player", 1.0f );
		float rot_speed			      = atts.getFloat( "rot_speed", 2.0f);
		float acceleration		    = atts.getFloat( "acceleration", 2.0f);
		float radius_chase        = atts.getFloat( "radius_chase", 8.0f );
		float radius_touch_player = atts.getFloat( "radius_touch_player", 0.6f );
		float max_time			      = atts.getFloat ("max_time", 5.0f);
		int points_score_up  = atts.getInt( "points_score_up", 10 );
		return new Follower(distance_player, rot_speed, acceleration, radius_chase, radius_touch_player, max_time, points_score_up);
	}
	else if( name == "twister_chase" ) {
		float radius_chase        = atts.getFloat( "radius_chase", 8.0f );
		float radius_touch_player = atts.getFloat( "radius_touch_player", 0.6f );
		float walk_speed          = atts.getFloat( "walk_speed", 3.5f );
		float run_speed           = atts.getFloat( "run_speed", 7.0f );
    float max_think_time      = atts.getFloat( "max_think_time", 10.0f );
    float rot_speed           = atts.getFloat( "rot_speed", 2.0f );
    float acceleration        = atts.getFloat( "acceleration", 2.0f );
    float points_hit_player   = atts.getFloat( "points_hit_player", 10.0f );
		return new TwisterChase( radius_chase, radius_touch_player, walk_speed, run_speed, max_think_time, rot_speed, acceleration, points_hit_player );
	}
	else if( name == "dopamina_brain4" || name == "noradrenalina_brain4" ) {
		float max_speed      = atts.getFloat( "max_speed", 3.0f );
    float max_think_time = atts.getFloat( "max_think_time", 10.0f );
    float rot_speed      = atts.getFloat( "rot_speed", 2.0f );
    float acceleration   = atts.getFloat( "acceleration", 2.0f );
		if( name == "dopamina_brain4" )
			return new DopaminaBrain4Controller( DopaminaBrain4Controller::DOPAMINA, max_speed, max_think_time, rot_speed, acceleration );
		else
			return new DopaminaBrain4Controller( DopaminaBrain4Controller::NORADRENALINA, max_speed, max_think_time, rot_speed, acceleration );
	}
	else if( name == "powerup_life" || name == "powerup_score" ) {
		const std::string &type_string = atts["type"];
		PowerUpController::Type type = PowerUpController::getTypeFromText( type_string );
    float points            = atts.getFloat( "points", 10.0f );
    int   score_for_taking  = atts.getInt( "score_for_taking", 10 );
    float radius            = atts.getFloat( "radius", 2.0f );
		assert( points >= 0.0f );
		assert( score_for_taking >= 0 );
		assert( radius >= 0.0f );
		return new PowerUpController( type, points, score_for_taking, radius );
	}
	else {
		assert( !"AI Controller type unknown" );
		return NULL;
	}
}

IAIController *ControllerManager::AIControllers::createController( const Controller *controller ) const {
	// Copy, a new instance
	if( controller->name == "dopamina_sphere_random_walk" ) {
		return new SphereRandomWalkController( *((SphereRandomWalkController *) controller->controller) );
	}
	else if( controller->name == "alucinacion_sphere_chase" ) {
		return new AlucinacionSphereChase( *((AlucinacionSphereChase *) controller->controller) );
	}
	else if( controller->name == "follower" ) {
		return new Follower( *((Follower *) controller->controller) );
	}
	else if( controller->name == "twister_chase" ) {
		return new TwisterChase( *((TwisterChase *) controller->controller) );
	}
	else if( controller->name == "dopamina_brain4" || controller->name == "noradrenalina_brain4" ) {
		return new DopaminaBrain4Controller( *((DopaminaBrain4Controller *) controller->controller) );
	}
	else if( controller->name == "powerup_life" || controller->name == "powerup_score" ) {
		return new PowerUpController( *((PowerUpController *) controller->controller) );
	}
	else {
		assert( !"AI Controller type unknown" );
		return NULL;
	}
}

// -------------------------------------------------------------------------------------------

IAnimationController *ControllerManager::AnimationControllers::loadController( const std::string &name, MKeyValue &atts ) {
	if( name == "sky_rotation" ) {
		float rot_speed = atts.getFloat( "rot_speed", 0.015f );
		return new YAxisRotation( rot_speed );
	}
	else {
		assert( !"Animation Controller type unknown" );
		return NULL;
	}
}

IAnimationController *ControllerManager::AnimationControllers::createController( const Controller *controller ) const {
	// Copy, a new instance
	if( controller->name == "sky_rotation" ) {
		return new YAxisRotation( *((YAxisRotation *) controller->controller) );
	}
	else {
		assert( !"Animation Controller type unknown" );
		return NULL;
	}
}

// -------------------------------------------------------------------------------------------

IPlayerController *ControllerManager::PlayerControllers::loadController( const std::string &name, MKeyValue &atts ) {
	if( name == "player_sphere" ) {
    float walk_speed   = atts.getFloat( "walk_speed", 3.5f );
    float run_speed    = atts.getFloat( "run_speed", 7.0f );
    float hit_speed    = atts.getFloat( "hit_speed", 5.0f );
    float rot_speed    = atts.getFloat( "rot_speed", 20.0f );
    float acceleration = atts.getFloat( "acceleration", 20.0f );
		return new SpherePlayerController( walk_speed, run_speed, hit_speed, rot_speed, acceleration );
	}
	else {
		assert( !"Player Controller type unknown" );
		return NULL;
	}
}

IPlayerController *ControllerManager::PlayerControllers::createController( const Controller *controller ) const {
	// Copy, a new instance
	if( controller->name == "player_sphere" ) {
		return new SpherePlayerController( *((SpherePlayerController *) controller->controller) );
	}
	else {
		assert( !"Player Controller type unknown" );
		return NULL;
	}
}

// -------------------------------------------------------------------------------------------

ICameraController *ControllerManager::CameraControllers::loadController( const std::string &name, MKeyValue &atts ) {
	if( name == "camera_sphere" ) {
    float target_speed    = atts.getFloat( "target_speed", 4.0f );
    float position_speed  = atts.getFloat( "position_speed", 1.4f );
    float distance_player = atts.getFloat( "distance_player", 22.0f );
		return new SSPlayerCameraController( target_speed, position_speed, distance_player );
	}
	else {
		assert( !"Camera Controller type unknown" );
		return NULL;
	}
}

ICameraController *ControllerManager::CameraControllers::createController( const Controller *controller ) const {
	// Copy, a new instance
	if( controller->name == "camera_sphere" ) {
		return new SSPlayerCameraController( *((SSPlayerCameraController *) controller->controller) );
	}
	else {
		assert( !"Camera Controller type unknown" );
		return NULL;
	}
}

// -------------------------------------------------------------------------------------------

std::string ControllerManager::pathXML = "";
ControllerManager * ControllerManager::controllerManager = NULL;

ControllerManager::~ControllerManager() {
	printDebugString( "Destroying ControllerManager...\n" );
}

ControllerManager * ControllerManager::get() {
	if(controllerManager == NULL)
		controllerManager = new ControllerManager();

	return controllerManager;
}

void ControllerManager::load( ) {
	if( pathXML.empty( ) )
		pathXML = Global::data_locator.getPath( "xml" );
	bool ok = xmlParseFile( pathXML + "controllers.xml" );
	assert( ok );
}

void ControllerManager::onStartElement( const std::string &elem, MKeyValue &atts ) {
	if( elem == "ai_controller" )
		ai_controllers.load( atts );
	else if( elem == "animation_controller" )
		animation_controllers.load( atts );
	else if( elem == "player_controller" )
		player_controllers.load( atts );
	else if( elem == "camera_controller" )
		camera_controllers.load( atts );
}

//void ControllerManager::reload( ) {
//	printDebugString( "Reloading ControllerManager...\n" );
//
//	ai_controllers.destroy( );
//	animation_controllers.destroy( );
//	player_controllers.destroy( );
//	camera_controllers.destroy( );
//
//	load( );
//}
