#include <cassert>
#include <algorithm>
#include <ctime>
#include "ScoreRecorder.h"
#include "Global/Rendering.h"

void ScoreRecorder::ScoreInfo::getDate( int &day, int &month, int &year ) const {
	tm timeinfo;
	errno_t err = localtime_s( &timeinfo, &rawtime );
	assert( err == 0 );
  year  = timeinfo.tm_year + 1900;
  month = timeinfo.tm_mon + 1;
  day   = timeinfo.tm_mday;
}

// ---------------------------------------------------------------

ScoreRecorder::ScoreRecorder( )
: num_score_infos( 0 )
, score_infos_filename( "" )
{
	memset( score_infos, 0x00, sizeof( score_infos ) );
}

void ScoreRecorder::load( ) {
	reset( );
	score_infos_filename = "./data/scores.bin";
	loadScoreInfos( );
}

void ScoreRecorder::reset( ) {
	memset( score_infos, 0x00, sizeof( score_infos ) );
	num_score_infos = 0;
}

bool ScoreRecorder::isInsideTopScores( size_t score ) {
	// Si hay sitio...
	if( num_score_infos < MAX_SCORE_INFOS )
		return true;

	// Si no hay sitio, pero el ultimo tiene menos score, es que esta dentro
	return (score_infos[ num_score_infos - 1 ].score < score);
}

bool ScoreRecorder::addScore( const char *initials, size_t score ) {
	if( !isInsideTopScores( score ) )
		return false;

	// Si hay sitio, hay uno nuevo
	if( num_score_infos < MAX_SCORE_INFOS )
		++num_score_infos; // Incrementar

	// Usar el ultimo
	ScoreInfo &score_info = score_infos[ num_score_infos - 1 ];
	score_info.score   = score;
	time( &score_info.rawtime ); // Obtain raw time
	strncpy_s( score_info.initials, sizeof( score_info.initials ), initials, NUM_CHARS_PER_INITIALS );

	// Ordenar
	std::sort( &score_infos[0], &score_infos[ 0 ] + num_score_infos );

	return true;
}

void ScoreRecorder::loadScoreInfos( ) {
	reset( );

	FILE *f = NULL;
	errno_t err = fopen_s( &f, score_infos_filename.c_str( ), "rb" );
	if( err != 0 || !f )
		return;

	// Read header
	THeader header;
	fread( &header, sizeof( THeader ), 1, f );

	// Check header
	assert( strncmp( header.magic, "SCO", 4 ) == 0 );
	assert( header.version == THeader::CURRENT_VERSION );
	assert( header.bytes_per_score_info == sizeof( ScoreInfo ) );
	assert( header.num_score_infos <= MAX_SCORE_INFOS );

	// Read scores
	fread( score_infos, header.bytes_per_score_info, header.num_score_infos, f );
	num_score_infos = header.num_score_infos;

	fclose( f );
}

void ScoreRecorder::saveScoreInfos( ) {
	if( num_score_infos == 0 )
		return;

	printf("Writing score info file \"%s\"\n", score_infos_filename.c_str( ));

	FILE *f = NULL;
	errno_t err = fopen_s( &f, score_infos_filename.c_str( ), "wb" );
	if( err != 0 || !f ) {
		warn( "Can't write score info file \"%s\". Write protected?!\n", score_infos_filename.c_str( ) );
		return;
	}

	// Header
	THeader header;
	strncpy_s( header.magic, sizeof( header.magic ), "SCO", 4 );
	header.version              = THeader::CURRENT_VERSION;
	header.bytes_per_score_info = sizeof( ScoreInfo );
	header.num_score_infos      = num_score_infos;

	// Save header and scores
	fwrite( &header, sizeof( THeader ), 1, f );
	fwrite( score_infos, header.bytes_per_score_info, header.num_score_infos, f );

	fclose( f );
}

const ScoreRecorder::ScoreInfo & ScoreRecorder::getScoreInfo( size_t idx ) const {
	assert( idx < num_score_infos );
	return score_infos[ idx ];
}
