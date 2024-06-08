#ifndef INIT_THIRD_LEVEL_STATE_H
#define INIT_THIRD_LEVEL_STATE_H

#include "GameState/IGameState.h"

/**
* Init Third Level game state
*/

class InitThirdLevelState : public IGameState
{
protected:
	Object vignetting;

	Object safe_path;

public:
	InitThirdLevelState();
	~InitThirdLevelState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "init_third_level_state"; };

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

#endif // INIT_THIRD_LEVEL_STATE_H
