#ifndef FINAL_SCORES_STATE_H
#define FINAL_SCORES_STATE_H

#include "GameState/IGameState.h"
#include "Util/StringRender/AlphabetRender.h"

/**
* Final Scores state
*/

class FinalScoresState : public IGameState
{
protected:
	bool enter_scores;
	bool exit_scores;

	Object background;
	
	CatalanAlphabetColorRender alphabet;

public:
	FinalScoresState();
	~FinalScoresState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "final_scores_state"; }

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

#endif // FINAL_SCORES_STATE_H
