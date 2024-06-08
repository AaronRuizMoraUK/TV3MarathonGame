#ifndef OPTIONS_H
#define OPTIONS_H

#include <set>
#include "Expat/XMLParser.h"

struct Resolution {
	int   width;
	int   height;
	int   rate;
	float aspect_ratio;

	Resolution();

	void set( int pWidth, int pHeight, int pRate=-1 );

	bool operator==( const Resolution &othr ) const;
	bool operator!=( const Resolution &othr ) const;
	bool operator<( const Resolution &othr ) const;

};

typedef std::set<Resolution> Resolutions;

struct Options : public CXMLParser {
	bool fullScreen;
	bool vSync;
	Resolution resolution;

	std::string quality;
	bool        renderTextureEnabled;

	int music;
	int sfx;

	Options();
	
	bool load( );
	void onStartElement(const std::string &elem, MKeyValue &atts);

	//bool operator==( const Options &othr );
	//bool operator!=( const Options &othr );
};

#endif // OPTIONS_H
