#include <cassert>
#include "TextureFactory.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"

std::string TextureFactory::pathToTextures = "";
TextureFactory * TextureFactory::textureFactory = NULL;

TextureFactory::TextureFactory( )
: num_textures( 0 )
{
}

TextureFactory::~TextureFactory() {
	printDebugString("Destroying TextureFactory...\n");

	// Destroy textures
	for( size_t i=0; i<num_textures; ++i ) {
		if( textures[i].tex ) {
			assert( !textures[i].name.empty( ) );
			textures[i].name = "";
			textures[i].tex->Release(), textures[i].tex = NULL;
		}
		else {
			assert( textures[i].name.empty( ) );
		}
	}
	num_textures = 0;
}

TextureFactory * TextureFactory::get() {
	if(textureFactory == NULL)
		textureFactory = new TextureFactory();

	return textureFactory;
}

const Texture *TextureFactory::createTexture(const std::string &filename) {

	Texture *texture_found = findTexture(filename);

	if( texture_found )
		return texture_found;
	else {
		if( pathToTextures.empty( ) )
			pathToTextures = Global::data_locator.getPath("texture");
		std::string pathFilename = pathToTextures + filename;

		TextureDX texture_dx = NULL;
		bool ok = loadTexture( pathFilename, texture_dx );
		assert( ok );

		assert( num_textures < max_textures );
		size_t last_texture = num_textures;
		textures[ last_texture ].name = filename;
		textures[ last_texture ].tex  = texture_dx;

		// Increment num textures
		++num_textures;

		return &textures[ last_texture ];
	}
}

void TextureFactory::reload( ) {
	printDebugString( "Reloading textures...\n" );

	for( size_t i=0; i<num_textures; ++i ) {
		std::string pathFilename = pathToTextures + textures[ i ].name;
		loadTexture( pathFilename, textures[ i ].tex );
	}
}

Texture *TextureFactory::findTexture( const std::string &filename ) {
	for( size_t i=0; i<num_textures; ++i ) {
		if( textures[ i ].name == filename )
			return &textures[ i ];
	}

	return NULL;
}

bool TextureFactory::loadTexture(const std::string &filename, TextureDX &texture) {
	if( texture )
		texture->Release( ), texture=NULL;

	// Use D3DX to create a texture from a file based image
	assert( Global::device || !"Device not created" );
	HRESULT hr;
	hr = D3DXCreateTextureFromFileEx( Global::device, filename.c_str(),
		D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN,
		D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture );

	if( FAILED( hr ) )
	{
		warn( "Could not load texture: %s", filename.c_str() );
		assert( !"Could not load texture in TextureFactory" );

		return false;
	}
	else
		return true;
}
