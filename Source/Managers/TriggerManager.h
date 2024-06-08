#ifndef TRIGGER_MANAGER_H
#define TRIGGER_MANAGER_H

#include "XMLParser/XMLParser.h"
#include "Logic/Triggers/LogicTimer/LogicTimerManager.h"
#include "Logic/Triggers/LogicVolume/LogicVolumeManager.h"

/**
* Manager of triggers.
*/

class TriggerManager : public CXMLParser
{
public:
	static bool triggers_enabled;

protected:
	static std::string pathToXml;

	LogicTimerManager  logic_timer_manager;
	LogicVolumeManager logic_volume_manager;

public:
	TriggerManager() {}
	~TriggerManager( );
	void clear( );

	void update( float deltaTime );
	void render( );
	void renderDebug( );

	void addLogicTimer( LogicTimer *logic_timer ); // Va a pertenecer a este manager
	void addLogicTimer( const std::string &ascript_name, float timeout, LogicTimer::Type type );
	void delLogicTimer( const std::string &ascript_name );

	void addLogicVolume( LogicVolume *logic_volume ); // Va a pertenecer a este manager
	void addLogicVolume( const std::string &ascript_name, const D3DXVECTOR3 &amin_point, const D3DXVECTOR3 &amax_point );
	void addLogicVolume( const std::string &ascript_name, const D3DXVECTOR3 &center, float radius );
	void delLogicVolume( const std::string &ascript_name );

	void addUserEvent( const std::string &ascript_name );

	void load( const std::string &filename );
	void onStartElement(const std::string &elem, MKeyValue &atts);

	inline const LogicTimerManager &getLogicTimerManager( ) const { return logic_timer_manager; }
	inline const LogicVolumeManager &getLogicVolumeManager( ) const { return logic_volume_manager; }
};

#endif //TRIGGER_MANAGER_H
