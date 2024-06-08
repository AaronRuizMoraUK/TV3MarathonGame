#ifndef FINAL_CREDITS_STATE_H
#define FINAL_CREDITS_STATE_H

#include "GameState/IGameState.h"
#include "Util/Fade/TextureSequencer.h"

/**
* Final Game credits state
*/

class FinalCreditsState : public IGameState
{
protected:
	bool enter_credits;
	bool exit_credits;

	TextureSequencer texture_sequencer;
	Object					 background;

public:
	FinalCreditsState();
	~FinalCreditsState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "final_credits_state"; }

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

#endif // FINAL_CREDITS_STATE_H
