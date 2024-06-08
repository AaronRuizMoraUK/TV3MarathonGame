#ifndef SCORE_RECORDER_H
#define SCORE_RECORDER_H

#include <string>

#define MAX_SCORE_INFOS 10
#define NUM_CHARS_PER_INITIALS 4 // 3 + '\0' 

class ScoreRecorder {
public:
	struct ScoreInfo {
		char   initials[ NUM_CHARS_PER_INITIALS ];
		size_t score;
		time_t rawtime; // Seconds since January 1, 1970 

		ScoreInfo( ) : score( 0 ), rawtime( 0 ) { initials[ 0 ] = 0x00; }
		bool operator<(const ScoreInfo &other) const { return ( score > other.score ); } // Para que se ordene de mayor a menor

		void getDate( int &day, int &month, int &year ) const;
	};

protected:
	struct THeader {
		static const size_t CURRENT_VERSION = 2;
		char   magic[ 4 ];
		size_t version;
		size_t bytes_per_score_info;
		size_t num_score_infos;
	};

	ScoreInfo score_infos[ MAX_SCORE_INFOS ];
	size_t    num_score_infos;

	std::string score_infos_filename;

public:
	ScoreRecorder( );

	void load( );

	void loadScoreInfos( );
	void saveScoreInfos( );

	bool isInsideTopScores( size_t score );
	bool addScore( const char *initials, size_t score );

	size_t getNumScoreInfos( ) const { return num_score_infos; }
	const ScoreInfo &getScoreInfo( size_t idx ) const;

protected:
	void reset( );
};

#endif // SCORE_RECORDER_H
