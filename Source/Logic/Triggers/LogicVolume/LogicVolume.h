#ifndef LOGIC_VOLUME_H
#define LOGIC_VOLUME_H

#include <string>
#include "Directx/Directx.h"
#include "Object/Object.h"
#include "Util/Bounding/Bounding.h"

/**
* Interface of a Logic Volume
*/

class LogicVolume : protected Object
{
protected:
	std::string script_name;
	bool is_player_listener;

	bool is_inside;
	bool was_inside;

	SignalListener *listener; // A quien enviar las señales que recibe este volumen lógico

	bool dead;

public:
	LogicVolume( const std::string &ascript_name );
	virtual ~LogicVolume();

	inline const std::string &getScriptName( ) const { return script_name; }

	bool update( const D3DXVECTOR3 &point );

	virtual void render( ) { Object::render( ); }
	virtual void renderDebug( ) = 0;

	virtual const char *getType( ) const = 0;

	void registerToPlayerListeners( SignalListener *alistener );
	void processSignal( TSignal tag, const void *data, size_t num_bytes ) {
		if( listener && is_inside )
			listener->processSignal( tag, data, num_bytes );
	}

protected:
	virtual bool isInside( const D3DXVECTOR3 &point ) const = 0;
};

// ------------------------------------------------------------------------

class LogicBox : public LogicVolume, public BoundingBox {
public:
	LogicBox( const std::string &ascript_name, const D3DXVECTOR3 &amin_point, const D3DXVECTOR3 &amax_point );

	void render( ) { return; } // No implementado el render para los bounding box
	void renderDebug( ) { BoundingBox::renderDebug( ); }

	const char *getType( ) const { return "LogicBox"; }

protected:
	inline bool isInside( const D3DXVECTOR3 &point ) const { return isPointInside( point ); }
};

// ------------------------------------------------------------------------

class LogicSphere : public LogicVolume, public BoundingSphere {
public:
	LogicSphere( const std::string &ascript_name, const D3DXVECTOR3 &center, float radius );

	void renderDebug( ) { BoundingSphere::renderDebug( ); }

	const char *getType( ) const { return "LogicSphere"; }

	inline const D3DXVECTOR3 &getCenter( ) const { return center; }
	void setCenter( const D3DXVECTOR3 &acenter );

protected:
	inline bool isInside( const D3DXVECTOR3 &point ) const { return isPointInside( point ); }
};

#endif //LOGIC_VOLUME_H
