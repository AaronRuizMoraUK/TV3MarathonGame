#ifndef END_SECOND_LEVEL_STATE_H
#define END_SECOND_LEVEL_STATE_H

#include "GameState/IGameState.h"
#include "GameState/Game/InstructionsState.h"

/**
* End Second Level game state
*/

class EndSecondLevelState : public IGameState
{
public:
	InstructionsState::InstructionsEnun next_instruction;

protected:
	Object vignetting;

public:
	EndSecondLevelState();
	~EndSecondLevelState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "end_second_level_state"; };

protected:
	void update( float deltaTime );

	void preRender( );
	void renderWorld3D( );
	void renderWorld2D( );
	void renderPostProcessed2D( );
	void renderGameUI( );
	void postRender( );

	void loadGameState( );

	Object *cilindroSerotonina;
	Object *cilindroNoradrenalina;

	Object *CrearCilindro(const std::string &mesh_name, const std::string &texture, float alpha, float delta);
};

#endif // END_SECOND_LEVEL_STATE_H
