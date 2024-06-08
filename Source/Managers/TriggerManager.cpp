#include "TriggerManager.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"

bool TriggerManager::triggers_enabled = true;
std::string TriggerManager::pathToXml = "";

TriggerManager::~TriggerManager() {
	printDebugString("Destroying TriggerManager...\n");
	clear( );
}

void TriggerManager::clear( ) {
	logic_timer_manager.clear( );
	logic_volume_manager.clear( );
}

void TriggerManager::update( float deltaTime ) {
	if( !triggers_enabled )
		return;

	logic_timer_manager.update( deltaTime );
	logic_volume_manager.update( );
}

void TriggerManager::render( ) {
	logic_volume_manager.render( );
}

void TriggerManager::renderDebug( ) {
	logic_volume_manager.renderDebug( );
}

void TriggerManager::addLogicTimer( LogicTimer *logic_timer ) {
	logic_timer_manager.add( logic_timer );
}

void TriggerManager::addLogicTimer( const std::string &ascript_name, float timeout, LogicTimer::Type type ) {
	logic_timer_manager.add( ascript_name, timeout, type );
}

void TriggerManager::delLogicTimer( const std::string &ascript_name ) {
	logic_timer_manager.del( ascript_name );
}

void TriggerManager::addLogicVolume( LogicVolume *logic_volume ) {
	logic_volume_manager.add( logic_volume );
}

void TriggerManager::addLogicVolume( const std::string &ascript_name, const D3DXVECTOR3 &amin_point, const D3DXVECTOR3 &amax_point ) {
	logic_volume_manager.add( ascript_name, amin_point, amax_point );
}

void TriggerManager::addLogicVolume( const std::string &ascript_name, const D3DXVECTOR3 &center, float radius ) {
	logic_volume_manager.add( ascript_name, center, radius );
}

void TriggerManager::delLogicVolume( const std::string &ascript_name ) {
	logic_volume_manager.del( ascript_name );
}

void TriggerManager::addUserEvent(const std::string &ascript_name) {
	if( triggers_enabled )
		Global::lua_vm.execute( "user_events/"+ascript_name+".lua" );
}

void TriggerManager::load( const std::string &filename ) {
	if( pathToXml.empty( ) )
		pathToXml = Global::data_locator.getPath( "xml" );
	bool ok = xmlParseFile( pathToXml + filename );
	assert( ok );
}

void TriggerManager::onStartElement(const std::string &elem, MKeyValue &atts) {
	// Logic Volume
	if (elem == "logic_volume") 
	{
		const std::string &center_text = atts.getString( "center", "" );
		const std::string &script      = atts["script"];

		// If center is empty, then it is a BOX
		if( center_text.empty( ) ) {
			const std::string &transform_text = atts["transform"];
			D3DXMATRIX transform;
			D3DXMatrixIdentity(&transform);
			sscanf_s( transform_text.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f",
				&transform(0,0), &transform(0,1), &transform(0,2),
				&transform(1,0), &transform(1,1), &transform(1,2),
				&transform(2,0), &transform(2,1), &transform(2,2),
				&transform(3,0), &transform(3,1), &transform(3,2) );

			float width  = atts.getFloat("width",  0.0f);
			float height = atts.getFloat("height", 0.0f);
			float length = atts.getFloat("length", 0.0f);

			D3DXVECTOR3 pMin(-width*0.5f, 0.0f, -length*0.5f);
			D3DXVECTOR3 pMax(width*0.5f, height, length*0.5f);

			D3DXVec3TransformCoord(&pMin, &pMin, &transform);
			D3DXVec3TransformCoord(&pMax, &pMax, &transform);

			addLogicVolume( script, pMin, pMax );
		}
		// It is an SPHERE
		else {
			D3DXVECTOR3 center;
			sscanf_s( center_text.c_str(), "%f %f %f", &center.x, &center.y, &center.z );
			float radius = atts.getFloat( "radius", 0.0f );

			addLogicVolume( script, center, radius );
		}
	}
	// Logic Timer
	else if( elem == "logic_timer" ) {
		const std::string &script = atts["script"];
		float time                = atts.getFloat( "time", 0.0f );
		const std::string &type   = atts["type"];

		if( type == "SCRIPT" )
			addLogicTimer( script, time, LogicTimer::SCRIPT );
		else if( type == "COMMAND" )
			addLogicTimer( script, time, LogicTimer::COMMAND );
		else {
			assert( !"This should not happend!" );
			return;
		}
	}
}
