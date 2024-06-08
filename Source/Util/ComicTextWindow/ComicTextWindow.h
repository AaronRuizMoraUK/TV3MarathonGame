#ifndef COMIC_TEXT_WINDOW_H
#define COMIC_TEXT_WINDOW_H

#include "DinamicText.h"
#include "Util/Fade/TextureFade.h"
#include "Util/StringRender/AlphabetRender.h"
#include "Expat/XMLParser.h"

class ComicTextWindow : public CXMLParser {
public:
	enum StatusEnum {
		FADING_IN
		, WRITE_TEXT
		, FADING_OUT
	};

	CatalanAlphabetRender alphabet_renderer; // Necesario estar público para que pinten el continuar y exit

protected:
	static std::string pathToXml;

	TextureFade comic_window;
	float comic_window_alpha;

	static const float fade_time;
	float time;

	StatusEnum status;

	// Comic Text
	IComicText *comic_text;
	std::string comic_text_type;

	std::string style_name;
	
	// Print text variables
	float x;
	float y;
	float xl;
	float yl;
	float dist;
	size_t num_lines;
	float time_per_letter;

	// Comic window variables
	std::string comic_window_texture;
  float comic_window_scale_x;
	float comic_window_scale_y;
  float comic_window_pos_x;
  float comic_window_pos_y;

public:
	ComicTextWindow();
	~ComicTextWindow();

	void load( const std::string &astyle_name );

	void initiate( const std::string &text );
	bool update( float deltaTime ); // Return if it is finished
	void render( );

	void onStartElement(const std::string &elem, MKeyValue &atts);

	StatusEnum getStatus( ) const { return status; }

	inline const std::string &getComicTextType( ) const { return comic_text_type; }

private:
	void setComicWindowTransform( );
	void adjustText( );
	void adjustComicWindow( );
};

#endif //COMIC_TEXT_WINDOW_H
