#ifndef INIT_SECOND_LEVEL_STATE_H
#define INIT_SECOND_LEVEL_STATE_H

#include "GameState/IGameState.h"

/**
* Init Second Level game state
*/

class InitSecondLevelState : public IGameState
{
protected:
	Object vignetting;

public:
	InitSecondLevelState();
	~InitSecondLevelState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "init_second_level_state"; };

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

#endif // INIT_SECOND_LEVEL_STATE_H
