#ifndef TEXTURE_SEQUENCER_H
#define TEXTURE_SEQUENCER_H

#include <vector>
#include "TextureFade.h"
#include "Expat/XMLParser.h"

class TextureSequencer : public CXMLParser
{
protected:
	struct Screen {
		TextureFade texture_fade;
		float       fade_time;
		float       duration;

		float       time;
		float       alpha;

		Screen( ) : fade_time( 0.0f ), duration( 0.0f ), time( 0.0f ), alpha( 0.0f ) {}

		void load( MKeyValue &atts );
		void reset( );

		bool update( float delta_time );
		void render( );

		void fadeOutNow( );
	};
	typedef std::vector< Screen > Screens;
	Screens screens;

	size_t idx_screen;

public:
	TextureSequencer();

	void load( const std::string &xmlFile );
	void onStartElement( const std::string &elem, MKeyValue &atts );

	void initiate( );
	void nextScreen( );

	bool update( float delta_time );
	void render( );
};

#endif //TEXTURE_SEQUENCER_H
