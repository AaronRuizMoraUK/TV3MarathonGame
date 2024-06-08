#ifndef DINAMIC_HELP_H
#define DINAMIC_HELP_H

#include "Expat/XMLParser.h"
#include "Util/ComicTextWindow/ComicTextWindow.h"

class DinamicHelp : public CXMLParser {

public:
	enum DinamicHelpDesc {
		NONE = 0

		, PLANET1_FIRST_MESSAGE
		, PLANET1_PLAYER_IS_HIT_BY_ALUCINACION
		, PLANET1_KILL_DOPAMINA
		, PLANET1_ENTER_RIGHT_DOP_LEVEL
		, PLANET1_EXIT_RIGHT_DOP_LEVEL

		, PLANET2_FIRST_MESSAGE
		, PLANET2_PLAYER_GETS_BOMB
		, PLANET2_PLAYER_LEAVES_BOMB

		, PLANET3_FIRST_MESSAGE
		, PLANET3_VALVE1_HIT
		, PLANET3_VALVE2_HIT
		, PLANET3_VALVE3_HIT
		, PLANET3_VALVE4_HIT

		, PLANET4_FIRST_MESSAGE
		, PLANET4_PLAYER_IS_HIT_BY_TWISTER
		, PLANET4_EXPENDER_HIT
		, PLANET4_ENTER_RIGHT_LEVELS
		, PLANET4_EXIT_RIGHT_LEVELS

		, NUM_DINAMIC_HELP_TEXTS
	};

protected:
	static std::string pathToXml;

	// Dinamic help texts
	ComicTextWindow comic;

	static const char *dinamic_help_descs[ NUM_DINAMIC_HELP_TEXTS ];

	typedef std::map<DinamicHelpDesc, std::string> DinamicHelpTexts;
	DinamicHelpTexts dinamic_help_texts;

public:
	~DinamicHelp( );

	void load( );
	void onStartElement(const std::string &elem, MKeyValue &atts);

	void initiate( DinamicHelpDesc help );

	bool update( float deltaTime ) { return comic.update( deltaTime ); }
	void render( ) { comic.render( ); }

	DinamicHelpDesc getDinamicHelpDesc( const char *desc_text ) const;

};

#endif //DINAMIC_HELP_H
