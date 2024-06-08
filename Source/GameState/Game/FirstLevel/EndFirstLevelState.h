#ifndef END_FIRST_LEVEL_STATE_H
#define END_FIRST_LEVEL_STATE_H

#include "GameState/IGameState.h"
#include "GameState/Game/InstructionsState.h"

/**
* End First Level game state
*/

class EndFirstLevelState : public IGameState
{
public:
	InstructionsState::InstructionsEnun next_instruction;

protected:
	Object vignetting;

public:
	EndFirstLevelState();
	~EndFirstLevelState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "end_first_level_state"; };

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

#endif // END_FIRST_LEVEL_STATE_H
