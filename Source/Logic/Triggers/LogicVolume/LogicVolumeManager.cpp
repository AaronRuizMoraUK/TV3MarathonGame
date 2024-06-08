#include "LogicVolumeManager.h"
#include "LogicVolume.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"

LogicVolumeManager::~LogicVolumeManager() {
	printDebugString("Destroying LogicVolumeManager...\n");
	clear();
}

void LogicVolumeManager::clear() {
	iterator it = begin();
	while(it != end()) {
		if( (*it) )
			delete (*it), (*it) = NULL;
		++it;
	}
	std::list< LogicVolume * >::clear( );
}

void LogicVolumeManager::add( LogicVolume *logic_volume ) {
	assert( logic_volume );
	push_back( logic_volume );
}

void LogicVolumeManager::add( const std::string &ascript_name, const D3DXVECTOR3 &amin_point, const D3DXVECTOR3 &amax_point ) {
	LogicVolume *logic_volume = new LogicBox( ascript_name, amin_point, amax_point );
	assert( logic_volume );
	push_back( logic_volume );
}

void LogicVolumeManager::add( const std::string &ascript_name, const D3DXVECTOR3 &center, float radius ) {
	LogicVolume *logic_volume = new LogicSphere( ascript_name, center, radius );
	assert( logic_volume );
	push_back( logic_volume );
}

void LogicVolumeManager::del( const std::string &ascript_name ) {
	iterator it = begin();
	while( it != end() ) {
		assert( *it );
		if ( (*it)->getScriptName( ) == ascript_name )
			it = del( it );
		else
			++it;
	}
}

LogicVolumeManager::iterator LogicVolumeManager::del( iterator it ) {
	if( (*it) )
		delete (*it), (*it) = NULL;
	return erase( it );
}

void LogicVolumeManager::update() {
	const D3DXVECTOR3 &player_pos = Global::player->loc.getPosition( );

	iterator it = begin();
	while( it != end( ) ) {
		assert( *it );
		if ( (*it)->update( player_pos ) )
			it = del( it );
		else
			++it;
	}
}

void LogicVolumeManager::render( ) {
	iterator it = begin();
	while(it != end()) {
		assert( *it );
		(*it)->render( );
		++it;
	}
}

void LogicVolumeManager::renderDebug( ) {
#ifndef NDEBUG
	iterator it = begin();
	while(it != end()) {
		assert( *it );
		(*it)->renderDebug( );
		++it;
	}
#endif
}
