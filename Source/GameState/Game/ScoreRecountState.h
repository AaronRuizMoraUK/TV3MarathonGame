#ifndef SCORE_RECOUNT_STATE_H
#define SCORE_RECOUNT_STATE_H

#include "GameState/IGameState.h"
#include "GameState/Game/InstructionsState.h"
#include "Util/StringRender/AlphabetRender.h"
#include "Util/StringRender/UINumberRender.h"

class ScoreRecountState : public IGameState
{
public:
	InstructionsState::InstructionsEnun next_instruction;

protected:
	Object background;

	Object sync_level_picture;
	Object score_picture;

	CatalanAlphabetRender alphabet;

	UINumberRender numbers;

	bool fading_in;
	bool fading_out;
	bool counting;

	// Parpadeo ENTER msg
	static const float print_enter_time;
	float curr_print_enter_time;
	bool print_enter;

	static const int score_up_per_sync = 2;
	size_t logic_score;
	int logic_player_sync;
	static const float time_one_point_score_up;
	float time_score_up;
	static const float sound_time_one_point_score_up;
	float sound_time_score_up;

	DWORD sound;

public:
	ScoreRecountState();
	~ScoreRecountState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "score_recount_state"; }

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

#endif // SCORE_RECOUNT_STATE_H
