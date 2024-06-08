#ifndef UI_NUMBER_RENDER_H
#define UI_NUMBER_RENDER_H

#include "IStringRender.h"

/**
* This class make possible print numbers
* for the User Interface in planet gamepley
*/

class UINumberRender : public IStringRender
{
protected:
	const static D3DXVECTOR2 letterTexture;

public:
	~UINumberRender( );

	void print( float x, float y, float letter_width, float letter_height, const char *msg );

protected:
	const char * getTextureName() const { return "ui_numbers.dds"; }
	const char * getTechnicName() const { return "basic_tech"; }

private:
	bool obtainTextureCoodinatesOf( const char letter, D3DXVECTOR2 &outTextureCoordinates ) const;
};

#endif // UI_NUMBER_RENDER_H
