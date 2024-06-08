#include "PlanetMapManager.h"
#include "Global/Variables.h"

PlanetMapManager * PlanetMapManager::planetMapManager = NULL;

void PlanetMapManager::load( ) {
	std::string pathFilename = Global::data_locator.getPath( "planetmap" ) + "planetmaps.xml";
	bool ok = xmlParseFile( pathFilename );
	assert( ok );
}

void PlanetMapManager::onStartElement( const std::string &elem, MKeyValue &atts ) {
	if( elem == "heightmap" )
		heightmaps.load( atts[ "file" ] );
	else if( elem == "normalmap" )
		normalmaps.load( atts[ "file" ] );
	else if( elem == "texturemap" )
		texturemaps.load( atts[ "file" ] );
}

void PlanetMapManager::reload( ) {
	printDebugString( "Reloading planetmaps...\n" );
	heightmaps.reload( );
	normalmaps.reload( );
	texturemaps.reload( );
}

PlanetMapManager * PlanetMapManager::get() {
	if(planetMapManager == NULL)
		planetMapManager = new PlanetMapManager();

	return planetMapManager;
}
