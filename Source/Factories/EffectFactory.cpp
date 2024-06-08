#include <cassert>
#include "EffectFactory.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Game/Game.h"

std::string EffectFactory::pathToShaders = "";
EffectFactory * EffectFactory::effectFactory = NULL;

EffectFactory::EffectFactory( )
: num_effects( 0 )
{
}

EffectFactory::~EffectFactory(void) {
	printDebugString("Destroying EffectFactory...\n");

	// Destroy effects
	for( size_t i=0; i<num_effects; ++i ) {
		if( effects[i].fx ) {
			assert( !effects[i].name.empty( ) );
			effects[i].name = "";
			effects[i].fx->Release(), effects[i].fx = NULL;
		}
		else {
			assert( effects[i].name.empty( ) );
		}
	}
	num_effects = 0;
}

EffectFactory * EffectFactory::get() {
	if(effectFactory == NULL)
		effectFactory = new EffectFactory();

	return effectFactory;
}

const Effect *EffectFactory::createEffect(const std::string &filename) 
{
	Effect *effect_found = findEffect(filename);

	if( effect_found )
		return effect_found;
	else {
		const std::string &quality = Game::configOptions.quality;
		const std::string &pathFilename = getRealPathFilename( filename, quality );

		EffectDX effect_dx = NULL;
		bool ok = loadEffect(pathFilename, effect_dx);
		assert( ok );

		assert( num_effects < max_effects );
		size_t last_effect = num_effects;
		effects[ last_effect ].name = filename;
		effects[ last_effect ].quality = quality;
		effects[ last_effect ].fx   = effect_dx;

		// Increment num effects
		++num_effects;

		return &effects[ last_effect ];
	}
}

std::string temp = "";
const std::string &EffectFactory::getRealPathFilename( const std::string &filename, const std::string &quality ) {
	if( pathToShaders.empty( ) )
		pathToShaders = Global::data_locator.getPath("shader");

	std::string realFilename = filename;
	if( quality == "baix" || quality == "mitjà" ) {
		// Change name of shader to load
		std::string::size_type pos = realFilename.find(".fx");
		realFilename.insert(pos, "_" + quality);
	}
	temp = pathToShaders + realFilename;
	return temp;
}

void EffectFactory::reload( ) {
	printDebugString( "Reloading shaders...\n" );
	const std::string &quality = Game::configOptions.quality;

	for( size_t i=0; i<num_effects; ++i ) {
		const std::string &pathFilename = getRealPathFilename( effects[ i ].name, quality );
		loadEffect( pathFilename, effects[ i ].fx );
		effects[ i ].quality = quality;
	}
}

Effect *EffectFactory::findEffect( const std::string &filename ) {
	for( size_t i=0; i<num_effects; ++i ) {
		if( effects[ i ].name == filename )
			return &effects[ i ];
	}

	return NULL;
}

bool EffectFactory::loadEffect(const std::string &filename, EffectDX &effect) {
	if( effect )
		effect->Release( ), effect=NULL;

	HRESULT hr;
	LPD3DXBUFFER errorBuffer = NULL;
	hr = D3DXCreateEffectFromFile( 
		Global::device
		, filename.c_str()
		, NULL
		, NULL
		, (Game::configOptions.quality == "baix") ? D3DXSHADER_USE_LEGACY_D3DX9_31_DLL : 0
		, NULL
		, &effect
		, &errorBuffer
		);

	if( FAILED( hr ) ) {
		const char *errMsg = "Unknow error compiling fx";
		if( errorBuffer ) 
			errMsg = (const char *) errorBuffer->GetBufferPointer( );
		warn( "Error compiling FX file\n%s\n%s", filename.c_str(), errMsg );
		assert( !"Error compiling FX file in EffectFactory" );
		return false;
	}
	else {
		if( errorBuffer )
			errorBuffer->Release( ), errorBuffer=NULL;

		return true;
	}
}
