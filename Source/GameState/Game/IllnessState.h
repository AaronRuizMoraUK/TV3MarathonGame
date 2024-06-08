#ifndef ILLNESS_STATE_H
#define ILLNESS_STATE_H

#include "GameState/IGameState.h"
#include "GameState/Game/InstructionsState.h"

class IllnessState : public IGameState
{
public:
	InstructionsState::InstructionsEnun next_instruction;

protected:
	Object background;

	CatalanAlphabetRender alphabet;

	bool fading_in;
	bool fading_out;

	// Parpadeo ENTER msg
	static const float print_enter_time;
	float curr_print_enter_time;
	bool print_enter;

public:
	IllnessState();
	~IllnessState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "illness_state"; }

protected:
	void update( float deltaTime );

	void preRender( );
	void renderWorld3D( );
	void renderWorld2D( );
	void renderPostProcessed2D( );
	void renderGameUI( );
	void postRender( );

	void loadGameState();

private:
	void selectIllnessBackground( );
};

#endif // ILLNESS_STATE_H
