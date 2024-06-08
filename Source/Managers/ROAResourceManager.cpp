#include <cassert>
#include "ROAResourceManager.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"

std::string ROAResourceManager::pathToROAS = "";
ROAResourceManager * ROAResourceManager::roaResourceManager = NULL;

ROAResourceManager::ROAResourceManager()
: num_resources( 0 ) 
{
}

ROAResourceManager::~ROAResourceManager() {
	printDebugString("Destroying ROAResourceManager...\n");
}

void ROAResourceManager::load( ) {
	if( pathToROAS.empty( ) )
		pathToROAS = Global::data_locator.getPath( "roa" );
	std::string pathFilename = pathToROAS + "roas.xml";
	bool ok = xmlParseFile( pathFilename );
	assert( ok );
}

void ROAResourceManager::onStartElement( const std::string &elem, MKeyValue &atts ) {
	if( elem == "ROA" ) {
		assert( num_resources < max_resources );
		size_t last_resource = num_resources;

		resources[ last_resource ].name = atts[ "file" ];
		std::string pathFilename = pathToROAS + resources[ last_resource ].name;
		bool ok = resources[ last_resource ].rigidAnimationResource.initFromFile( pathFilename.c_str( ) );
		assert( ok );

		// Increment num resources
		++num_resources;
	}
}

void ROAResourceManager::reload( ) {
	printDebugString( "Reloading ROA resources...\n" );

	for( size_t i=0; i<num_resources; ++i ) {
		std::string pathFilename = pathToROAS + resources[ i ].name;
		bool ok = resources[ i ].rigidAnimationResource.initFromFile( pathFilename.c_str( ) );
		assert( ok );
	}
}

ROAResourceManager * ROAResourceManager::get() {
	if(roaResourceManager == NULL)
		roaResourceManager = new ROAResourceManager();

	return roaResourceManager;
}

const ROAResource *ROAResourceManager::getResource( const std::string &filename ) const {
	for( size_t i=0; i<num_resources; ++i ) {
		if( resources[ i ].name == filename )
			return &resources[ i ];
	}

	assert( !"ROA Resource not found" );
	return NULL;
}
