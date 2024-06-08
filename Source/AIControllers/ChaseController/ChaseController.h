#ifndef CHASE_CONTROLLER_H
#define CHASE_CONTROLLER_H

#include "AIControllers/IAIController.h"

class ChaseController : public IAIController
{
public:
	ChaseController() {}
	~ChaseController();

	const char *getControllerType () const { return "ChaseController"; }

	bool update(IntelligentObject &object, float deltaTime);
};

#endif //CHASE_CONTROLLER_H
