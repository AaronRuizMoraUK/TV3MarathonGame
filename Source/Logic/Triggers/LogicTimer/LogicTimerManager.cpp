#include <cassert>
#include "LogicTimerManager.h"
#include "Global/Rendering.h"

LogicTimerManager::~LogicTimerManager() {
	printDebugString("Destroying LogicTimerManager...\n");
	clear();
}

void LogicTimerManager::clear() {
	iterator it = begin();
	while(it != end()) {
		if( (*it) )
			delete (*it), (*it) = NULL;
		++it;
	}
	std::list< LogicTimer * >::clear( );
}

void LogicTimerManager::add( LogicTimer *logic_timer ) {
	assert( logic_timer );
	push_back( logic_timer );
}

void LogicTimerManager::add(const std::string &ascript_name, float timeout, LogicTimer::Type type) {
	LogicTimer *logic_timer = new LogicTimer( ascript_name, timeout, type );
	assert( logic_timer );
	push_back( logic_timer );
}

void LogicTimerManager::del(const std::string &ascript_name) {
	iterator it = begin();
	while( it != end() ) {
		assert( *it );
		if ( (*it)->getScriptName( ) == ascript_name )
			it = del( it );
		else
			++it;
	}
}

LogicTimerManager::iterator LogicTimerManager::del( iterator it ) {
	if( (*it) )
		delete (*it), (*it) = NULL;
	return erase( it );
}

void LogicTimerManager::update( float deltaTime ) {
	iterator it = begin();
	while( it != end( ) ) {
		assert( *it );
		if ( (*it)->update( deltaTime ) )
			it = del( it );
		else
			++it;
	}
}
