#include "CorePSubsystem.h"
#include "PSManager.h"

CorePSubsystem::CorePSubsystem(void)
: maxParticles( 0 )
, shaderType( "" )
, particleColorFactor( 0.0f )
, texture( NULL )
{
}

CorePSubsystem::~CorePSubsystem(void) {
	destroy( );
}

void CorePSubsystem::destroy( ) {
	// NO FUNCIONA EN PAPI
	//for( size_t i = 0; i < updateParamsVector.size( ); ++i )
	//	PSManager::particleEngine.DeleteActionLists( updateParamsVector[ i ].updateHandle );
	updateParamsVector.clear( );

	maxParticles = 0;
	shaderType = "";
	particleColorFactor = 0.0f;
	texture = NULL;
}

void CorePSubsystem::setUpdateHandle(int handle, float init, float finish) {
	TUpdateParams params; 
	params.updateHandle = handle; 
	params.initTime = init;
	params.finishTime = finish;

	updateParamsVector.push_back(params);
}

int CorePSubsystem::getUpdateHandle(float lifeTime) const { 
	size_t size = updateParamsVector.size();
	for (size_t index = 0; index < size; ++index) {
		float finishTime = updateParamsVector[index].finishTime;
		float initTime = updateParamsVector[index].initTime;
		int updateHandle = updateParamsVector[index].updateHandle;
		if (finishTime < 0.0f) 
			return updateHandle;

		if ((initTime <= lifeTime) && (lifeTime <= finishTime)) {
			return updateHandle;
		}
	}

	// Si no hay handle valido, se devuelve -1
	return -1;
}
