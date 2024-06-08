#ifndef TWISTER_CHASE_H
#define TWISTER_CHASE_H

#include "AIControllers/IAIController.h"
#include "Controllers/SphericalController.h"

class TwisterChase : public IAIController
{
protected:
	SphericalController spherical_controller;

	float radius_chase;
	float radius_touch_player;
	float walk_speed;
	float run_speed;
	float points_hit_player;

	// Think time
	float max_think_time;
	float curr_max_think_time;
	float think_time;

	// Status
	enum Status {
		STOPED = 0
		, WALKING
		, HUNTING
	};
	Status status;
	Status prev_status;

	bool dead;

	DWORD sound;

public:
	TwisterChase( float aradius_chase
		, float aradius_touch_player
		, float awalk_speed
		, float arun_speed
		, float amax_think_time
		, float arot_speed
		, float aacceleration
		, float apoints_hit_player );
	~TwisterChase();

	const char *getControllerType () const { return "TwisterChase"; }

	bool update(IntelligentObject &object, float deltaTime);

	void collision( IntelligentObject &int_object, const Object &object, CollisionType collision_type, const D3DXVECTOR3 &normal );

private:
	void think( IntelligentObject &object, float deltaTime );
	float getRotationToFollowPlayer( IntelligentObject &object ) const;
	void checkHitPlayer( IntelligentObject &object );
};

#endif //TWISTER_CHASE_H
