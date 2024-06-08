#ifndef LOGOS_STATE_H
#define LOGOS_STATE_H

#include "GameState/IGameState.h"
#include "Util/Fade/TextureSequencer.h"

class LogosState : public IGameState
{
protected:
	TextureSequencer texture_sequencer;

public:
	LogosState();
	~LogosState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "logos_state"; }

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

#endif // LOGOS_STATE_H
