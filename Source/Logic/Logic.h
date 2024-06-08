#ifndef LOGIC_H
#define LOGIC_H

#define MAX_SCORE_CYPHERS 6
extern const size_t MAX_SCORE_VALUE_TO_RENDER;
extern char PRINTF_SCORE_CHAR[];

class MKeyValue;
class FirstLevelLogic;
class SecondLevelLogic;
class ThirdLevelLogic;
class FourthLevelLogic;

class Logic {
protected:
	// XML
	static size_t init_num_lifes;

	// Number of lifes and score
	static size_t num_lifes;
	static size_t score;
	static float player_sync; // Es la vida -> [0,init_player_sync(de cada level)]

public:
	virtual ~Logic();

	static void load( );
	static void destroy( );

	// -----------------------------------
	// Game function
	static void initGame();
	inline static bool isGameOver( ) { return (num_lifes==0); }

	// Life functions
	inline static void lifeUp( ) { ++num_lifes; }
	inline static void lifeDown( ) { if( num_lifes>0 ) --num_lifes; }
	inline static size_t getNumLifes( ) { return num_lifes; }

	// Score functions
	inline static void scoreUp( size_t points ) { score += points; }
	inline static size_t getScore( ) { return score; }
	
	// Sync fuctions
	inline static float getPlayerSync( ) { return player_sync; }
	inline static void playerSyncDown( float points ) { player_sync -= points; if( player_sync < 0.0f ) player_sync = 0.0f; }
	inline static void playerSyncUp( float points ) { player_sync += points; if( player_sync > 999.0f ) player_sync = 999.0f; }

	// -----------------------------------
	// Level functions
	virtual void initLevel() = 0;
	virtual void destroyLevel( ) = 0;
	virtual void resetDinamicHelp( ) = 0;

	virtual void update( float deltaTime ) = 0;

	virtual bool isLevelWon() const = 0;
	virtual bool isLevelLost() const = 0;

protected:
	virtual void loadLogic( MKeyValue &atts ) = 0;

	friend class LogicLoader; // Clase amiga para que pueda cargar la logica (acceder a cosas protected)
};

// ----------------------------------------------------------

FirstLevelLogic  &getFirstLevelLogic( );
SecondLevelLogic &getSecondLevelLogic( );
ThirdLevelLogic  &getThirdLevelLogic( );
FourthLevelLogic &getFourthLevelLogic( );

#endif //LOGIC_H
