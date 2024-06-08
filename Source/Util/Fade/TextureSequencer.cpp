#include "TextureSequencer.h"
#include "Global/Variables.h"

void TextureSequencer::Screen::load( MKeyValue &atts ) {
	texture_fade.load( atts["texture"] );
	fade_time = atts.getFloat( "fade_time", 0.5f );
	duration  = atts.getFloat( "duration", 2.0f );

	assert( duration > 0.0f );
	assert( duration >= fade_time * 2.0f );
}

void TextureSequencer::Screen::reset( ) {
	time  = 0.0f;
	alpha = 0.0f;
}

bool TextureSequencer::Screen::update( float delta_time ) {
	time += delta_time;

	if( time <= fade_time )
		alpha = time / fade_time;
	else if( time >= duration - fade_time )
		alpha = (duration - time) / fade_time;
	else
		alpha = 1.0f;

	return time >= duration;
}

void TextureSequencer::Screen::render( ) {
	texture_fade.render( alpha );
}

void TextureSequencer::Screen::fadeOutNow( ) {
	// Si ya está haciendo fade out, no hacer nada
	if( time >= duration - fade_time )
		return;
	// Si estamos en fade in no hacer nada
	else if( time <= fade_time )
		//time = duration - time;
		return;
	// Si estamos a full, ponerlo a fade out
	else
		time = duration - fade_time;
}

// ----------------------------------------------------------------

TextureSequencer::TextureSequencer()
: idx_screen( 0 )
{
}

void TextureSequencer::load( const std::string &xmlFile ) {
	const std::string &xmlPath = Global::data_locator.getPath( "xml" );
	bool ok = xmlParseFile( xmlPath + xmlFile );
	assert( ok );
}

void TextureSequencer::initiate( ) {
	idx_screen = 0;
	for( size_t i=0; i<screens.size( ); ++i ) 
		screens[i].reset( );
}

void TextureSequencer::onStartElement( const std::string &elem, MKeyValue &atts ) {
	if( elem == "image" ) {
		Screen new_screen;
		new_screen.load( atts );
		screens.push_back( new_screen );
	}
}

bool TextureSequencer::update( float delta_time ) {
	assert( idx_screen < screens.size( ) );
	bool screen_finished = screens[ idx_screen ].update( delta_time );
	if( screen_finished )
		++idx_screen;
	return idx_screen == screens.size( );
}

void TextureSequencer::render( ) {
	assert( idx_screen < screens.size( ) );
	screens[ idx_screen ].render( );
}

void TextureSequencer::nextScreen( ) {
	if( idx_screen >= screens.size( ) )
		return;
	screens[ idx_screen ].fadeOutNow( );
}
