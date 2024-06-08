#include "Logic.h"
#include "FirstLevelLogic.h"
#include "SecondLevelLogic.h"
#include "ThirdLevelLogic.h"
#include "FourthLevelLogic.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"

const size_t MAX_SCORE_VALUE_TO_RENDER = ((size_t)powf(10, MAX_SCORE_CYPHERS) - 1);
char PRINTF_SCORE_CHAR[8];

// ---------------------------------------------------------------------

// Logicas de los niveles
FirstLevelLogic  *first_level_logic  = NULL;
SecondLevelLogic *second_level_logic = NULL;
ThirdLevelLogic  *third_level_logic  = NULL;
FourthLevelLogic *fourth_level_logic = NULL;

FirstLevelLogic  &getFirstLevelLogic( )  { assert( first_level_logic );  return *first_level_logic; }
SecondLevelLogic &getSecondLevelLogic( ) { assert( second_level_logic ); return *second_level_logic; }
ThirdLevelLogic  &getThirdLevelLogic( )  { assert( third_level_logic );  return *third_level_logic; }
FourthLevelLogic &getFourthLevelLogic( ) { assert( fourth_level_logic ); return *fourth_level_logic; }

// ---------------------------------------------------------------------

// Clase solo para heredar de XMLParser y cargar lógica
class LogicLoader : public CXMLParser {
public:
	void onStartElement(const std::string &elem, MKeyValue &atts) {
		if( elem == "logic" ) {
			Logic::init_num_lifes = atts.getInt( "lifes", 5 );
		}
		else if( elem == "first_level_logic" ) {
			first_level_logic = new FirstLevelLogic( );
			((Logic *)first_level_logic)->loadLogic( atts );
		}
		else if( elem == "second_level_logic" ) {
			second_level_logic = new SecondLevelLogic( );
			((Logic *)second_level_logic)->loadLogic( atts );
		}
		else if( elem == "third_level_logic" ) {
			third_level_logic = new ThirdLevelLogic( );
			((Logic *)third_level_logic)->loadLogic( atts );
		}
		else if( elem == "fourth_level_logic" ) {
			fourth_level_logic = new FourthLevelLogic( );
			((Logic *)fourth_level_logic)->loadLogic( atts );
		}
	}
};
LogicLoader logic_loader;

// ---------------------------------------------------------------------

size_t Logic::init_num_lifes = 5;

size_t Logic::num_lifes = Logic::init_num_lifes;
size_t Logic::score = 0;
float Logic::player_sync = 250.0f;

Logic::~Logic() {
	printDebugString("Destroying Logic...\n");
}

void Logic::initGame() {
	num_lifes = init_num_lifes;
	score = 0;

	// Destroy all level logics
	getFirstLevelLogic( ).destroyLevel( );
	getSecondLevelLogic( ).destroyLevel( );
	getThirdLevelLogic( ).destroyLevel( );
	getFourthLevelLogic( ).destroyLevel( );
	
	// Reset all level logics dinamic help variables
	getFirstLevelLogic( ).resetDinamicHelp( );
	getSecondLevelLogic( ).resetDinamicHelp( );
	getThirdLevelLogic( ).resetDinamicHelp( );
	getFourthLevelLogic( ).resetDinamicHelp( );
}

void Logic::load( ) {
	std::string pathFilename = Global::data_locator.getPath( "xml" ) + "logic.xml";
	bool ok = logic_loader.xmlParseFile( pathFilename );	
	assert( ok );
	sprintf_s( PRINTF_SCORE_CHAR, sizeof( PRINTF_SCORE_CHAR ), "%%%dd", MAX_SCORE_CYPHERS );
}

void Logic::destroy( ) {
	// Destroy all level logics
	getFirstLevelLogic( ).destroyLevel( );
	getSecondLevelLogic( ).destroyLevel( );
	getThirdLevelLogic( ).destroyLevel( );
	getFourthLevelLogic( ).destroyLevel( );

	delete first_level_logic,  first_level_logic = NULL;
	delete second_level_logic, second_level_logic = NULL;
	delete third_level_logic,  third_level_logic = NULL;
	delete fourth_level_logic, fourth_level_logic = NULL;
}
