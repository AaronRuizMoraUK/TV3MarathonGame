#ifndef FIRST_LEVEL_LOGIC_H
#define FIRST_LEVEL_LOGIC_H

#include "Logic.h"
#include "Directx/Directx.h"
#include "FirstLevel/Dopaminas.h"
#include "FirstLevel/Alucinaciones.h"
#include "PowerUps/PowerUps.h"

class ParticleGroup;

class FirstLevelLogic : public Logic {
public:
	// Enemies
	// NOTE: El update y los render no se encarga esta clase de logica
	Dopaminas     dopaminas;
	Alucinaciones alucinaciones;
	PowerUps      power_ups;

	// Dinamic Help variables
	bool  must_show_welcome_image;
	bool  show_dh_first_time_inside_interval_to_win;
	bool  show_dh_first_time_outside_interval_to_win;
	bool  show_dh_first_time_player_kick_dopamina;
	bool  show_dh_first_time_player_is_hit_by_alucinacion;
	bool  show_dh_first_time_show_planet_gameplay_msg;

protected:
	// Logic parameters
	float dopamina_excess; // Porcetaje [0,1]
	float prev_dopamina_excess; // Porcetaje [0,1]
	float time_dopamina_born;
	float time_to_born_powerup;

	bool	player_win;  // Por LUA se dice si el player gana

	bool  inside_interval_to_win;
	ParticleGroup *portal_particle;

	// XML
	float init_player_sync;
	int init_num_alucinaciones;
	int init_num_dopaminas;
	int min_num_dopaminas;
	int max_num_dopaminas;

	size_t max_num_alucinaciones;
	float  max_time_dopamina_born;
	float  syn_player_lose_per_second;

	float  start_right_percentage;
	float  end_right_percentage;
	std::string portal_particle_name;

	float  time_to_show_planet_gameplay_msg;
	float  max_time_to_born_powerup;
	float  portal_radius;

public:
	FirstLevelLogic();

	void destroyLevel( );
	void resetDinamicHelp( );

	void update( float deltaTime );

	void initLevel();
	inline bool isLevelWon() const { return player_win; }
	inline bool isLevelLost() const { return (player_sync==0.0f); }

	// -------------------------------------------------------------------
	inline void winLevel( ) { player_win = true; } // Por LUA

	inline float getDopaminaExcess( ) const { return dopamina_excess; }
	inline bool isDopaminaLevelRight( ) const { return inside_interval_to_win; }
	
protected:
	void loadLogic( MKeyValue &atts );

private:
	void updateDopaminaExcess( );
	void updateRightInterval( );

	bool isDopaminaExcessInRightPercentages( ) const;
	bool mustBornAlucinacion( ) const;

	void obtainPortalPosition( D3DXVECTOR3 &out_portal_pos );
};

#endif //FIRST_LEVEL_LOGIC_H
