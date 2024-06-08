#ifndef ISTRING_RENDER_H
#define ISTRING_RENDER_H

#include "Directx/Directx.h"
#include <string>

class IMesh;

/**
* This class make possible print strings with
* letters and numbers coming from a texture
*
* (0,0) is left-bottom. (1,1) is rigth-top.
*/

class IStringRender
{
protected:
	D3DXMATRIX transformMatrix;

	const IMesh *mesh;

	const Texture *texture;

	const Effect *shader;
	static const char *shaderName;

public:
	IStringRender( );
	virtual ~IStringRender( );

	void load( );

	void printf( float x, float y, float letter_width, float letter_height, const char *msg, ... );
	virtual void print( float x, float y, float letter_width, float letter_height, const char *msg ) = 0;

protected:
	virtual const char *getTextureName() const = 0;
	virtual const char *getTechnicName() const = 0;
};

#endif // ISTRING_RENDER_H
