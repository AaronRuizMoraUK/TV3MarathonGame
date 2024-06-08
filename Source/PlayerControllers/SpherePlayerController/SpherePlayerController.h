#ifndef SPHERE_PLAYER_CONTROLLER_H
#define SPHERE_PLAYER_CONTROLLER_H

#include "PlayerControllers/IPlayerController.h"
#include "Controllers/SphericalController.h"

class ParticleGroup;

class SpherePlayerController : public IPlayerController
{
protected:
	SphericalController spherical_controller;

	float walk_speed;
	float run_speed;
	float hit_speed;

	// Screen/Gamepad Angles
	float angle_desired;
	float curr_angle;

	// Status
	enum Status {
		STOPED = 0
		, WALKING
		, RUNNING
		, HITTED
	};
	Status status;
	Status prev_status;

	// Para no hacer una accion un monton de veces
	bool  is_player_doing_an_action;	
	float max_action_time;
	float action_time;

	bool signal_kick_sent;

	Player *player_obj;

	ParticleGroup *run_particle;

public:
	SpherePlayerController(float awalk_speed, float arun_speed, float ahit_speed, float arot_speed, float aacceleration);
	~SpherePlayerController();

	const char *getControllerType () const { return "SpherePlayerController"; }

	void update(Player &object, float deltaTime);
	float getCurrentAngle( ) const {return curr_angle;} // Solo para el controlador de camara

	void processSignal( TSignal tag, const void *data, size_t num_bytes );

	void collision( Player &player, const Object &object, CollisionType collision_type, const D3DXVECTOR3 &normal );

	inline float getCurrentSpeed() const {return spherical_controller.getCurrentSpeed(); }

private:
	void updateInput( );
	void updateActions( Player &object, float deltaTime );

	float getAngleToRotateCloserToCurrAngle( ) const;
	void updateAndFixCurrentAngle( );
};

#endif //SPHERE_PLAYER_CONTROLLER_H
