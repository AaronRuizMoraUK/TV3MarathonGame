#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "XMLParser/XMLParser.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"

/**
* Manager of enemies definitions.
*
* This is a singleton.
*/

class EnemyManager : public CXMLParser {

protected:
	static std::string pathXML;

	struct Enemy {
		std::string name;
		IntelligentObject enemy_obj;
		Enemy( ) : name( "" ) {}
	};

	static const size_t max_enemies = 10;
	Enemy   enemies[ max_enemies ];
	size_t  num_enemies;

private:
	// Just for reading XML
	Enemy *xml_enemy;

public:
	~EnemyManager();

	static EnemyManager * get();

	void load( );
	void onStartElement( const std::string &elem, MKeyValue &atts );
	void onEndElement( const std::string &elem );
	void reload( );

	// ESTO CONST!!!! Luego quien lo quiera que se haga una copia
	const IntelligentObject &getEnemy( const std::string &name ) const;

private:
	static EnemyManager * enemyManager;
	EnemyManager();

};

#endif //ENEMY_MANAGER_H
