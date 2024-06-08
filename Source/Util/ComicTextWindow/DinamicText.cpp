#include <cassert>
#include "DinamicText.h"
#include "Global/Rendering.h"
#include "IO/IOStatus.h"
#include "SoundPlayer/SoundPlayer.h"

// ----------------------------------------------------------------
DinamicText::DinamicText( )
: num_lines( 6 )
, time_per_letter( 1.0f )
, curr_time( 0.0f )
, curr_sheet_id( 0 )
{ }

DinamicText::~DinamicText( ) {
	printDebugString( "Destroying DinamicText...\n" );
}

void DinamicText::createDinamicText( const std::string &text, size_t anum_lines, float atime_per_letter ) {
	assert( !text.empty( ) );
	assert( anum_lines > 0 );
	assert( atime_per_letter > 0.0f );

	// Limpiar las sheets
	sheets.clear( );

	// Iniciar variables
	num_lines = anum_lines;
	time_per_letter = atime_per_letter;
	curr_time = 0.0f;
	curr_sheet_id = 0;

	// Rellenar las sheets
	static const std::string next_line_key( "\\ " );
	bool exit = false;
	size_t index = 0;
	while( !exit ) {			
		// Nuevo sheet, meterle num_lines líneas en blanco
		TSheet sheet;
		for( size_t i=0; i<num_lines; ++i )
			sheet.push_back( "" );

		// Rellenar las num_lines líneas
		for( size_t i=0; i<num_lines; ++i ) {
			// Buscamos next_line_key
			size_t index_next_line = text.find( next_line_key, index );

			// Si no lo hemos encontrado significa que el resto del texto va en esta línea
			// y ya no seguimos, ya que seguro que no hay más texto
			if( index_next_line == text.npos ) {
				sheet[ i ] = text.substr( index );
				exit = true;
				i=num_lines; // Como break;
			}
			else {
				size_t lenght = index_next_line - index;
				sheet[ i ] = text.substr( index, lenght );
				index = index_next_line + next_line_key.size( ); // para saltar el next_line_key
			}
		}

		// Añadir el nuevo sheet
		sheets.push_back( sheet );
	}

	// Limpiar la current sheet que se está mostrando
	current_sheet.clear( );
	for( size_t i=0; i<num_lines; ++i )
		current_sheet.push_back( "" );
}

void DinamicText::showWholeSheet( ) {
	assert( curr_sheet_id >= 0 && curr_sheet_id < sheets.size( ) );
	current_sheet.clear( );
	current_sheet = sheets[ curr_sheet_id ]; // Copiar directamente la sheet
}

bool DinamicText::isSheetFinished( ) const {
	assert( curr_sheet_id >= 0 && curr_sheet_id < sheets.size( ) );
	for( size_t i=0; i<num_lines; ++i ) {
		// Si una línea no tiene el mismo tamañan entonces no hemos terminado
		if( current_sheet[ i ].size( ) != sheets[ curr_sheet_id ][ i ].size( ) )
			return false;
	}
	return true;
}

bool DinamicText::nextSheet( ) {
	// Limpiar la sheet actual
	current_sheet.clear( );
	for( size_t i=0; i<num_lines; ++i )
		current_sheet.push_back( "" );

	// Incrementar la sheet id e inicializar el tiempo
	++curr_sheet_id;
	curr_time = 0.0f;

	if( curr_sheet_id >= sheets.size( ) ) {
		curr_sheet_id = sheets.size( ) - 1; // Quedarse en la última sheet
		return true;
	}
	else
		return false;
}

bool DinamicText::update( float deltaTime ) {

	curr_time += deltaTime;
	// Si es el momento de añadir la siguiente letra...
	if( curr_time >= time_per_letter ) {
		curr_time = curr_time - time_per_letter;

		for( size_t i=0; i<num_lines; ++i ) {
			// A la primera línea que no tenga el tamañano igual se le añade la siguiente letra
			if( current_sheet[ i ].size( ) < sheets[ curr_sheet_id ][ i ].size( ) ) {
				size_t idx = current_sheet[ i ].size( );
				const std::string &new_letter = sheets[ curr_sheet_id ][ i ].substr( idx, 1 );
				current_sheet[ i ].append( new_letter );
				break;
			}
		}
	}

	// If exit button is pressed finish
	if( ioStatus.exitOptions.becomesPressed( ) ) {
		SoundPlayer::get( )->playSample( "dynamic_text_exit" );
		return true;
	}

	// Is current sheet finished?
	if( isSheetFinished( ) ) {
		// If we pressed enter button go to next sheet
		if( ioStatus.start.becomesPressed() 
			|| ioStatus.selectOption.becomesPressed() ) {
				bool no_more_sheets = nextSheet( );

				// If no more sheets finish
				if( no_more_sheets ) {
					SoundPlayer::get( )->playSample( "dynamic_text_exit" );
					return true;
				}
				// Go to next sheet
				else {
					SoundPlayer::get( )->playSample( "dynamic_text_continue" );
				}
		}
	}
	else {
		// If we pressed enter button show whole current sheet
		if( ioStatus.start.becomesPressed() 
			|| ioStatus.selectOption.becomesPressed() ) {
				SoundPlayer::get( )->playSample( "dynamic_text_continue" );
				showWholeSheet( );
		}
	}

	return false;
}

