#include "CalAnimCoreModelManager.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"

std::string CalAnimCoreModelManager::pathToCal = "";
CalAnimCoreModelManager * CalAnimCoreModelManager::calAnimCoreModelFactory = NULL;

CalAnimCoreModelManager::CalAnimCoreModelManager()
: num_cores( 0 )
{
	memset( cores, 0x00, sizeof( cores ) );
}

CalAnimCoreModelManager::~CalAnimCoreModelManager() {
	printDebugString("Destroying CalAnimCoreModelManager...\n");
	
	for( size_t i=0; i<max_cores; ++i ) {
		if( cores[ i ] )
			delete cores[ i ], cores[ i ] = NULL;
	}
	num_cores = 0;
}

void CalAnimCoreModelManager::load( ) {
	if( pathToCal.empty( ) )
		pathToCal = Global::data_locator.getPath("cal");

	bool ok = xmlParseFile( pathToCal + "cal_core_models.xml" );
	assert( ok );
}

CalAnimCoreModelManager * CalAnimCoreModelManager::get() {
	if(calAnimCoreModelFactory == NULL)
		calAnimCoreModelFactory = new CalAnimCoreModelManager();

	return calAnimCoreModelFactory;
}

const CalAnimCoreModel * CalAnimCoreModelManager::getCalAnimCoreModel(const std::string &filename) const {
	for( size_t i=0; i<num_cores; ++i ) {
		assert( cores[ i ] );
		if( cores[ i ]->name == filename )
			return cores[ i ];
	}
	assert( "!CalAnimCoreModel not found" );
	return NULL;
}

void CalAnimCoreModelManager::reload( ) {
	printDebugString( "Reloading Core Animations factors...\n" );
	for( size_t i=0; i<num_cores; ++i ) {
		assert( cores[ i ] );
		cores[ i ]->reloadTimeFactors( );
	}
}

void CalAnimCoreModelManager::onStartElement (const std::string &elem, MKeyValue &atts) {
	if (elem=="core_model") {
		const std::string &name = atts[ "name" ];
		std::string pathFilename = pathToCal + name + "/cal_core_model.xml";

		assert( num_cores < max_cores );
		size_t last_core = num_cores;

		assert( !cores[ last_core ] );
		cores[ last_core ] = new CalAnimCoreModel( name );
		assert( cores[ last_core ] );
		bool ok = cores[ last_core ]->xmlParseFile( pathFilename );
		assert( ok );

		// Increment num cores
		++num_cores;
	}
}
