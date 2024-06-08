#ifndef LOGIC_VOLUME_MANAGER_H
#define LOGIC_VOLUME_MANAGER_H

#include <list>
#include "Directx/Directx.h"
#include "Expat/XMLParser.h"

class LogicVolume;

/**
* Manager of logic volumes for player.
*/

class LogicVolumeManager : public std::list< LogicVolume * >
{
public:
	LogicVolumeManager() {}
	~LogicVolumeManager();
	void clear();

	void add( LogicVolume *logic_volume ); // Va a pertenecer a este manager
	void add( const std::string &ascript_name, const D3DXVECTOR3 &amin_point, const D3DXVECTOR3 &amax_point );
	void add( const std::string &ascript_name, const D3DXVECTOR3 &center, float radius );
	void del( const std::string &ascript_name );
	iterator del( iterator it );

	void update( );
	void render( );
	void renderDebug( );
};

#endif //LOGIC_VOLUME_MANAGER_H
