#ifndef VALVES_H
#define VALVES_H

#include <vector>
#include "XMLParser/XMLParser.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "AIControllers/ValveController/ValveController.h"

// Valvulas con sus carteles

struct Valve {
	IntelligentObject object;
	Object            godray;
	D3DXVECTOR3       initial_position;
	float             yaw;

	Valve( ) : initial_position(0,0,0), yaw( 0.0f ) { }
	void reset( );
};

class Valves : public std::vector<Valve *>, public CXMLParser {
public:
	struct ValveStatus {
		ValveController::Status status;
		float time_left;
		bool  right_status;

		ValveStatus( )
			: status( ValveController::UNDEF )
			, time_left( 0.0f )
			, right_status( false )
		{
		}
	};
	typedef std::vector< ValveStatus > ValvesStatus;

protected:
	ValvesStatus valves_status;

	typedef std::vector<Object *> Carteles;
	Carteles carteles;

public:
	Valves( ) {}
	~Valves( );

	void load( );
	void onStartElement (const std::string &elem, MKeyValue &atts);

	void init( );
	void destroy( );

	void updateAI( float deltaTime );
	void updateAnimation( float deltaTime );
	void render( );
	void renderGodrays( );

	inline const ValvesStatus &getValvesStatus( ) const { return valves_status; }
};

#endif //VALVES_H
