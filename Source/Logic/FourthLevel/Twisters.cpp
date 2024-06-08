#include "Twisters.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "Managers/ControllerManager.h"
#include "Managers/BrainManager.h"
#include "Particles/PSManager.h"
#include "Util/Tracker/Tracker.h"
#include "Factories/CollisionShapeFactory.h"
#include "Factories/MeshFactory.h"

Twisters::~Twisters( ) {
	printDebugString( "Destroying Twisters...\n" );
	clear( );
}

void Twisters::clear( ) {
	for( size_t i=0; i<size( ); ++i ) {
		IntelligentObject *obj = (*this)[i];
		Global::spatial.removeObject( obj ); // Remove it to the spatial
		if( obj != NULL )
			delete obj, obj = NULL;
	}
	std::vector<IntelligentObject *>::clear( );
}

void Twisters::add( int num ) {
	for( int i=0; i<num; ++i ) {
		// Random posicion en planeta
		float alpha, delta;
		BrainManager::get( )->getCurrentBrain( ).getRandomPolarAngles( alpha, delta );

		// No nazcan cerca del punto inicial del planeta
		float radians = deg2rad( 45.0f );
		if( fabs( delta ) < radians )
			delta = (rand()%2 == 0) ? -radians : radians;
		if( fabs( alpha ) < radians || fabs( alpha ) > 2.0f*D3DX_PI - radians )
			alpha = (rand()%2 == 0) ? radians : 2.0f*D3DX_PI - radians;

		float rot = obtainRandom( 0.0f, 2.0f * D3DX_PI - 0.1f );

		// Crear un personaje y ponerlo en el planeta
		IntelligentObject *twister = new IntelligentObject( );
		assert( twister );
		twister->loc.setAlpha( alpha );
		twister->loc.setDelta( delta );

		D3DXMATRIX transform;
		BrainManager::get( )->getCurrentBrain( ).getTransformOnPlanet( transform, rot, alpha, delta );
		twister->loc.setTransform( transform );

		// AI
		IAIController *ai_controller = ControllerManager::get( )->getAIController( "twister_chase" );
		assert( ai_controller );
		twister->setAIController( ai_controller );

		// Particle
		ParticleGroup *pgroup = PSManager::get( )->addParticleGroup( "twister_particles", TTracker( &twister->loc ) );
		twister->setParticleGroup( pgroup );

		// Born particle
		PSManager::get( )->addParticleGroup( "twister_born", TTracker( twister->loc.getTransform( ) ) );

		// Poner una maya para las colisiones
		const IMesh *mesh = MeshFactory::get( )->createMesh( "player.mesh" );
		assert( mesh );
		twister->setMesh( mesh );

		// Collisions
		CollisionShapeFactory::TypeShape shape_type = CollisionShapeFactory::getTypeShape( "CYLINDER" );
		const ShapeHandle *shape_handle = CollisionShapeFactory::get( )->createCollisionShape( twister->getMesh( ), shape_type, 1.0f );
		assert( shape_handle );
		twister->loc.createCollisionObject( shape_handle );

		// Add it to the spatial
		Global::spatial.addMovableObject( twister );

		push_back( twister );
	}
}

void Twisters::updateAI( float deltaTime ) {

	iterator it = begin();
	while( it != end() ) {
		bool dead = (*it)->updateAI( deltaTime );

		if( dead )
			it = del( it );
		else
			++it;
	}
}

Twisters::iterator Twisters::del( iterator it ) {
	assert( *it );

	// Dead particle
	PSManager::get( )->addParticleGroup( "twister_death", TTracker( (*it)->loc.getTransform( ) ) );

	Global::spatial.removeObject( (*it) ); // Remove it to the spatial
	delete (*it), (*it) = NULL;
	return erase( it );
}
