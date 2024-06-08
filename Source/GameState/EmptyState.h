#ifndef EMPTY_STATE_H
#define EMPTY_STATE_H

#include "IGameState.h"

class EmptyState : public IGameState
{
protected:
	// Things of this game state (si necesitan cargarse hacerlo en la funcion loadGameState())

public:
	EmptyState( );
	~EmptyState( );

	void onLoad( );
	void onUnload( );

	const char *getName( ) const { return "empty_state"; }

protected:
	void update( float deltaTime );

	void preRender( );
	void renderWorld3D( );
	void renderWorld2D( );
	void renderPostProcessed2D( );
	void renderGameUI( );
	void postRender( );

	void loadGameState( );
};

#endif // EMPTY_STATE_H