const char *DinamicText::getTextOnLine( size_t line ) const {
	assert( line >= 0 && line < current_sheet.size( ) );
	return current_sheet[ line ].c_str( );
}

// ----------------------------------------------------------------
StaticText::StaticText( )
: num_lines( 6 )
, curr_time( 0.0f )
, curr_sheet_id( 0 )
, current_sheet( NULL )
{ }

StaticText::~StaticText( ) {
	printDebugString( "Destroying StaticText...\n" );
}

void StaticText::createDinamicText( const std::string &text, size_t anum_lines, float atime_per_letter ) {
	assert( !text.empty( ) );
	assert( anum_lines > 0 );
	(void)atime_per_letter;

	// Limpiar los tiempos
	next_sheet_times.clear( );

	// Limpiar las sheets
	current_sheet = NULL;
	sheets.clear( );

	// Iniciar variables
	num_lines = anum_lines;
	curr_time = 0.0f;
	curr_sheet_id = 0;

	// Rellenar las sheets
	static const std::string next_line_key( "\\ " );
	static const std::string end_times_key( "/ " );
	bool exit = false;
	size_t index = 0;

	// Obtain times information
	size_t index_times_string = text.find( end_times_key, index );
	assert( index_times_string != text.npos ); // Tiene que encontrarla
	size_t lenght_times_string = index_times_string - index;
	std::string times_string = text.substr( index, lenght_times_string );
	index = index_times_string + end_times_key.size( ); // para saltar el end_times_key
	typedef std::vector< std::string > TTimeTexts;
	TTimeTexts times_texts_splited;
	stringSplit( times_string, " ", times_texts_splited );
	for( size_t i=0; i<times_texts_splited.size( ); ++i ) {
		float next_sheet_time = (float)atof( times_texts_splited[i].c_str( ) );
		next_sheet_times.push_back( next_sheet_time );
	}

	while( !exit ) {			
		// Nuevo sheet, meterle num_lines líneas en blanco
		TSheet sheet;
		for( size_t i=0; i<num_lines; ++i )
			sheet.push_back( "" );

		// Rellenar las num_lines líneas
		for( size_t i=0; i<num_lines; ++i ) {
			// Buscamos next_line_key
			size_t index_next_line = text.find( next_line_key, index );

			// Si no lo hemos encontrado significa que el resto del texto va en esta línea
			// y ya no seguimos, ya que seguro que no hay más texto
			if( index_next_line == text.npos ) {
				sheet[ i ] = text.substr( index );
				exit = true;
				i=num_lines; // Como break;
			}
			else {
				size_t lenght = index_next_line - index;
				sheet[ i ] = text.substr( index, lenght );
				index = index_next_line + next_line_key.size( ); // para saltar el next_line_key
			}
		}

		// Añadir el nuevo sheet
		sheets.push_back( sheet );
	}

	// Usar la primera sheet
	current_sheet = &sheets[0];
	if( next_sheet_times.size( ) != sheets.size( ) ) {
		warn( "El texto ocupa %d bocadillos de comic y hay %d tiempos en el xml!", sheets.size( ), next_sheet_times.size( ) );
		assert( !"Different size of sheets and next_sheet_times" );
		return;
	}
}


bool StaticText::nextSheet( ) {
	// Incrementar la sheet id
	++curr_sheet_id;

	if( curr_sheet_id >= sheets.size( ) ) {
		curr_sheet_id = sheets.size( ) - 1; // Quedarse en la última sheet
		return true;
	}
	else {
		current_sheet = &sheets[ curr_sheet_id ];
		return false;
	}
}

bool StaticText::update( float deltaTime ) {

	curr_time += deltaTime;

	// If exit button is pressed finish
	if( ioStatus.exitOptions.becomesPressed( ) ) {
		SoundPlayer::get( )->playSample( "dynamic_text_exit" );
		return true;
	}

	// Time to go to next sheet?
	if( goToNextSheet( ) ) {
		bool finished = nextSheet( );
		return finished;
	}

	return false;
}

bool StaticText::goToNextSheet( ) {
	assert( curr_sheet_id < next_sheet_times.size( ) );
	return curr_time >= next_sheet_times[ curr_sheet_id ];
}

const char *StaticText::getTextOnLine( size_t line ) const {
	assert( current_sheet );
	assert( line >= 0 && line < current_sheet->size( ) );
	return (*current_sheet)[ line ].c_str( );
}

void StaticText::stringSplit(std::string str, std::string delim, std::vector<std::string> &results)
{
	size_t cutAt;
	while( (cutAt = str.find_first_of(delim)) != str.npos ) {
		if(cutAt > 0) {
			results.push_back(str.substr(0,cutAt));
		}
		str = str.substr(cutAt+1);
	}
	if(str.length() > 0) {
		results.push_back(str);
	}
}
