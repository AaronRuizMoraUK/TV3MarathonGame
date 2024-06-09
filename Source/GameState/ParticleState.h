#ifndef PARTICLE_STATE_H
#define PARTICLE_STATE_H

#include "IGameState.h"

class ParticleState : public IGameState
{
protected:
	TLocation loc;
	size_t current_core_particle;

    //Object sphere;

public:
	ParticleState( ) : current_core_particle( 0 ) {}
	~ParticleState( );

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "particle_state"; };

protected:
	void update( float deltaTime );

	void preRender( );
	void renderWorld3D( );
	void renderWorld2D( );
	void renderPostProcessed2D( );
	void renderGameUI( );
	void postRender( );

private:
	void updateCamera( float deltaTime );
};

#endif // PARTICLE_STATE_H
