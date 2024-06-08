#include "PowerUps.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "Managers/EnemyManager.h"
#include "Managers/BrainManager.h"
#include "Managers/ROAResourceManager.h"
#include "AnimationControllers/ROA/ROAObject.h"
#include "Mesh/IMesh.h"
#include "Particles/PSManager.h"
#include "Util/Tracker/Tracker.h"

PowerUps::PowerUpBornPoint::PowerUpBornPoint( const D3DXVECTOR3 &aposition, float apowerup_life_probability, float apowerup_score_probability )
: position( aposition )
, powerup_life_probability( apowerup_life_probability )
, powerup_score_probability( apowerup_score_probability )
, total_probability( apowerup_life_probability + apowerup_score_probability )
, initiated( false )
, alpha( 0.0f )
, delta( 0.0f )
, powerup_associated( NULL )
{
	D3DXMatrixIsIdentity( &transform );
}

const IntelligentObject &PowerUps::PowerUpBornPoint::getPowerUpToCreate( ) const {
	float probability = obtainRandom( 0.0f, total_probability );
	const char *type = ( probability < powerup_life_probability ) ? "powerup_life" : "powerup_score";
	return EnemyManager::get( )->getEnemy( type );
}

const D3DXMATRIX &PowerUps::PowerUpBornPoint::getTransform( ) {
	if( !initiated ) {
		initiated = true;

		// Calculate transform
		BrainManager::get( )->getCurrentBrain( ).getPolarAngles( position, alpha, delta );
		BrainManager::get( )->getCurrentBrain( ).getTransformOnPlanet( transform, 0.0f, alpha, delta );
	}

	return transform;
}

float PowerUps::PowerUpBornPoint::getAlpha( ) const {
	assert( initiated );
	return alpha;
}

float PowerUps::PowerUpBornPoint::getDelta( ) const {
	assert( initiated );
	return delta;
}

// -----------------------------------------------------------------------------------------
PowerUps::~PowerUps( ) {
	printDebugString( "Destroying PowerUps...\n" );

	// Clear powerups
	clear( );

	// Clear XML things
	for( size_t i=0; i<powerup_born_points.size( ); ++i ) {
		PowerUpBornPoint *obj = powerup_born_points[i];
		if( obj != NULL )
			delete obj, obj = NULL;
	}
	powerup_born_points.clear( );
	non_used_power_up_born_points.clear( );
}

void PowerUps::clear( ) {
	resetPowerUpBornPoints( );

	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject *obj = (*this)[i];
		if( obj != NULL )
			delete obj, obj = NULL;
	}
	std::vector<IntelligentObject *>::clear( );
}

void PowerUps::resetPowerUpBornPoints( ) {
	for( size_t i=0; i<powerup_born_points.size( ); ++i )
		powerup_born_points[i]->powerup_associated = NULL;
	used_power_up_born_points.clear( );
	non_used_power_up_born_points = powerup_born_points; // Copy pointers
}

void PowerUps::onStartElement( const std::string &elem, MKeyValue &atts ) {
	if( elem == "powerups" ) {
		max_powerups_same_time = atts.getInt( "max_powerups_same_time", 1 );
	}
	else if( elem == "powerup" ) {
		// Position
		const std::string &position_text = atts[ "position" ];
		D3DXVECTOR3 position;
		sscanf_s( position_text.c_str(), "%f %f %f", &position.x, &position.y, &position.z );

		float powerup_life_probability  = atts.getFloat( "powerup_life_probability", 0.0f );
		float powerup_score_probability = atts.getFloat( "powerup_score_probability", 0.0f );

		PowerUpBornPoint *new_powerup_born_point = new PowerUpBornPoint( position, powerup_life_probability, powerup_score_probability );
		powerup_born_points.push_back( new_powerup_born_point );
	}
}

