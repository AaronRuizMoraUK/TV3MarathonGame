#ifndef SHOW_SCORES_STATE_H
#define SHOW_SCORES_STATE_H

#include "GameState/IGameState.h"
#include "Util/StringRender/AlphabetRender.h"

class ShowScoresState : public IGameState
{
protected:
	bool enter_scores;
	bool exit_scores;

	CatalanAlphabetColorRender alphabet;
	
	// Flowers pictures
	Object flowerUp;
	Object flowerDown;

public:
	ShowScoresState();
	~ShowScoresState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "show_scores_state"; }

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

#endif // SHOW_SCORES_STATE_H
