#include "ChaseController.h"
#include "Global/Rendering.h"

ChaseController::~ChaseController() {
	printDebugString( "Destroying ChaseController...\n" );
}


bool ChaseController::update(IntelligentObject &object, float deltaTime) {
	return false;
}
