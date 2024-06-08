#include <cassert>
#include "PowerUpController.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Logic/Logic.h"
#include "SoundPlayer/SoundPlayer.h"
#include "Managers/BrainManager.h"

const char *PowerUpController::types_chars[ NUM_TYPES ] = {
	"LIFE"
	, "SCORE"
};

PowerUpController::Type PowerUpController::getTypeFromText( const std::string &type_text ) {
	for( size_t i=0; i<NUM_TYPES; ++i ) {
		if( type_text == types_chars[ i ] )
			return (Type)i;
	}

	assert( !"This should not happend" );
	return LIFE;
}

PowerUpController::PowerUpController( Type atype, float apoints, size_t ascore_for_taking, float aradius )
: type( atype )
, points( apoints )
, score_for_taking( ascore_for_taking )
, radius( aradius )
{
}

PowerUpController::~PowerUpController() {
	printDebugString( "Destroying PowerUpController...\n" );
}

bool PowerUpController::update(IntelligentObject &object, float deltaTime) {
	if( hasPlayerTakePowerUp( object ) ) {
		applyPowerUp( );
		return true;
	}

	return false;
}

bool PowerUpController::hasPlayerTakePowerUp( const IntelligentObject &object ) const {
	const D3DXVECTOR3 &playerPosition = Global::player->loc.getPosition();
	D3DXVECTOR3 powerupPosition_onBrain; // Alway use the position on brain
	BrainManager::get( )->getCurrentBrain( ).getPointOnPlanet( powerupPosition_onBrain, object.loc.getAlpha( ), object.loc.getDelta( ) );

	D3DXVECTOR3 distance = powerupPosition_onBrain - playerPosition;
	return D3DXVec3Length( &distance ) <= radius;
}

void PowerUpController::applyPowerUp( ) {
	switch( type ) {
		case LIFE:
			// Sound of take a life power up
			SoundPlayer::get( )->playSample( "power_up_picked_up" );

			// Apply logic
			Logic::scoreUp( score_for_taking );
			Logic::playerSyncUp( points );
			break;

		case SCORE:
			// Sound of take a life power up
			SoundPlayer::get( )->playSample( "power_up_picked_up" );

			// Apply logic
			Logic::scoreUp( score_for_taking + (size_t)points );
			break;

		default:
			assert( !"This should not happend" );
			return;
	}
}
