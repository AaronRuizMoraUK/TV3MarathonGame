#ifndef INIT_FOURTH_LEVEL_STATE_H
#define INIT_FOURTH_LEVEL_STATE_H

#include "GameState/IGameState.h"

/**
* Init Fourth Level game state
*/

class InitFourthLevelState : public IGameState
{
protected:
	Object vignetting;

public:
	InitFourthLevelState();
	~InitFourthLevelState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "init_fourth_level_state"; };

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

#endif // INIT_FOURTH_LEVEL_STATE_H
