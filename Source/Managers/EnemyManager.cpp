#include "EnemyManager.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Factories/MeshFactory.h"
#include "Factories/CollisionShapeFactory.h"
#include "Managers/ControllerManager.h"

std::string EnemyManager::pathXML = "";
EnemyManager * EnemyManager::enemyManager = NULL;

EnemyManager::EnemyManager()
: num_enemies( 0 )
, xml_enemy( NULL )
{
}

EnemyManager::~EnemyManager() {
	printDebugString("Destroying EnemyManager...\n");
}

void EnemyManager::load( ) {
	if( pathXML.empty( ) )
		pathXML = Global::data_locator.getPath( "xml" );
	assert( num_enemies == 0 );
	bool ok = xmlParseFile( pathXML + "enemies.xml" );
	assert( ok );
}

void EnemyManager::onStartElement( const std::string &elem, MKeyValue &atts ) {
	if( elem == "enemy" ) {
		assert( xml_enemy == NULL );
		xml_enemy = new Enemy( );
		assert( xml_enemy );
		xml_enemy->name = atts["name"];
	}
	else if( elem == "mesh" ) {
		assert( xml_enemy != NULL );
		const std::string &type = atts["type"];
		if( type == "cal" )
			xml_enemy->enemy_obj.setCalAnimModel( atts["name"].c_str( ) );
		else {
			const IMesh *mesh = MeshFactory::get( )->createMesh( atts["name"] );
			assert( mesh );
			xml_enemy->enemy_obj.setMesh( mesh );
		}
	}
	else if( elem == "material" ) {
		assert( xml_enemy != NULL );
		xml_enemy->enemy_obj.SETMATERIAL( atts["texture"], atts["shader"], atts["technique"] );
	}
	else if( elem == "AI" ) {
		assert( xml_enemy != NULL );
		IAIController *ai_controller = ControllerManager::get( )->getAIController( atts["name"] );
		assert( ai_controller );
		xml_enemy->enemy_obj.setAIController( ai_controller );
	}
	else if( elem == "collision" ) {
		assert( xml_enemy != NULL );
		const std::string &shape_type_name = atts[ "shape" ];
		float bias = atts.getFloat( "bias", 1.0f );

		CollisionShapeFactory::TypeShape shape_type = CollisionShapeFactory::getTypeShape( shape_type_name.c_str( ) );
		const ShapeHandle *shape_handle = CollisionShapeFactory::get( )->createCollisionShape( xml_enemy->enemy_obj.getMesh( )
			, shape_type, bias );
		assert( shape_handle );
		xml_enemy->enemy_obj.loc.createCollisionObject( shape_handle );
	}
}

void EnemyManager::onEndElement( const std::string &elem ) {
	if( elem == "enemy" ) {
		assert( num_enemies < max_enemies );
		size_t last_enemy = num_enemies;
		enemies[ last_enemy ] = *xml_enemy; // Copia

		// Increment num enemies
		++num_enemies;

		assert( xml_enemy != NULL );
		delete xml_enemy, xml_enemy = NULL;
	}
}

void EnemyManager::reload( ) {
	printDebugString( "Reloading enemies...\n" );
	num_enemies = 0;
	bool ok = xmlParseFile( pathXML + "enemies.xml" );
	assert( ok );
}

EnemyManager * EnemyManager::get() {
	if(enemyManager == NULL)
		enemyManager = new EnemyManager();

	return enemyManager;
}

IntelligentObject zero;
const IntelligentObject &EnemyManager::getEnemy( const std::string &name ) const {
	for( size_t i=0; i<num_enemies; ++i ) {
		if( enemies[ i ].name == name )
			return enemies[ i ].enemy_obj;
	}

	assert( !"Enemy not found in EnemyManager" );
	return zero;
}
