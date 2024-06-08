#ifndef END_FOURTH_LEVEL_STATE_H
#define END_FOURTH_LEVEL_STATE_H

#include "GameState/IGameState.h"
#include "GameState/Game/InstructionsState.h"

/**
* End Fourth Level game state
*/

class EndFourthLevelState : public IGameState
{
public:
	InstructionsState::InstructionsEnun next_instruction;
	
protected:
	Object vignetting;

public:
	EndFourthLevelState();
	~EndFourthLevelState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "end_fourth_level_state"; };

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

#endif // END_FOURTH_LEVEL_STATE_H
