#ifndef INIT_FIRST_LEVEL_STATE_H
#define INIT_FIRST_LEVEL_STATE_H

#include "GameState/IGameState.h"

/**
* Init First Level game state
*/

class InitFirstLevelState : public IGameState
{
protected:
	Object vignetting;

public:
	InitFirstLevelState();
	~InitFirstLevelState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "init_first_level_state"; };

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

#endif // INIT_FIRST_LEVEL_STATE_H
