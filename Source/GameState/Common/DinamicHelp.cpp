#include <cassert>
#include "DinamicHelp.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"

std::string DinamicHelp::pathToXml = "";

const char *DinamicHelp::dinamic_help_descs[ NUM_DINAMIC_HELP_TEXTS ] = {
	"NONE"
	, "PLANET1_FIRST_MESSAGE"
	, "PLANET1_PLAYER_IS_HIT_BY_ALUCINACION"
	, "PLANET1_KILL_DOPAMINA"
	, "PLANET1_ENTER_RIGHT_DOP_LEVEL"
	, "PLANET1_EXIT_RIGHT_DOP_LEVEL"

	, "PLANET2_FIRST_MESSAGE"
	, "PLANET2_PLAYER_GETS_BOMB"
	, "PLANET2_PLAYER_LEAVES_BOMB"

	, "PLANET3_FIRST_MESSAGE"
	, "PLANET3_VALVE1_HIT"
	, "PLANET3_VALVE2_HIT"
	, "PLANET3_VALVE3_HIT"
	, "PLANET3_VALVE4_HIT"

	, "PLANET4_FIRST_MESSAGE"
	, "PLANET4_PLAYER_IS_HIT_BY_TWISTER"
	, "PLANET4_EXPENDER_HIT"
	, "PLANET4_ENTER_RIGHT_LEVELS"
	, "PLANET4_EXIT_RIGHT_LEVELS"
};

DinamicHelp::~DinamicHelp( ) {
	printDebugString( "Destroying DinamicHelp...\n" );
}

void DinamicHelp::load( ) {
	// Load texts and comic style
	if( pathToXml.empty( ) )
		pathToXml = Global::data_locator.getPath("xml");
	bool ok = xmlParseFile( pathToXml + "dinamic_help.xml" );
	assert( ok );
}

void DinamicHelp::onStartElement(const std::string &elem, MKeyValue &atts) {
	if( elem == "dinamic_help" ) {
		const std::string &id = atts["id"];
		const std::string &help_text = atts.getString( "text", "" );

		DinamicHelpDesc desc_id = getDinamicHelpDesc( id.c_str( ) );
		assert( desc_id != NONE );

		dinamic_help_texts[ desc_id ] = help_text;
	}
	else if( elem == "comic_text_window" ) {
		// Comic text window
		comic.load( atts[ "style" ] );
	}
}

void DinamicHelp::initiate( DinamicHelpDesc help ) {
	assert( help != NONE );

	// Get text of help desc
	DinamicHelpTexts::const_iterator it = dinamic_help_texts.find( help );
	assert( dinamic_help_texts.end( ) != it );
	comic.initiate( it->second );
}

DinamicHelp::DinamicHelpDesc DinamicHelp::getDinamicHelpDesc( const char *desc_text ) const {
	for( int i=0; i<NUM_DINAMIC_HELP_TEXTS; ++i ) {
		if( strcmp( dinamic_help_descs[i], desc_text ) == 0 )
			return (DinamicHelpDesc)i;
	}

	return NONE;
}
