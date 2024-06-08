#ifndef GAME_OVER_STATE_H
#define GAME_OVER_STATE_H

#include "GameState/IGameState.h"
#include "Util/StringRender/AlphabetRender.h"

class GameOverState : public IGameState
{
protected:
	Object background;

	bool fading_in;
	bool fading_out;

	CatalanAlphabetRender alphabet;

	// Parpadeo ENTER msg
	static const float print_enter_time;
	float curr_print_enter_time;
	bool print_enter;

	DWORD sound;

public:
	GameOverState();
	~GameOverState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "game_over_state"; }

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

#endif // GAME_OVER_STATE_H
