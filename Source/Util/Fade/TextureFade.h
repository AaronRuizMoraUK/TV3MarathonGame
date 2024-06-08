#ifndef TEXTURE_FADE_H
#define TEXTURE_FADE_H

#include "Directx/Directx.h"

class IMesh;

/**
* Class to make fades with texture.
* It is made to work with an orthogonal camera.
*/

class TextureFade
{
protected:
	D3DXMATRIX transform;

	const IMesh *mesh;

	const Effect *shader;
	static const char *shaderName;
	static const char *techniqueName;

	const Texture *texture;

public:
	TextureFade();
	~TextureFade();

	void setTransform( const D3DXMATRIX &m ) { transform = m; }
	void load( const std::string &textureName );
	void render( float alpha );
};

#endif //TEXTURE_FADE_H
