#ifndef FOURTH_LEVEL_STATE_H
#define FOURTH_LEVEL_STATE_H

#include "GameState/IGameState.h"
#include "UI/FourthLevelUI.h"

/**
* Fourth Level game state
*/

class FourthLevelState : public IGameState
{
protected:
	Object vignetting;

	// Hurt
	TextureFade hurt;
	static const float max_hurt_time;
	float  hurt_time;

	FourthLevelUI     fourth_level_ui;

public:
	FourthLevelState();
	~FourthLevelState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "fourth_level_state"; }

	inline void playerHurt( ) { hurt_time = max_hurt_time; }

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

#endif // FOURTH_LEVEL_STATE_H
