#ifndef TEST_STATE_H
#define TEST_STATE_H

#include "IGameState.h"
#include "Object/Object.h"
#include "Object/AnimatedObject/AnimatedObject.h"
#include "Object/AnimatedObject/CalAnimatedObject.h"
#include "Object/Planet/Planet.h"

class TestState : public IGameState
{
protected:
	Object quad;
	Object quad2;
	AnimatedObject player;
	CalAnimatedObject player2;

	Planet planet;

public:
	~TestState( );

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "test_state"; };

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

#endif // TEST_STATE_H
