#ifndef CREDITS_STATE_H
#define CREDITS_STATE_H

#include "GameState/IGameState.h"
#include "Util/Fade/TextureSequencer.h"

/**
* Menu credits state
*/

class CreditsState : public IGameState
{
protected:
	bool enter_credits;
	bool exit_credits;

	TextureSequencer texture_sequencer;

public:
	CreditsState();
	~CreditsState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "credits_state"; }

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

#endif // CREDITS_STATE_H
