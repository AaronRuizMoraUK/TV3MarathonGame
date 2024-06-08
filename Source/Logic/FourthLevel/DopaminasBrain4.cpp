#include "DopaminasBrain4.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "Managers/EnemyManager.h"
#include "Managers/BrainManager.h"
#include "Mesh/IMesh.h"
#include "Particles/PSManager.h"
#include "Util/Tracker/Tracker.h"
#include "AIControllers/DopaminaBrain4Controller/DopaminaBrain4Controller.h"

DopaminasBrain4::~DopaminasBrain4( ) {
	printDebugString( "Destroying DopaminasBrain4...\n" );
	clear( );
}

void DopaminasBrain4::clear( ) {
	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject *obj = (*this)[i];
		Global::spatial.removeObject( obj ); // Remove it to the spatial
		if( obj != NULL )
			delete obj, obj = NULL;
	}
	std::vector<IntelligentObject *>::clear( );
}

void DopaminasBrain4::add( int num, const char *dopamina_type ) {
	assert( strcmp( dopamina_type, "dopamina_brain4" ) == 0 ||
		strcmp( dopamina_type, "noradrenalina_brain4" ) == 0 );

	const IntelligentObject &dopamina_to_add = EnemyManager::get( )->getEnemy( dopamina_type );
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
		DopaminaBrain4Controller *controller = (DopaminaBrain4Controller *)dopamina->getAIController( );
		assert( controller );
		const char *born_particle = ( controller->type == DopaminaBrain4Controller::DOPAMINA ) ? "dopamine4brain_born" : "noradrenaline4brain_born";
		PSManager::get( )->addParticleGroup( born_particle, TTracker( dopamina->loc.getTransform( ) ) );

		// Add it to the spatial
		Global::spatial.addMovableObject( dopamina );

		push_back( dopamina );
	}
}

void DopaminasBrain4::updateAI( float deltaTime ) {

	iterator it = begin();
	while( it != end() ) {
		bool dead = (*it)->updateAI( deltaTime );

		if( dead )
			it = del( it );
		else
			++it;
	}
}

DopaminasBrain4::iterator DopaminasBrain4::del( iterator it ) {
	assert( *it );

	// Dead particle
	DopaminaBrain4Controller *controller = (DopaminaBrain4Controller *)(*it)->getAIController( );
	assert( controller );
	const char *death_particle = ( controller->type == DopaminaBrain4Controller::DOPAMINA ) ? "dopamine4brain_death" : "noradrenaline4brain_death";
	PSManager::get( )->addParticleGroup( death_particle, TTracker( (*it)->loc.getTransform( ) ) );

	Global::spatial.removeObject( (*it) ); // Remove it to the spatial
	delete (*it), (*it) = NULL;
	return erase( it );
}

void DopaminasBrain4::render( ) {
	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject *dopamina = (*this)[i];

		BoundingSphere bsphere = dopamina->getMesh( )->getBoundingSphere( );
		bsphere.center += dopamina->loc.getPosition();
		if( Global::frustum.isBoundingVisible( bsphere ) && Global::back_side_frustum.isBoundingVisible( bsphere ) )
			dopamina->render();
	}
}
