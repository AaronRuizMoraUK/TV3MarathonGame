#ifndef DOPAMINA_BRAIN4_CONTROLLER_H
#define DOPAMINA_BRAIN4_CONTROLLER_H

#include "AIControllers/IAIController.h"
#include "Controllers/SphericalController.h"

class DopaminaBrain4Controller : public IAIController
{
public:
	enum Type {
		DOPAMINA = 0,
		NORADRENALINA
	};
	Type type;

protected:
	SphericalController spherical_controller;

	float max_speed;

	// Think time
	float max_think_time;
	float curr_max_think_time;
	float think_time;

	bool dead;

	IntelligentObject *int_obj;

public:
	DopaminaBrain4Controller(
		Type atype
		, float amax_speed
		, float amax_think_time
		, float arot_speed
		, float aacceleration );
	~DopaminaBrain4Controller();

	const char *getControllerType () const { return "DopaminaBrain4Controller"; }

	bool update(IntelligentObject &object, float deltaTime);

	void collision( IntelligentObject &int_object, const Object &object, CollisionType collision_type, const D3DXVECTOR3 &normal );

private:
	bool hasPlayerKickMe( ) const;
};

#endif //DOPAMINA_BRAIN4_CONTROLLER_H
