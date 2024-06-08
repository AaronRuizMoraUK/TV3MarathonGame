#include "Dopaminas.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "Managers/EnemyManager.h"
#include "Managers/BrainManager.h"
#include "Mesh/IMesh.h"
#include "Particles/PSManager.h"
#include "Util/Tracker/Tracker.h"

Dopaminas::~Dopaminas( ) {
	printDebugString( "Destroying Dopaminas...\n" );
	clear( );
}

void Dopaminas::clear( ) {
	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject *obj = (*this)[i];
		Global::player->removeSignalListener( obj ); // Eliminar de la lista de listeners del player
		Global::spatial.removeObject( obj ); // Remove it to the spatial
		if( obj != NULL )
			delete obj, obj = NULL;
	}
	std::vector<IntelligentObject *>::clear( );
}

void Dopaminas::add( int num ) {
	const IntelligentObject &dopamina_to_add = EnemyManager::get( )->getEnemy( "dopamina" );
	for( int i=0; i<num; ++i ) {
		// Random posicion en planeta
		float alpha, delta;
		BrainManager::get( )->getCurrentBrain( ).getRandomPolarAngles( alpha, delta );

		// Fix angles
		alpha = (obtainRandom(0.0f,1.0f)>0.25f) ? 0.0f : D3DX_PI;
		float radians = deg2rad( 20.0f );
		if( fabs( delta ) < radians )
			delta = (rand()%2 == 0) ? -radians : radians;

		float rot = obtainRandom( 0.0f, 2.0f*D3DX_PI - 0.1f );

		// Crear un personaje y ponerlo en el planeta
		IntelligentObject *dopamina = new IntelligentObject( dopamina_to_add ); // Copia
		assert( dopamina );
		dopamina->loc.setAlpha( alpha );
		dopamina->loc.setDelta( delta );

		D3DXMATRIX transform;
		BrainManager::get( )->getCurrentBrain( ).getTransformOnPlanet( transform, rot, alpha, delta );
		dopamina->loc.setTransform( transform );

		// Particles
		ParticleGroup *pgroup = PSManager::get( )->addParticleGroup( "dopamina_particles", TTracker( &dopamina->loc ) );
		dopamina->setParticleGroup( pgroup );

		// Born particle
		PSManager::get( )->addParticleGroup( "dopamine_born", TTracker( dopamina->loc.getTransform( ) ) );

		// Registrarse para que pueda recibir señales del player
		Global::player->addSignalListener( dopamina );

		// Add it to the spatial
		Global::spatial.addMovableObject( dopamina );

		push_back( dopamina );
	}
}

void Dopaminas::updateAI( float deltaTime ) {

	iterator it = begin();
	while( it != end() ) {
		bool dead = (*it)->updateAI( deltaTime );

		if( dead )
			it = del( it );
		else
			++it;
	}
}

Dopaminas::iterator Dopaminas::del( iterator it ) {
	assert( *it );

	// Dead particle
	PSManager::get( )->addParticleGroup( "dopamine_death", TTracker( (*it)->loc.getTransform( ) ) );

	Global::player->removeSignalListener( (*it) ); // Desregistrar la dopamina de los listeners del player
	Global::spatial.removeObject( (*it) ); // Remove it to the spatial
	delete (*it), (*it) = NULL;
	return erase( it );
}

void Dopaminas::render( ) {
	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject *dopamina = (*this)[i];

		BoundingSphere bsphere = dopamina->getMesh( )->getBoundingSphere( );
		bsphere.center += dopamina->loc.getPosition();
		if( Global::frustum.isBoundingVisible( bsphere ) && Global::back_side_frustum.isBoundingVisible( bsphere ) )
			dopamina->render();
	}
}
