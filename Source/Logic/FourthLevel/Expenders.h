#ifndef EXPENDERS_H
#define EXPENDERS_H

#include <vector>
#include "XMLParser/XMLParser.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"

struct Expender {
	IntelligentObject object;
	D3DXVECTOR3       initial_position;
	float             yaw;

	Expender( ) : initial_position(0,0,0), yaw( 0.0f ) { }
	void reset( );
};

class Expenders : public std::vector<Expender *>, public CXMLParser {

public:
	Expenders( ) {}
	~Expenders( );

	void load( );
	void onStartElement (const std::string &elem, MKeyValue &atts);

	void init( );
	void destroy( );

	void updateAI( float deltaTime );
	void updateAnimation( float deltaTime );
	void render( );
};

#endif //EXPENDERS_H
