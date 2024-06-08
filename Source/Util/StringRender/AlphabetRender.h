#ifndef ALPHABET_RENDER_H
#define ALPHABET_RENDER_H

#include "IStringRender.h"

/**
* This class make possible print with a
* a texture with alphabet
*/

class IAlphabetRender : public IStringRender
{
public:
	virtual ~IAlphabetRender( );

	void print( float x, float y, float letter_width, float letter_height, const char *msg );

protected:
	const char *getTechnicName() const { return "basic_tech"; }

	virtual const D3DXVECTOR2 &getLetterTexture( ) const = 0;

	virtual bool obtainTextureCoodinatesOf( const char letter, D3DXVECTOR2 &outTextureCoordinates ) const = 0;
};

// ----------------------------------------------------------------
class AlphabetRender : public IAlphabetRender
{
protected:
	const static D3DXVECTOR2 letterTexture;

public:
	~AlphabetRender( );

protected:
	const char *getTextureName() const { return "alphabet.dds"; }

	const D3DXVECTOR2 &getLetterTexture( ) const { return letterTexture; }

	bool obtainTextureCoodinatesOf( const char letter, D3DXVECTOR2 &outTextureCoordinates ) const;
};

// ----------------------------------------------------------------
class CatalanAlphabetRender : public IAlphabetRender
{
protected:
	const static D3DXVECTOR2 letterTexture;

public:
	virtual ~CatalanAlphabetRender( );

protected:
	virtual const char *getTextureName() const { return "catalan_alphabet.dds"; }

	const D3DXVECTOR2 &getLetterTexture( ) const { return letterTexture; }

	bool obtainTextureCoodinatesOf( const char letter, D3DXVECTOR2 &outTextureCoordinates ) const;
};

// ----------------------------------------------------------------
class CatalanAlphabetColorRender : public CatalanAlphabetRender
{
protected:
	const char *getTextureName() const { return "catalan_alphabet_color.dds"; }
};

#endif // ALPHABET_RENDER_H
