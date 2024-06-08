#include <cassert>
#include "IStringRender.h"
#include "Factories/QuadFactory.h"
#include "Factories/TextureFactory.h"
#include "Factories/EffectFactory.h"
#include "Global/Rendering.h"

const char *IStringRender::shaderName = "string_render.fx";

IStringRender::IStringRender( )
: mesh( NULL )
, texture( NULL )
, shader( NULL )
{
	D3DXMatrixIdentity( &transformMatrix );
}

IStringRender::~IStringRender( ) {
	printDebugString("Destroying IStringRender...\n");
}

void IStringRender::load( ) {
	mesh = QuadFactory::get()->createQuad(0.5f, 0.5f);
	assert( mesh );

	texture = TextureFactory::get()->createTexture( getTextureName() );
	assert( texture );

	shader = EffectFactory::get()->createEffect( shaderName );
	assert( shader );
}

void IStringRender::printf( float x, float y, float letter_width, float letter_height, const char *msg, ... ) {
	char txt[512];
	va_list ap;
	va_start(ap, msg);
	vsnprintf_s(txt, sizeof( txt ), 511, msg, ap);
	va_end (ap);

	print( x, y, letter_width, letter_height, txt );
}
