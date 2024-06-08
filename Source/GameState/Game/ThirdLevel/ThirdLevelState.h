#ifndef THIRD_LEVEL_STATE_H
#define THIRD_LEVEL_STATE_H

#include "GameState/IGameState.h"
#include "UI/ThirdLevelUI.h"

/**
* Third Level game state
*/

class ThirdLevelState : public IGameState
{
protected:
	Object vignetting;

	// Hurt
	TextureFade hurt;
	static const float max_hurt_time;
	float  hurt_time;

	ThirdLevelUI third_level_ui;
	Object       safe_path;

public:
	ThirdLevelState();
	~ThirdLevelState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "third_level_state"; }

	void playerHurt( );

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

#endif // THIRD_LEVEL_STATE_H
