#include "BrainManager.h"
#include "PlanetMapManager.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Factories/MeshFactory.h"

BrainManager * BrainManager::brainManager = NULL;

BrainManager::BrainManager()
: num_brains( 0 )
, current_brain( 0 )
{
}

BrainManager::~BrainManager() {
	printDebugString("Destroying BrainManager...\n");
}

void BrainManager::load( ) {
	const std::string &pathXML = Global::data_locator.getPath( "xml" );
	assert( num_brains == 0 );
	bool ok = xmlParseFile( pathXML + "brains.xml" );
	assert( ok );
}

void BrainManager::onStartElement( const std::string &elem, MKeyValue &atts ) {
	if( elem == "brain" ) {
		assert( num_brains < max_brains );
		size_t last_brain = num_brains;
		++num_brains;
		
		const IMesh *brain_mesh = MeshFactory::get( )->createMesh( atts["mesh"] );
		assert( brain_mesh );
		brains[ last_brain ].setMesh( brain_mesh );
		brains[ last_brain ].SETMATERIAL( atts["texture"], atts["shader"], atts["technique"] );

		float backside_frustum_radius = atts.getFloat( "backside_frustum_radius", 20.0f );
		brains[ last_brain ].setBackSideFrustumRadius( backside_frustum_radius );

		const std::string &heightmap = atts.getString( "heightmap", "" );
		if( !heightmap.empty( ) )
			brains[ last_brain ].setHeightMap( PlanetMapManager::get( )->getHeightMap( heightmap ) );

		const std::string &normalmap = atts.getString( "normalmap", "" );
		if( !normalmap.empty( ) )
			brains[ last_brain ].setNormalMap( PlanetMapManager::get( )->getNormalMap( normalmap ) );

		const std::string &texturemap = atts.getString( "texturemap", "" );
		if( !texturemap.empty( ) )
			brains[ last_brain ].setTextureMap( PlanetMapManager::get( )->getTextureMap( texturemap ) );
	}
}

BrainManager * BrainManager::get() {
	if(brainManager == NULL)
		brainManager = new BrainManager();

	return brainManager;
}

Planet &BrainManager::getCurrentBrain( ) {
	assert( current_brain < num_brains );
	return brains[ current_brain ];
}

void BrainManager::setCurrentBrain( size_t index ) {
	--index; // Para ponerlo de [0,n-1]
	assert( index < num_brains );
	current_brain = index;
}
