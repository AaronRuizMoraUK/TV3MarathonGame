#ifndef XBOX_BUTTONS_RENDER_H
#define XBOX_BUTTONS_RENDER_H

#include "IStringRender.h"

/**
* This class make possible print XBOX 360 Buttons
* for the Options in main menu
*/

class XBOXButtonsRender : public IStringRender
{
protected:
	const static D3DXVECTOR2 letterTexture;

public:
	~XBOXButtonsRender( );

	void print( float x, float y, float letter_width, float letter_height, const char *msg );

protected:
	const char * getTextureName() const { return "xbox_buttons.dds"; }
	const char * getTechnicName() const { return "basic_tech"; }

private:
	bool obtainTextureCoodinatesOf( const char letter, D3DXVECTOR2 &outTextureCoordinates ) const;
};

#endif // XBOX_BUTTONS_RENDER_H
