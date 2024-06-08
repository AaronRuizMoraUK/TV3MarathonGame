#ifndef LOGIC_TIMER_H
#define LOGIC_TIMER_H

#include <string>
#include "Util/Timeout/Timeout.h"
#include "Util/Signal/Signal.h"

/**
* Logic Timer
*/

class LogicTimer : public SignalListener, protected Timeout {
public:
	enum Type {
		SCRIPT = 0
		, COMMAND
		, NUM_TYPES
	};

protected:
	std::string scriptName;
	bool is_player_listener;

	Type type;
	
	SignalListener *listener; // A quien enviar las señales que recibe este timer logico
	
	bool dead;

public:
	LogicTimer( const std::string &pScriptName, float pTimeout, Type atype );
	~LogicTimer( );

	inline const std::string &getScriptName( ) const { return scriptName; }

	void registerToPlayerListeners( SignalListener *alistener );
	void processSignal( TSignal tag, const void *data, size_t num_bytes ) {
		if( listener )
			listener->processSignal( tag, data, num_bytes );
	}

	bool update( float deltaTime );
};

#endif //LOGIC_TIMER_H
