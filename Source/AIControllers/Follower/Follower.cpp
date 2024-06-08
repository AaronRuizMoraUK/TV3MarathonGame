#include "Follower.h"
#include "Global/Rendering.h"
#include "Global/Geometry.h"
#include "Global/Variables.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "PlayerControllers/SpherePlayerController/SpherePlayerController.h"
#include "Managers/BrainManager.h"
#include "SoundPlayer/SoundPlayer.h"
#include "Logic/Logic.h"
#include "Logic/SecondLevelLogic.h"

Follower::Follower(
				   float adistance_player,
				   float arot_speed,
				   float aaceleration,
				   float aradius_chase,
				   float aradius_touch_player,
				   float amax_time,
				   int apoints_score_up 
				   )
				   : spherical_controller( arot_speed, aaceleration )
				   , distance_player(adistance_player)
				   , status( STOPED )
				   , prev_status( STOPED )
				   , dead( false )
				   , radius_chase( aradius_chase )
				   , radius_touch_player( aradius_touch_player )
				   , max_time (amax_time)
				   , current_time (0.0f)
				   , points_score_up (apoints_score_up)
{
}

Follower::~Follower() {
	printDebugString( "Destroying Follower...\n" );
}


bool Follower::update(IntelligentObject &object, float deltaTime) {

	// STATUS
	switch( status ) {
		case STOPED: {
			if( prev_status != status ) {
				// Esto se ejecuta sólo una vez, cuando hay cambio de estado
				spherical_controller.setAngle( 0.0f );
				spherical_controller.setTargetSpeed( 0.0f );
			}

			current_time = 0.0f;
			checkHitPlayer( object );
			break; 
					 }

		case FOLLOWING: {
			if( prev_status != status ) {
				// Esto se ejecuta sólo una vez, cuando hay cambio de estado
				spherical_controller.setTargetSpeed( 60.0f );
			}

			// Rotar para llegar al player
			float angle = getRotationToFollowPlayer( object );
			spherical_controller.setAngle( angle );

			// Ir de frente
			spherical_controller.setDirection( object.loc.getLocalFront( ) );

			checkMustFollowPlayer(object);
			checkExplosion(object, deltaTime);
			break; 
						}

		case EXPLODING:
			{
				break;
			}
		case PLANTED:
			{
				if( prev_status != status ) {
					current_time = max_time - 1.0f - deltaTime;
				}
				checkExplosion(object, deltaTime);
				float angle = getSecondLevelLogic().getRotationToFollowZona();

				if (angle != 0.0f)
				{
					spherical_controller.setTargetSpeed(0.0f);
				}
				spherical_controller.setAngle(angle);
				spherical_controller.setDirection( object.loc.getLocalFront( ) );
				break;
			}

		default:
			assert( !"This should not happend!" );
			return false;
			break;
	}

	// ----------------------------------------------------------------

	// Update Rotation
	spherical_controller.updateRotation( object.loc, deltaTime );

	// Update Position
	spherical_controller.updatePosition( object.loc, deltaTime );

	// Comprobar si he golpeado al player

	prev_status = status;
	return dead;
}

float Follower::getRotationToFollowPlayer( IntelligentObject &object ) const {
	const D3DXVECTOR3 &player_pos = Global::player->loc.getPosition( );
	const D3DXVECTOR3 &obj_pos = object.loc.getPosition( );
	if( player_pos == obj_pos )
		return 0.0f;

	D3DXVECTOR3 dir_to_player = player_pos - obj_pos;
	normalizeVector3( dir_to_player, dir_to_player );

	// Get angle
	float angle = D3DXVec3Dot( &object.loc.getLocalFront( ), &dir_to_player );
	angle = acos( angle );
	if( angle == 0.0f )
		return 0.0f;

	// Get sign of angle with a plane test
	// Ax + By + Cz + D = 0
	// (A,B,C) -> getLocalLeft
	// (x,y,z) -> dir_to_player
	const D3DXVECTOR3 &obj_left = object.loc.getLocalLeft( );
	float sign = obj_left.x * dir_to_player.x + obj_left.y * dir_to_player.y + obj_left.z * dir_to_player.z;

	return ( sign < 0.0f ) ? -angle : angle;
}

void Follower::checkMustFollowPlayer( IntelligentObject &object)
{
	SpherePlayerController *spc = (SpherePlayerController *) Global::player->getPlayerController();
	float speed = spc->getCurrentSpeed();

	float distance_to_player = distance( object.loc.getPosition( ), Global::player->loc.getPosition( ) );
	if( distance_to_player <= radius_touch_player ) {
		spherical_controller.setTargetSpeed(0.0f);
		// spherical_controller.setCurrentSpeed(0.0f);
	}
	else
	{	
		spherical_controller.setTargetSpeed(speed);
	}	
}

void Follower::checkHitPlayer( IntelligentObject &object ) {
	// Si no estoy en estado de persiguiendo no compruebo
	if( status != STOPED )
		return;

	float distance_to_player = distance( object.loc.getPosition( ), Global::player->loc.getPosition( ) );
	if( distance_to_player <= radius_chase ) {

		status = FOLLOWING;

		getSecondLevelLogic().showGetBombDinamicHelp();
		current_time = 0.0f;
	}
}

void Follower::checkExplosion(IntelligentObject &object, float deltaTime)
{
	current_time += deltaTime;

	if (current_time > max_time)
	{
		getSecondLevelLogic().exploding(object, status, points_score_up);
		status = EXPLODING;
	}
}
