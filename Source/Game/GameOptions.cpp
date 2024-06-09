#include <cassert>
#include "GameOptions.h"
#include "Global/Geometry.h"

Resolution::Resolution()
: width(800)
, height(600)
, rate(60)
, aspect_ratio(width/(float)height)
{
};

void Resolution::set( int pWidth, int pHeight, int pRate ) {
	width = pWidth;
	height = pHeight;
	rate = pRate;
	assert( width > 0 );
	assert( height > 0 );
	aspect_ratio = width/(float)height;
}

bool Resolution::operator==( const Resolution &othr ) const {
	if( width == othr.width &&
		height == othr.height &&
		rate == othr.rate )
		return true;
	else
		return false;
}

bool Resolution::operator!=( const Resolution &othr ) const {
	return !(*this == othr);
}

bool Resolution::operator<( const Resolution &othr ) const {
	if( width != othr.width )
		return width < othr.width;
	else if( height != othr.height )
		return height < othr.height;
	else if(rate != othr.rate )
		return rate < othr.rate;
	else
		return false;
}

// --------------------------------------------------------------

Options::Options()
: fullScreen(false)
, vSync(false)
, quality("alt")
, renderTextureEnabled( false )
, music(100)
, sfx(100)
{
};

bool Options::load( ) {
	bool ok = xmlParseFile("./data/config.xml");
	return ok;
}

void Options::onStartElement (const std::string &elem, MKeyValue &atts) {
	if( elem=="video" ) {
		fullScreen = atts.getBool("fullscreen", true);
		vSync      = atts.getBool("vsync", true);
		int width  = atts.getInt("width", 800);
		int height = atts.getInt("height", 600);
		int rate   = atts.getInt("rate", -1);
		resolution.set( width, height, rate );
	}
	else if( elem=="options" ) {
		quality = atts.getString("quality", "alt");
		if( quality != "baix" &&
			quality != "mitjà" &&
			quality != "alt" ) {
				quality = "alt";
		}
		
		//renderTextureEnabled = (quality == "alt");

		music = atts.getInt("music", 100);
		music = clamp( music, 0, 100 );
		sfx = atts.getInt("sfx", 100);
		sfx = clamp( sfx, 0, 100 );
	}
}

//bool Options::operator==( const Options &othr ) {
//	if( resolution == othr.resolution &&
//		quality == othr.quality &&
//		music == othr.music &&
//		sfx == othr.sfx )
//		return true;
//	else
//		return false;
//}
//
//bool Options::operator!=( const Options &othr ) {
//	return !(*this == othr);
//}
