#ifndef END_THIRD_LEVEL_STATE_H
#define END_THIRD_LEVEL_STATE_H

#include "GameState/IGameState.h"
#include "GameState/Game/InstructionsState.h"

/**
* End Third Level game state
*/

class EndThirdLevelState : public IGameState
{
public:
	InstructionsState::InstructionsEnun next_instruction;

protected:
	Object vignetting;

	Object safe_path;

public:
	EndThirdLevelState();
	~EndThirdLevelState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "end_third_level_state"; };

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

#endif // END_THIRD_LEVEL_STATE_H
