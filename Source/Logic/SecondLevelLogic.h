#ifndef SECOND_LEVEL_LOGIC_H
#define SECOND_LEVEL_LOGIC_H

#include "Logic.h"
#include "Directx/Directx.h"
#include "SecondLevel/Bombas.h"
#include "PowerUps/PowerUps.h"

class ParticleGroup;


class SecondLevelLogic : public Logic {
public:
	enum Status {
		SEROTONINA = 0,
		NORADRENALINA = 1
	};
	// Enemies
	// NOTE: El update y los render no se encarga esta clase de logica
	IntelligentObject *bomba;
	PowerUps           power_ups;

	// Dinamic Help variables
	bool must_show_welcome_image;
	bool show_dh_first_time_gets_bomb;
	bool show_dh_first_time_explodes_planted;
	bool show_dh_first_time_show_planet_gameplay;

protected:
	// Logic parameters
	float timer_rayos;
	
	ParticleGroup * particles;

	int serotoninaRemaining;
	int noradrenalinaRemaining;

	int	state;
	int previousState;
	bool mustDeleteBomba;

	int currentTipo;

	bool	player_win;

	float alphaRayo;
	float deltaRayo;
	DWORD idSonido;

	float current_syn_player_lose_per_second;
	float time_to_born_powerup;

	// XML
	float init_player_sync;
	int init_serotoninaRemaining;
	int init_noradrenalinaRemaining;
	float syn_player_lose_per_second;
	float radius_zona_plantar;
	float alpha_nacimiento;
	float delta_nacimiento;
	float time_to_show_planet_gameplay_msg;

	float minRandomLeft;
	float maxRandomLeft;
	float minRandomFront;
	float maxRandomFront;
	float max_time_to_born_powerup;

	float tiempo_rayos;
	float new_tiempo_rayos;

	D3DXMATRIX transNoradrenalina;
	D3DXMATRIX transSerotonina;

	Bombas rayos;

public:
	SecondLevelLogic();

	void destroyLevel( );
	void resetDinamicHelp( );

	void update( float deltaTime );

	void initLevel();
	inline bool isLevelWon() const { return state > 6; }
	inline bool isLevelLost() const { return (player_sync==0.0f); }

	inline void winLevel( ) { player_win = true; } // Por LUA

	inline int getSerotoninaRemaining() const {return serotoninaRemaining;}
	inline int getNoradrenalinaRemaining() const {return noradrenalinaRemaining;} 

	// -------------------------------------------------------------------

	void dentro_zona_plantar_noradrenalina();
	void dentro_zona_plantar_serotonina();
	void exploding(IntelligentObject &object, int status, int points_score_up);
	void showGetBombDinamicHelp();

	float GetTimerBomba();
	void playerInsideResidual(float deltaTime);

	float getRotationToFollowZona();

protected:
	void loadLogic( MKeyValue &atts );

	void processRayos (float deltaTime);

	void incrementarState() {++state; }

	void changeTexture();

	void createBomba();


};

#endif //SECOND_LEVEL_LOGIC_H
