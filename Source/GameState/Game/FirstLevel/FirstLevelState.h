#ifndef FIRST_LEVEL_STATE_H
#define FIRST_LEVEL_STATE_H

#include "GameState/IGameState.h"
#include "UI/FirstLevelUI.h"

/**
* First Level game state
*/

class FirstLevelState : public IGameState
{
protected:
	Object vignetting;

	// Hurt
	TextureFade hurt;
	static const float max_hurt_time;
	float  hurt_time;

	FirstLevelUI     first_level_ui;

public:
	FirstLevelState();
	~FirstLevelState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "first_level_state"; }

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

#endif // FIRST_LEVEL_STATE_H
