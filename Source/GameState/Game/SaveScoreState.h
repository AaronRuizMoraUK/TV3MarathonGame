#ifndef SAVE_SCORE_STATE_H
#define SAVE_SCORE_STATE_H

#include "GameState/IGameState.h"
#include "Util/StringRender/AlphabetRender.h"
#include "Util/ScoreRecorder/ScoreRecorder.h"

class SaveScoreState : public IGameState
{
protected:
	Object background;
	CatalanAlphabetRender alphabet;

	bool fading_in;
	bool fading_out;

	static const char letters[];
	char              initials[ NUM_CHARS_PER_INITIALS ];
	size_t            initials_idx;
	size_t            letters_idx;

	// Parpadeo
	static const float render_initial_idx_time;
	float              curr_render_initial_idx_time;
	bool               render_initial_idx;

public:
	SaveScoreState();
	~SaveScoreState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "save_score_state"; }

protected:
	void update( float deltaTime );

	void preRender( );
	void renderWorld3D( );
	void renderWorld2D( );
	void renderPostProcessed2D( );
	void renderGameUI( );
	void postRender( );

	void loadGameState();
};

#endif // SAVE_SCORE_STATE_H
