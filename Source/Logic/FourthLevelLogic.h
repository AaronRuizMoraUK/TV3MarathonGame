#ifndef FOURTH_LEVEL_LOGIC_H
#define FOURTH_LEVEL_LOGIC_H

#include "Logic.h"
#include "FourthLevel/DopaminasBrain4.h"
#include "FourthLevel/Twisters.h"
#include "FourthLevel/Expenders.h"
#include "PowerUps/PowerUps.h"

class FourthLevelLogic : public Logic {
public:
	// Enemies
	// NOTE: El update y los render no se encarga esta clase de logica
	DopaminasBrain4 dopaminas;
	DopaminasBrain4 noradrenalinas;
	Twisters  twisters;
	Expenders expenders;
	PowerUps  power_ups;

	// Dinamic Help variables
	bool must_show_welcome_image;
	bool show_dh_first_time_show_planet_gameplay_msg;
	bool show_dh_first_time_player_is_hit_by_twister;
	bool show_dh_first_time_expender_hit;
	bool show_dh_first_time_inside_interval_to_win;
	bool show_dh_first_time_outside_interval_to_win;

protected:
	// Logic parameters
	float time_twister_born;
	float time_to_born_powerup;

	float dopamina_level;
	float noradrenalina_level;

	bool	player_win;  // Por LUA se dice si el player gana

	bool  inside_interval_to_win;
	ParticleGroup *portal_particle;

	// XML
	float init_player_sync;
	int   init_num_dopaminas;
	int   init_num_noradrenalinas;
	int   init_num_twisters;
	int   min_num_dopaminas;
	int   max_num_dopaminas;
	int   min_num_noradrenalinas;
	int   max_num_noradrenalinas;
	int   max_num_twisters;
	float max_time_twister_born;

	float dopamina_start_right_percentage;
	float dopamina_end_right_percentage;
	float noradrenalina_start_right_percentage;
	float noradrenalina_end_right_percentage;
	std::string portal_particle_name;

	float syn_player_lose_per_second;
	
	float time_to_show_planet_gameplay_msg;
	float max_time_to_born_powerup;
	float portal_radius;

public:
	FourthLevelLogic();

	void destroyLevel( );
	void resetDinamicHelp( );

	void update( float deltaTime );

	void initLevel();
	inline bool isLevelWon() const { return player_win; }
	inline bool isLevelLost() const { return (player_sync==0.0f); }

	// -------------------------------------------------------------------
	inline void winLevel( ) { player_win = true; } // Por LUA

	inline int getMaxNumDopaminas( )      const { return max_num_dopaminas; }
	inline int getMaxNumNoradrenalinas( ) const { return max_num_noradrenalinas; }

	inline float getDopaminaLevel( ) const { return dopamina_level; }
	inline float getNoradrenalinaLevel( ) const { return noradrenalina_level; }

	inline bool isLevelsRight( ) const { return inside_interval_to_win; }
	
protected:
	void loadLogic( MKeyValue &atts );

private:
	void updatePercentages( );
	void updateRightInterval( );

	bool areLevelsInRightPercentages( );
	
	void obtainPortalPosition( D3DXVECTOR3 &out_portal_pos );
};

#endif //FOURTH_LEVEL_LOGIC_H
