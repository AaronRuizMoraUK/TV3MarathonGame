#ifndef FADE_H
#define FADE_H

#include "Directx/Directx.h"

class IMesh;

/**
* Class to make fades out, int, mediums out, mediums int, etc.
* It is made to work with an orthogonal camera.
*/

class Fade
{
protected:
	// Variable to control alpha value
	// alpha = 1.0f -> The Black Quad is shown, so the screen is black
	// alpha = 0.0f -> The Black Quad is not shown, so the screen shows the render
	float alpha;

	const IMesh *mesh;

	const Effect *shader;
	static const char *shaderName;
	static const char *techniqueName;
	
	float duration;
	float time;

	bool initedFadeIn;
	bool initedFadeOut;
	bool initedMediumFadeOut;

	float initAlphaFadeIn;
	float initAlphaFadeOut;

public:
	Fade();
	~Fade();
	
	void load( );
	bool isFinished( ) const { return (time >= duration); }

	void update( float deltaTime );

	void renderFadeIn( float pDuration, const D3DXVECTOR3 &color=D3DXVECTOR3(0,0,0) );
	void renderFadeOut( float pDuration, const D3DXVECTOR3 &color=D3DXVECTOR3(0,0,0) );
	void renderMediumFadeOut( float pDuration, const D3DXVECTOR3 &color=D3DXVECTOR3(0,0,0) );

private:
	void initVariables( );

	void render( const D3DXVECTOR3 &color ) const;
};

#endif //FADE_H
