#include <cassert>
#include "ComicTextWindow.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Game/Game.h"
#include "IO/IOStatus.h"

std::string ComicTextWindow::pathToXml = "";

const float ComicTextWindow::fade_time = 0.5f;

ComicTextWindow::ComicTextWindow()
: comic_window_alpha( 0.0f )
, time( 0.0f )
, status( FADING_IN )
, comic_text( NULL )
, x( 0.524f )
, y( 0.912f )
, xl( 0.04f )
, yl( 0.04f )
, dist( 0.069f )
, num_lines( 6 )
, time_per_letter( 1.0f )
, comic_window_scale_x( 1.0f )
, comic_window_scale_y( 1.0f )
, comic_window_pos_x( 0.0f )
, comic_window_pos_y( 0.0f )
{}

ComicTextWindow::~ComicTextWindow() {
	printDebugString("Destroying ComicTextWindow...\n");

	if( comic_text )
		delete comic_text, comic_text = NULL;
}

void ComicTextWindow::initiate( const std::string &text ) {
	comic_text->createDinamicText( text, num_lines, time_per_letter );

	comic_window_alpha = 0.0f;
	time = 0.0f;
	status = FADING_IN;
}

bool ComicTextWindow::update( float deltaTime ) {
	switch( status ) {
		case FADING_IN:
			time += deltaTime;
			comic_window_alpha = time / fade_time;
			if( time >= fade_time ) {
				time = 0.0f;
				comic_window_alpha = 1.0f;
				status = WRITE_TEXT;
			}
			break;

		case WRITE_TEXT: {
			// Update dinamic text
			bool finished = comic_text->update( deltaTime );
			if( finished )
				status = FADING_OUT;
			break; }

		case FADING_OUT:
			time += deltaTime;
			comic_window_alpha = 1.0f - time / fade_time;
			if( time >= fade_time ) {
				comic_window_alpha = 0.0f;
				return true; // If we are here is because fade out is finished
			}
			break;

		default:
			assert( !"This should not happend" );
			break;
	}

	return false;
}

void ComicTextWindow::render( ) {
	comic_window.render( comic_window_alpha );

	//adjustComicWindow( );
	//adjustText( );

	if( status == WRITE_TEXT ) {
		for( size_t i=0; i<comic_text->getNumLines( ); i++)
			alphabet_renderer.print( x, y-i*dist, xl, yl, comic_text->getTextOnLine( i ) );
		//for( size_t i=0; i<num_lines; i++)
		//	alphabet_renderer.print( x, y-i*dist, xl, yl, "El perro de mi primo tiene polla, como los demas rabos del norte" );
	}
}

void ComicTextWindow::adjustText( ) {
#ifndef NDEBUG
	if( isPushed( 'Q' ) )
		x += 0.001f;
	else if( isPushed( 'A' ) )
		x -= 0.001f;
	else if( isPushed( 'W' ) )
		y += 0.001f;
	else if( isPushed( 'S' ) )
		y -= 0.001f;
	else if( isPushed( 'E' ) )
		dist += 0.0005f;
	else if( isPushed( 'D' ) )
		dist -= 0.0005f;
	else if( ioStatus.kick.becomesPressed() )
		num_lines += 1;
	else if( ioStatus.walkSlow.becomesPressed() )
		num_lines -= 1;
	printf( 0, 30, 0xffffffff, "Text: pos(%1.3f, %1.3f) letter(%1.3f, %1.3f) dis(%1.3f)", x, y, xl, yl, dist );
#endif
}

void ComicTextWindow::adjustComicWindow( ) {
#ifndef NDEBUG
	if( isPushed( 'R' ) )
		comic_window_scale_x += 0.001f;
	else if( isPushed( 'F' ) )
		comic_window_scale_x -= 0.001f;
	else if( isPushed( 'T' ) )
		comic_window_scale_y += 0.001f;
	else if( isPushed( 'G' ) )
		comic_window_scale_y -= 0.001f;
	else if( isPushed( 'Y' ) )
		comic_window_pos_x += 0.001f;
	else if( isPushed( 'H' ) )
		comic_window_pos_x -= 0.001f;
	else if( isPushed( 'U' ) )
		comic_window_pos_y += 0.001f;
	else if( isPushed( 'J' ) )
		comic_window_pos_y -= 0.001f;
	printf( 0, 40, 0xffffffff, "Comic Window: scale(%1.3f, %1.3f) pos(%1.3f, %1.3f)"
		, comic_window_scale_x, comic_window_scale_y, comic_window_pos_x, comic_window_pos_y );
	setComicWindowTransform( );
#endif
}

void ComicTextWindow::onStartElement(const std::string &elem, MKeyValue &atts) {
	if( elem == style_name ) {
		comic_window_texture = atts[ "comic_window_texture" ];
		assert( !comic_window_texture.empty( ) );
		comic_window_scale_x = atts.getFloat( "comic_window_scale_x", 1.0f );
		comic_window_scale_y = atts.getFloat( "comic_window_scale_y", 1.0f );
		comic_window_pos_x   = atts.getFloat( "comic_window_pos_x", 0.0f );
		comic_window_pos_y   = atts.getFloat( "comic_window_pos_y", 0.0f );

		x = atts.getFloat( "x", 0.524f );
		y = atts.getFloat( "y", 0.912f );
		xl = atts.getFloat( "xl", 0.04f );
		yl = atts.getFloat( "yl", 0.04f );
		dist = atts.getFloat( "dist", 0.069f );
		num_lines = atts.getInt( "num_lines", 6 );
		time_per_letter = atts.getFloat( "time_per_letter", 1.0f );

		comic_text_type = atts[ "comic_text_type" ];
		assert( comic_text == NULL );
		if( comic_text_type == "dinamic_text" )
			comic_text = new DinamicText( );
		else if( comic_text_type == "static_text" )
			comic_text = new StaticText( );
		else {
			assert( !"Unknown comic_text_type" );
			return;
		}
	}
}

void ComicTextWindow::load( const std::string &astyle_name ) {
	alphabet_renderer.load( );

	// Guardar el estilo que deseamos para leerlo de xml
	style_name = astyle_name;

	// Leer datos necesarios
	if( pathToXml.empty( ) )
		pathToXml = Global::data_locator.getPath("xml");
	bool ok = xmlParseFile( pathToXml + "comic_styles.xml" );
	assert( ok );

	comic_window.load( comic_window_texture );
	setComicWindowTransform( );
}

void ComicTextWindow::setComicWindowTransform( ) {
	D3DXMATRIX matrix;
	// 0.6  * 3/4 = 0.45
	// 0.45 * aspectRatio = 0.6 <- el ratio para la x que queremos. Así luego en 16/9 queda perfecto.
	D3DXMatrixScaling( &matrix, (comic_window_scale_x*3.0f/4.0f)*Game::configOptions.resolution.aspect_ratio, comic_window_scale_y, 1.0f );
	matrix(3,0) = comic_window_pos_x;
	matrix(3,1) = comic_window_pos_y;
	comic_window.setTransform( matrix );
}
