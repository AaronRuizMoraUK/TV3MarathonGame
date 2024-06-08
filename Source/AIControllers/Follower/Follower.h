#ifndef FOLLOWER_H
#define FOLLOWER_H

#include "AIControllers/IAIController.h"
#include "Controllers/SphericalController.h"

class Follower: public IAIController
{
public:
	enum Status {
		STOPED = 0
		, FOLLOWING
		, EXPLODING
		, PLANTED
	};

protected:
	SphericalController spherical_controller;

	float distance_player;
	float radius_chase;
	float radius_touch_player;
	float max_time;
	float current_time;
	int   points_score_up;

	// Status
	
	Status status;
	Status prev_status;

	bool dead;



public:
	Follower(
	float adistance_player,
	float arot_speed,
	float aaceleration, 
	float aradius_chase,
	float aradius_touch_player,
	float amax_time,
	int apoints_score_up
	);
	~Follower();

	const char *getControllerType () const { return "Follower"; }

	bool update(IntelligentObject &object, float deltaTime);

	void SetStatus(Status astatus) {status = astatus;}
	Status GetStatus() { return status; }

	inline float getTimeRemaining() const {return max_time - current_time; }

private:
	float getRotationToFollowPlayer( IntelligentObject &object ) const;
	void checkHitPlayer( IntelligentObject &object );
	void checkExplosion (IntelligentObject &object, float deltaTime);
	void checkMustFollowPlayer (IntelligentObject &object);
};

#endif //ALUCINACION_SPHERE_CHASE_H