void PowerUps::add( int num ) {
	assert( powerup_born_points.size( ) > 0 );
	assert( max_powerups_same_time <= powerup_born_points.size( ) );

	for( int i=0; i<num; ++i ) {
		// Condition for no adding more powerups
		if( non_used_power_up_born_points.empty( ) || size( ) >= max_powerups_same_time )
			return;

		size_t powerup_born_idx = (size_t)obtainRandom( 0.0f, (float)non_used_power_up_born_points.size( ) );
		if( powerup_born_idx == non_used_power_up_born_points.size( ) )
			--powerup_born_idx;
		assert( powerup_born_idx < non_used_power_up_born_points.size( ) );
		PowerUpBornPoint *powerup_born_point_to_use = non_used_power_up_born_points[ powerup_born_idx ];
		const IntelligentObject &powerup_to_add = powerup_born_point_to_use->getPowerUpToCreate( );

		// Crear un personaje y ponerlo en el planeta
		IntelligentObject *powerup = new IntelligentObject( powerup_to_add ); // Copia
		assert( powerup );

		const D3DXMATRIX &transform = powerup_born_point_to_use->getTransform( );
		powerup->loc.setTransform( transform );
		powerup->loc.setAlpha( powerup_born_point_to_use->getAlpha( ) );
		powerup->loc.setDelta( powerup_born_point_to_use->getDelta( ) );

		// ROA
		const ROAResource *roa_resource_powerup = ROAResourceManager::get( )->getResource( "powerup.xml" );
		assert( roa_resource_powerup );
		ROAObject *roa = new ROAObject( &roa_resource_powerup->rigidAnimationResource, powerup->loc.getPosition(), powerup->loc.getRotation(), true );
		assert( roa );
		powerup->setAnimationController( roa );

		// Particles
		ParticleGroup *pgroup = PSManager::get( )->addParticleGroup( "powerup", TTracker( powerup->loc.getTransform( ) ) );
		powerup->setParticleGroup( pgroup );

		// Born particle
		PSManager::get( )->addParticleGroup( "powerup_born", TTracker( powerup->loc.getTransform( ) ) );

		push_back( powerup );
		setPowerUpBornPointAsUsed( powerup_born_idx, powerup );
	}
}

PowerUps::iterator PowerUps::del( iterator it ) {
	// Dead particle
	assert( *it );
	PSManager::get( )->addParticleGroup( "powerup_picked_up", TTracker( (*it)->loc.getTransform( ) ) );

	setPowerUpBornPointAsNotUsed( *it );
	delete (*it), (*it) = NULL;
	return erase( it );
}

void PowerUps::setPowerUpBornPointAsUsed( size_t powerup_born_idx, const IntelligentObject *powerup ) {
	assert( powerup_born_idx < non_used_power_up_born_points.size( ) );

	PowerUpBornPoint *powerup_born_point_to_be_used = non_used_power_up_born_points[ powerup_born_idx ];
	assert( powerup_born_point_to_be_used->powerup_associated == NULL );
	powerup_born_point_to_be_used->powerup_associated = powerup;

	non_used_power_up_born_points.erase( non_used_power_up_born_points.begin( ) + powerup_born_idx );
	used_power_up_born_points.push_back( powerup_born_point_to_be_used );
}

void PowerUps::setPowerUpBornPointAsNotUsed( const IntelligentObject *powerup ) {
	assert( !used_power_up_born_points.empty( ) );

	int idx = -1;
	for( size_t i=0; i<used_power_up_born_points.size( ); ++i ) {
		PowerUpBornPoint *obj = used_power_up_born_points[i];
		assert( obj );
		assert( obj->powerup_associated );
		if( obj->powerup_associated == powerup ) {
			idx = (int)i;
			obj->powerup_associated = NULL;
			break;
		}
	}

	if( idx == -1 ) {
		assert( !"This should not happend" );
		return;
	}

	non_used_power_up_born_points.push_back( used_power_up_born_points[ idx ] );
	used_power_up_born_points.erase( used_power_up_born_points.begin( ) + idx );
}

void PowerUps::updateAnimation( float deltaTime ) {
	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject *powerup = (*this)[i];
		assert( powerup );

		powerup->updateAnimation( deltaTime );
	}
}

void PowerUps::updateAI( float deltaTime ) {

	iterator it = begin();
	while( it != end() ) {
		bool dead = (*it)->updateAI( deltaTime );

		if( dead )
			it = del( it );
		else
			++it;
	}
}

void PowerUps::render( ) {
	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject *powerup = (*this)[i];
		assert( powerup );

		BoundingSphere bsphere = powerup->getMesh( )->getBoundingSphere( );
		bsphere.center += powerup->loc.getPosition();
		if( Global::frustum.isBoundingVisible( bsphere ) && Global::back_side_frustum.isBoundingVisible( bsphere ) )
			powerup->render();
	}
}
