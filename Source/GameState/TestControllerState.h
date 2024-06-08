#ifndef TEST_CONTROLLER_STATE_H
#define TEST_CONTROLLER_STATE_H

#include "IGameState.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"

class TestControllerState : public IGameState
{
protected:
	// Things of this game state (si necesitan cargarse hacerlo en la funcion loadGameState())
	IntelligentObject chaser;

public:
	TestControllerState( );
	~TestControllerState( );

	void onLoad( );
	void onUnload( );

	const char *getName( ) const { return "test_controller_state"; }

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

#endif // TEST_CONTROLLER_STATE_H
