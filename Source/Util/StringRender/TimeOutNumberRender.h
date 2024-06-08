#ifndef TIMEOUT_NUMBER_RENDER_H
#define TIMEOUT_NUMBER_RENDER_H

#include "IStringRender.h"

/**
* This class make possible print numbers
* for the time out in planet gameplay
*/

class TimeOutNumberRender : public IStringRender
{
protected:
	const static D3DXVECTOR2 letterTexture;

private:
	std::string lastLetter;
	float show_time;
	float last_global_time;

public:
	TimeOutNumberRender( );
	~TimeOutNumberRender( );

	void print( float x, float y, float letter_width, float letter_height, const char *msg );
	void printfClosing( float x, float y, float letter_width, float letter_height, const char *msg, ... );
	void printClosing( float x, float y, float letter_width, float letter_height, const char *msg );

protected:
	const char * getTextureName() const { return "timeout_numbers.dds"; }
	const char * getTechnicName() const { return "timeout_tech"; }

private:
	bool obtainTextureCoodinatesOf( const char letter, D3DXVECTOR2 &outTextureCoordinates ) const;
};

#endif // TIMEOUT_NUMBER_RENDER_H
