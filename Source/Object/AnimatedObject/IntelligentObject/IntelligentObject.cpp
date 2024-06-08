#include "IntelligentObject.h"
#include "Global/Rendering.h"
#include "AIControllers/IAIController.h"
#include "AIControllers/ChaseController/ChaseController.h"
#include "AIControllers/SphereRandomWalkController/SphereRandomWalkController.h"
#include "AIControllers/AlucinacionSphereChase/AlucinacionSphereChase.h"
#include "AIControllers/Follower/Follower.h"
#include "AIControllers/TwisterChase/TwisterChase.h"
#include "AIControllers/ValveController/ValveController.h"
#include "AIControllers/ExpenderController/ExpenderController.h"
#include "AIControllers/DopaminaBrain4Controller/DopaminaBrain4Controller.h"
#include "AIControllers/PowerUpController/PowerUpController.h"

bool IntelligentObject::AI_enabled = true;

IntelligentObject::IntelligentObject()
: aiController(NULL)
{
}

IntelligentObject::IntelligentObject(const IntelligentObject &source)
: CalAnimatedObject(source)
, aiController(NULL)
{
	copy(source);
}

// Destructor
IntelligentObject::~IntelligentObject() {
	printDebugString("Destroying IntelligentObject...\n");
	destroy();
}

// Override Operator '='
void IntelligentObject::operator=(const IntelligentObject &source) {
	CalAnimatedObject::operator=(source);
	copy(source);
}

void IntelligentObject::destroy() {
	// Destroy AI controller
	if (aiController)
		delete aiController, aiController=NULL;
}

void IntelligentObject::copy(const IntelligentObject &source) {
	destroy( );

	if( source.aiController != NULL ) {
		const char *controller_type = source.aiController->getControllerType();

		if( strcmp( controller_type, "ChaseController" ) == 0 )
			aiController = new ChaseController( *((ChaseController *) source.aiController) );
		else if( strcmp( controller_type, "SphereRandomWalkController" ) == 0 )
			aiController = new SphereRandomWalkController( *((SphereRandomWalkController *) source.aiController) );
		else if( strcmp( controller_type, "AlucinacionSphereChase" ) == 0 )
			aiController = new AlucinacionSphereChase( *((AlucinacionSphereChase *) source.aiController) );
		else if( strcmp( controller_type, "Follower" ) == 0 )
			aiController = new Follower( *((Follower *) source.aiController) );
		else if( strcmp( controller_type, "TwisterChase" ) == 0 )
			aiController = new TwisterChase( *((TwisterChase *) source.aiController) );
		else if( strcmp( controller_type, "ValveController" ) == 0 )
			aiController = new ValveController( *((ValveController *) source.aiController) );
		else if( strcmp( controller_type, "ExpenderController" ) == 0 )
			aiController = new ExpenderController( *((ExpenderController *) source.aiController) );
		else if( strcmp( controller_type, "DopaminaBrain4Controller" ) == 0 )
			aiController = new DopaminaBrain4Controller( *((DopaminaBrain4Controller *) source.aiController) );
		else if( strcmp( controller_type, "PowerUpController" ) == 0 )
			aiController = new PowerUpController( *((PowerUpController *) source.aiController) );
		else {
			assert( !"Not type of AI controller allowed in IntelligentObject" );
			return;
		}
	}
}

bool IntelligentObject::updateAI(float deltaTime) {
	if (aiController && AI_enabled )
		return aiController->update(*this, deltaTime);
	else
		return false;
}

void IntelligentObject::setAIController(IAIController *pAiController) {
	if (aiController)
		delete aiController, aiController=NULL;
	aiController=pAiController;
}

void IntelligentObject::processSignal( TSignal tag, const void *data, size_t num_bytes ) {
	if(aiController)
		aiController->processSignal( tag, data, num_bytes );
}

void IntelligentObject::collisionWith( const Object &object, CollisionType collision_type, const D3DXVECTOR3 &normal ) {
	if( aiController )
		aiController->collision( *this, object, collision_type, normal );
}
