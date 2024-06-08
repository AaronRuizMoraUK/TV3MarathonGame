#ifndef SPHERE_RANDOM_WALK_CONTROLLER_H
#define SPHERE_RANDOM_WALK_CONTROLLER_H

#include "AIControllers/IAIController.h"
#include "Controllers/SphericalController.h"

class SphereRandomWalkController : public IAIController
{
protected:
	SphericalController spherical_controller;

	float max_speed;
	int   points_score_up;

	// Think time
	float max_think_time;
	float curr_max_think_time;
	float think_time;

	// Kick Variables
	float kick_distance;
	float kick_angle;  // Radians
	float kick_radius;

	bool dead;

	IntelligentObject *int_obj;

public:
	SphereRandomWalkController(
		float amax_speed
		, float amax_think_time
		, float arot_speed
		, float aacceleration
		, float akick_distance
		, float akick_angle
		, float akick_radius
		, int apoints_score_up );
	~SphereRandomWalkController();

	const char *getControllerType () const { return "SphereRandomWalkController"; }

	bool update(IntelligentObject &object, float deltaTime);

	void processSignal( TSignal tag, const void *data, size_t num_bytes );

	void collision( IntelligentObject &int_object, const Object &object, CollisionType collision_type, const D3DXVECTOR3 &normal );

private:
	bool hasPlayerKickMe( ) const;
};

#endif //SPHERE_RANDOM_WALK_CONTROLLER_H
