#ifndef TEXTURE_FACTORY_H
#define TEXTURE_FACTORY_H

#include "Directx/Directx.h"

/**
* Factory of textures.
*
* This is a singleton.
*/

class TextureFactory
{
protected:
	static std::string pathToTextures;

	static const size_t max_textures = 150;
	Texture textures[ max_textures ];
	size_t  num_textures;

public:
	~TextureFactory();

	static TextureFactory * get();

	const Texture *createTexture(const std::string &filename);
	void reload( );

private:
	static TextureFactory * textureFactory;
	TextureFactory();

	Texture *findTexture( const std::string &filename );

	bool loadTexture(const std::string &filename, TextureDX &texture);
};

#endif //TEXTURE_FACTORY_H
