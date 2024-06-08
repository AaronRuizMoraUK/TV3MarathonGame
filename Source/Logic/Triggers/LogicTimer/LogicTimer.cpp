#include "LogicTimer.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"

LogicTimer::LogicTimer( const std::string &pScriptName, float pTimeout, Type atype ) 
: Timeout( pTimeout )
, scriptName(pScriptName)
, is_player_listener( false )
, type( atype )
, listener( NULL )
, dead( false )
{
}

LogicTimer::~LogicTimer( ) {
	printDebugString( "Destroying LogicTimer...\n" );
	if( is_player_listener )
		Global::player->removeSignalListener( this );
}

bool LogicTimer::update( float deltaTime )
{
	if( !TriggerManager::triggers_enabled )
		return false;

	Timeout::update( deltaTime );
	if( isFinished( ) ) {
		if( type == SCRIPT )
			Global::lua_vm.execute( "timeouts/"+scriptName+".lua" );
		else if( type == COMMAND )
			Global::lua_vm.executeCmd( scriptName.c_str( ) );
		else {
			assert( !"This should not happend!" );
			return true;
		}
		return true;
	}
	return dead;
}

void LogicTimer::registerToPlayerListeners( SignalListener *alistener ) {
	listener = alistener;

	// Si no esta ya registrado
	if( !is_player_listener ) {
		is_player_listener = true;
		Global::player->addSignalListener( this );
	}
}
