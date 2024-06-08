#include "Bombas.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Global/Rendering.h"
#include "Managers/EnemyManager.h"
#include "Managers/BrainManager.h"
#include "Mesh/IMesh.h"
#include "Particles/PSManager.h"
#include "Util/Tracker/Tracker.h"
#include "Logic/SecondLevelLogic.h"
#include "Particles/ParticleGroup.h"
#include "Bass/SoundPlayer.h"

Rayo::Rayo(float aminRandomLeft, float amaxRandomLeft, float aminRandomFront, float amaxRandomFront) 
: status(WARNING),
currentTime(0.0f),
timeWarning(1.5f),
timeExploding(1.9f),
timeBurning(6.0f),
minRandomLeft(aminRandomLeft),
maxRandomLeft(amaxRandomLeft),
minRandomFront(aminRandomFront),
maxRandomFront(amaxRandomFront),
radius_touch_player(1.0f),
bomba_hit(25.0f),
hasHit(false)
{
	const D3DXVECTOR3 &playerPos = Global::player->loc.getPosition();
	const D3DXVECTOR3 &playerLeft = Global::player->loc.getLocalLeft();
	const D3DXVECTOR3 &playerFront = Global::player->loc.getLocalFront();

	float randomLeft = obtainRandom(minRandomLeft, maxRandomLeft);
	float randomFront = obtainRandom(minRandomFront, maxRandomFront);

	D3DXVECTOR3 pos = playerPos + playerLeft * randomLeft + playerFront * randomFront;

	float alpha, delta;
	BrainManager::get()->getCurrentBrain().getPolarAngles(pos, alpha, delta);

	D3DXMATRIX newTransform;
	BrainManager::get()->getCurrentBrain().getTransformOnPlanet(newTransform, 0.0f, alpha, delta);
	loc.setTransform( newTransform );

	p = PSManager::get( )->addParticleGroup( "warning_rayo", TTracker( loc.getTransform() ) );

}

Rayo::~Rayo()
{
	printDebugString( "Destroying Rayo...\n" );

	if (p)
	{
		p->kill();
		p = NULL;
	}
}

bool Rayo::update(float deltaTime)
{

	currentTime+=deltaTime;
	switch (status)
	{
	case WARNING:
		checkExploding();
		break;
	case EXPLODING:
		checkBurning();
		checkPlayerEatsExplosion();
		break;
	case BURNING:
		isPlayerInsideResidual(deltaTime);
		break;
	case DEAD:
		return true;
	}

	return false;
}

void Rayo::checkExploding()
{
	if (currentTime > timeWarning)
	{
		status = EXPLODING;
		explode();
	}
}

void Rayo::checkBurning()
{
	if (currentTime > timeExploding)
	{
		status = BURNING;
		p = PSManager::get( )->addParticleGroup( "after_rayo", TTracker( loc.getTransform() ) );
	}
}

void Rayo::isPlayerInsideResidual(float deltaTime)
{
	currentTime += deltaTime;

	float distance_to_player = distance( loc.getPosition( ), Global::player->loc.getPosition( ) );
	if( distance_to_player <= radius_touch_player )
	{
		getSecondLevelLogic().playerInsideResidual(deltaTime);
	}

	if (currentTime > timeBurning)
	{
		status = DEAD;
	}
}
void Rayo::explode()
{
	if (p)
	{
		p->kill();
		p = NULL;
	}

	// get a random explosion particle
	int randomExplosionNum = (int)floor(obtainRandom( 1.0f, 5.5f ));
	randomExplosionNum = clamp( randomExplosionNum, 1, 5 );
	char explosion[ 128 ];
	sprintf_s(explosion, sizeof(explosion), "light_explosion_%d", randomExplosionNum);
	SoundPlayer::get( )->playSample( explosion );
	PSManager::get( )->addParticleGroup( explosion, TTracker( loc.getTransform() ) );
}

void Rayo::checkPlayerEatsExplosion()
{
	if (!hasHit)
	{
		float distance_to_player = distance( loc.getPosition( ), Global::player->loc.getPosition( ) );
		if( distance_to_player <= radius_touch_player )
		{
			const D3DXVECTOR3 &player_pos = Global::player->loc.getPosition( );
			const D3DXVECTOR3 &obj_pos = loc.getPosition( );
			D3DXVECTOR3 hit_dir;

			if( player_pos == obj_pos )
				hit_dir = -loc.getLocalFront( );
			else {
				hit_dir = player_pos - obj_pos;
				normalizeVector3( hit_dir, hit_dir );
			}

			Global::player->processSignal( SIGNAL_HIT_PLAYER, &hit_dir, sizeof( hit_dir ) );
			getSecondLevelLogic().playerSyncDown(bomba_hit);
			hasHit = true;
		}
	}
}


Bombas::~Bombas( ) {
	printDebugString( "Destroying Rayos...\n" );
	clear( );
}

void Bombas::clear( ) {
	for( size_t i=0; i<size( ); ++i ) {
		Rayo *obj = (*this)[i];
		if( obj != NULL )
			delete obj, obj = NULL;
	}
	std::vector<Rayo *>::clear( );
}

void Bombas::add(float aminRandomLeft, float amaxRandomLeft, float aminRandomFront, float amaxRandomFront) {
	Rayo *rayo = new Rayo(aminRandomLeft, amaxRandomLeft, aminRandomFront, amaxRandomFront);

	// Random posicion en planeta
	push_back( rayo );
}

void Bombas::update( float deltaTime ) {

	iterator it = begin();
	while( it != end() ) {
		bool dead = (*it)->update( deltaTime );

		if( dead )
			it = del( it );
		else
			++it;
	}
}

Bombas::iterator Bombas::del( iterator it ) {
	assert( *it );
	delete (*it), (*it) = NULL;
	return erase( it );
}

