#ifndef THIRD_LEVEL_LOGIC_H
#define THIRD_LEVEL_LOGIC_H

#include "Logic.h"
#include "ThirdLevel/Valves.h"
#include "ThirdLevel/River.h"
#include "PowerUps/PowerUps.h"

class ThirdLevelLogic : public Logic {
public:
	// Enemies
	// NOTE: El update y los render no se encarga esta clase de logica
	Valves   valves;
	River    river;
	PowerUps power_ups;

	// Dinamic Help variables
	bool must_show_welcome_image;
	bool show_dh_first_time_show_planet_gameplay_msg;

	static const unsigned NUM_VALVES = 4;
	bool show_dh_first_time_valve_hit[ NUM_VALVES ];

protected:
	// Logic parameters
	bool	player_win;  // Por Vaves se dice si el player gana
	float time_to_born_powerup;

	// XML
	float init_player_sync;

	float syn_player_lose_per_second_on_safe;
	float syn_player_lose_per_second_on_damage;

	float time_to_show_planet_gameplay_msg;
	float max_time_to_born_powerup;

public:
	ThirdLevelLogic();

	void destroyLevel( );
	void resetDinamicHelp( );

	void update( float deltaTime );

	void initLevel();
	inline bool isLevelWon() const { return player_win; }
	inline bool isLevelLost() const { return (player_sync==0.0f); }

	// -------------------------------------------------------------------
	inline void winLevel( ) { player_win = true; } // Por Valves

	inline const Valves::ValvesStatus &getValvesStatus( ) const { return valves.getValvesStatus( ); }
	
protected:
	void loadLogic( MKeyValue &atts );

};

#endif //THIRD_LEVEL_LOGIC_H
