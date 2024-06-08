#include "LogicVolume.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Factories/MeshFactory.h"

LogicVolume::LogicVolume( const std::string &ascript_name ) 
: script_name(ascript_name)
, is_player_listener( false )
, is_inside( false )
, was_inside( false )
, listener( NULL )
, dead( false )
{
}

LogicVolume::~LogicVolume() {
	printDebugString( "Destroying LogicVolume...\n" );
	if( is_player_listener )
		Global::player->removeSignalListener( this );
}

bool LogicVolume::update( const D3DXVECTOR3 &point ) 
{
	if( !TriggerManager::triggers_enabled )
		return false;

	std::string script = "";
	bool is_inside = isInside( point );
	bool executeScript = false;

	// ON ENTER
	if(is_inside && !was_inside)
		script = "on_enter_"+script_name+".lua";
	// ON EXIT
	else if(!is_inside && was_inside)	
		script = "on_exit_"+script_name+".lua";

	if( !script.empty( ) )
		Global::lua_vm.execute( "logic_volumes/" + script );

	was_inside = is_inside;
	return dead;
}

void LogicVolume::registerToPlayerListeners( SignalListener *alistener ) {
	listener = alistener;

	// Si no esta ya registrado
	if( !is_player_listener ) {
		is_player_listener = true;
		Global::player->addSignalListener( this );
	}
}

// ------------------------------------------------------------------------------

LogicBox::LogicBox( const std::string &ascript_name, const D3DXVECTOR3 &amin_point, const D3DXVECTOR3 &amax_point )
: LogicVolume( ascript_name )
, BoundingBox( amin_point, amax_point )
{
}

// ------------------------------------------------------------------------------

LogicSphere::LogicSphere( const std::string &ascript_name, const D3DXVECTOR3 &center, float radius )
: LogicVolume( ascript_name )
, BoundingSphere( center, radius )
{
	const IMesh *box = MeshFactory::get( )->createMesh( "sphere.mesh" );
	assert( box );
	setMesh( box );
	SETMATERIAL( "logic_test.dds", "shader.fx", "basic_tech" );

	D3DXMATRIX transform;
	D3DXMatrixScaling( &transform, radius, radius, radius );
	loc.setTransform( transform );
	loc.setPosition( center );
}

void LogicSphere::setCenter( const D3DXVECTOR3 &acenter ) {
	center = acenter;
	loc.setPosition( center );
}
