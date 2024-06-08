#include "Alucinaciones.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "Managers/EnemyManager.h"
#include "Managers/BrainManager.h"
#include "Mesh/IMesh.h"
#include "Particles/PSManager.h"
#include "Util/Tracker/Tracker.h"

Alucinaciones::~Alucinaciones( ) {
	printDebugString( "Destroying Alucinaciones...\n" );
	clear( );
}

void Alucinaciones::clear( ) {
	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject *obj = (*this)[i];
		Global::spatial.removeObject( obj ); // Remove it to the spatial
		if( obj != NULL )
			delete obj, obj = NULL;
	}
	std::vector<IntelligentObject *>::clear( );
}

void Alucinaciones::add( int num ) {
	const IntelligentObject &alucinacion_to_add = EnemyManager::get( )->getEnemy( "alucinacion" );
	for( int i=0; i<num; ++i ) {
		// Random posicion en planeta
		float alpha, delta;
		BrainManager::get( )->getCurrentBrain( ).getRandomPolarAngles( alpha, delta );

		// Nacen lejos del player
		if( alpha < D3DX_PI/2.0f || alpha > 3.0f*D3DX_PI/2.0f )
			alpha = (rand()%2 == 0) ? D3DX_PI/2.0f : 3.0f*D3DX_PI/2.0f;

		float rot = obtainRandom( 0.0f, 2.0f * D3DX_PI - 0.1f );

		// Crear un personaje y ponerlo en el planeta
		IntelligentObject *alucinacion = new IntelligentObject( alucinacion_to_add ); // Copia
		assert( alucinacion );
		alucinacion->loc.setAlpha( alpha );
		alucinacion->loc.setDelta( delta );

		D3DXMATRIX transform;
		BrainManager::get( )->getCurrentBrain( ).getTransformOnPlanet( transform, rot, alpha, delta );
		alucinacion->loc.setTransform( transform );

		// Add it to the spatial
		Global::spatial.addMovableObject( alucinacion );

		// Born particle
		PSManager::get( )->addParticleGroup( "hallucination_born", TTracker( alucinacion->loc.getTransform( ) ) );

		push_back( alucinacion );
	}
}

void Alucinaciones::updateAI( float deltaTime ) {

	iterator it = begin();
	while( it != end() ) {
		bool dead = (*it)->updateAI( deltaTime );

		if( dead )
			it = del( it );
		else
			++it;
	}
}

Alucinaciones::iterator Alucinaciones::del( iterator it ) {
	assert( *it );

	// Dead particle
	PSManager::get( )->addParticleGroup( "hallucination_death", TTracker( (*it)->loc.getTransform( ) ) );

	Global::spatial.removeObject( (*it) ); // Remove it to the spatial
	delete (*it), (*it) = NULL;
	return erase( it );
}

void Alucinaciones::updateAnimation( float deltaTime ) {
	for( size_t i=0; i<size( ); ++i )
		(*this)[i]->updateAnimation( deltaTime );
}

void Alucinaciones::render( ) {
	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject *alucinacion = (*this)[i];

		BoundingSphere bsphere = alucinacion->getMesh( )->getBoundingSphere( );
		bsphere.center += alucinacion->loc.getPosition();
		if( Global::frustum.isBoundingVisible( bsphere ) && Global::back_side_frustum.isBoundingVisible( bsphere ) )
			alucinacion->render();
	}
}
