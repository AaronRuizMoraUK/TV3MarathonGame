#ifndef SECOND_LEVEL_STATE_H
#define SECOND_LEVEL_STATE_H

#include "GameState/IGameState.h"
#include "UI/SecondLevelUI.h"

/**
* Second Level game state
*/

class SecondLevelState : public IGameState
{
protected:
	Object vignetting;

	// Hurt
	TextureFade hurt;
	static const float max_hurt_time;
	float  hurt_time;

	SecondLevelUI     second_level_ui;

public:
	SecondLevelState();
	~SecondLevelState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "second_level_state"; }

	inline void playerHurt( ) { hurt_time = max_hurt_time; }

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

#endif // SECOND_LEVEL_STATE_H
