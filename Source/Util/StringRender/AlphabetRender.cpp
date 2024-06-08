#include <cassert>
#include "AlphabetRender.h"
#include "Global/Rendering.h"
#include "Global/Geometry.h"
#include "Source/Game.h"
#include "Camera/ICamera.h"
#include "Mesh/IMesh.h"

IAlphabetRender::~IAlphabetRender( ) {
	printDebugString("Destroying IAlphabetRender...\n");
}

void IAlphabetRender::print( float x, float y, float letter_width, float letter_height, const char *atxt ) {
	assert( mesh && shader && texture );
	assert( letter_width>=0 && letter_height>=0 );

	if( strlen( atxt ) == 0 )
		return;

	errno_t err;
	char txt[512];
	err = strcpy_s( txt, sizeof( txt ), atxt );
	assert( err == 0 );
	err = _strlwr_s(txt, sizeof(txt)); // Lower-case
	assert( err == 0 );

	normalize( x, x, -1.0f, 1.0f, 0.0f, 1.0f );
	normalize( y, y, -1.0f, 1.0f, 0.0f, 1.0f );
	const D3DXVECTOR2 &letterTexture = getLetterTexture( );
	letter_height *= letterTexture.y/letterTexture.x * Game::configOptions.resolution.aspect_ratio; // For having the aspect of the letter
	
	// Calculate World View Projection matrix
	// We suppose that the orthogonal camera is activated
	const D3DXMATRIX &matView = getCurrentCamera( )->getMatView();
	const D3DXMATRIX &matProj = getCurrentCamera( )->getMatProj();
	D3DXMATRIX matViewProj;
	D3DXMatrixMultiply(&matViewProj, &matView, &matProj);

	D3DXHANDLE handleParam = NULL;
	EffectDX shader_fx = shader->fx;
	assert( shader_fx );

	// Set the width and the height of a letter in screen
	handleParam = shader_fx->GetParameterByName( NULL, "letter_screen_width" );
	if( handleParam )
		shader_fx->SetFloat( handleParam, letter_width );

	handleParam = shader_fx->GetParameterByName( NULL, "letter_screen_height" );
	if( handleParam )
		shader_fx->SetFloat( handleParam, letter_height );

	// Set the width and the height of a letter in the texture
	handleParam = shader_fx->GetParameterByName( NULL, "letter_texture" );
	if( handleParam )
		shader_fx->SetFloatArray( handleParam, letterTexture, 2 );

	// Activate diffuse texture
	handleParam = shader_fx->GetParameterByName( NULL, "diffuse_texture" );
	if( handleParam ) {
		assert( texture->tex );
		shader_fx->SetTexture( handleParam, texture->tex );
	}

	// Activate technique
	handleParam = shader_fx->GetTechniqueByName( getTechnicName( ) );
	if( handleParam ) {
		HRESULT hr = shader_fx->SetTechnique( handleParam );
		assert( hr == D3D_OK );
	}

	UINT cPasses, iPass;
	shader_fx->Begin( &cPasses, 0 ); // How many passes has the technique?
	for( iPass = 0; iPass < cPasses; ++iPass ) { // For each pass
		shader_fx->BeginPass( iPass );	// Begin pass

		// Print each letter in the text
		int next_line = 0;
		int next_letter = 0;
		for( size_t i=0; i<strlen( txt ); ++i ) {

			// Set texture coordinates according to the letter
			if( txt[i] == '\n' ) {
				next_line++;
				next_letter = 0;
				continue;
			}
			else {
				D3DXVECTOR2 textureCoordinates;
				bool found = obtainTextureCoodinatesOf( txt[i], textureCoordinates );
				if( !found )
					continue;
				handleParam = shader_fx->GetParameterByName( NULL, "texture_coords" );
				if( handleParam )
					shader_fx->SetFloatArray( handleParam, textureCoordinates, 2 );
			}

			// Go to the right to write the next letter
			transformMatrix(3,0) = x + letter_width*next_letter;
			transformMatrix(3,1) = y - letter_height*next_line;
			D3DXMATRIX matWorlViewProj;
			D3DXMatrixMultiply(&matWorlViewProj, &transformMatrix, &matViewProj);

			// Update matrix wvp of shader
			handleParam = shader_fx->GetParameterByName( NULL, "world_view_proj" );
			if( handleParam )
				shader_fx->SetMatrix( handleParam, &matWorlViewProj );

			// Only call CommitChanges if any state changes have happened
			// after BeginPass is called
			shader_fx->CommitChanges();

			// Do the real rendering of geometry
			mesh->renderStrip(0);

			++next_letter;
		}

		shader_fx->EndPass( );	// End Pass
	}
	shader_fx->End( );
}

// --------------------------------------------------------------------------------------------------
const D3DXVECTOR2 AlphabetRender::letterTexture(64.0f/512.0f, 64.0f/512.0f);

AlphabetRender::~AlphabetRender( ) {
	printDebugString("Destroying AlphabetRender...\n");
}

bool AlphabetRender::obtainTextureCoodinatesOf( const char letter, D3DXVECTOR2 &outTextureCoordinates ) const {
	
	if( letter == ' ' )
		outTextureCoordinates = D3DXVECTOR2(0.0f, 5.0f*letterTexture.y);
	else if( letter == '0' )
		outTextureCoordinates = D3DXVECTOR2(3.0f*letterTexture.x, 4.0f*letterTexture.y);
	else if( letter == '1' )
		outTextureCoordinates = D3DXVECTOR2(2.0f*letterTexture.x, 3.0f*letterTexture.y);
	else if( letter == '2' )
		outTextureCoordinates = D3DXVECTOR2(3.0f*letterTexture.x, 3.0f*letterTexture.y);
	else if( letter == '3' )
		outTextureCoordinates = D3DXVECTOR2(4.0f*letterTexture.x, 3.0f*letterTexture.y);
	else if( letter == '4' )
		outTextureCoordinates = D3DXVECTOR2(5.0f*letterTexture.x, 3.0f*letterTexture.y);
	else if( letter == '5' )
		outTextureCoordinates = D3DXVECTOR2(6.0f*letterTexture.x, 3.0f*letterTexture.y);
	else if( letter == '6' )
		outTextureCoordinates = D3DXVECTOR2(7.0f*letterTexture.x, 3.0f*letterTexture.y);
	else if( letter == '7' )
		outTextureCoordinates = D3DXVECTOR2(0.0f, 4.0f*letterTexture.y);
	else if( letter == '8' )
		outTextureCoordinates = D3DXVECTOR2(letterTexture.x, 4.0f*letterTexture.y);
	else if( letter == '9' )
		outTextureCoordinates = D3DXVECTOR2(2.0f*letterTexture.x, 4.0f*letterTexture.y);

	else if( letter == 'a' )
		outTextureCoordinates = D3DXVECTOR2(0.0f, 0.0f);
	else if( letter == 'b' )
		outTextureCoordinates = D3DXVECTOR2(letterTexture.x, 0.0f);
	else if( letter == 'c' )
		outTextureCoordinates = D3DXVECTOR2(2.0f*letterTexture.x, 0.0f);
	else if( letter == 'd' )
		outTextureCoordinates = D3DXVECTOR2(3.0f*letterTexture.x, 0.0f);
	else if( letter == 'e' )
		outTextureCoordinates = D3DXVECTOR2(4.0f*letterTexture.x, 0.0f);
	else if( letter == 'f' )
		outTextureCoordinates = D3DXVECTOR2(5.0f*letterTexture.x, 0.0f);
	else if( letter == 'g' )
		outTextureCoordinates = D3DXVECTOR2(6.0f*letterTexture.x, 0.0f);
	else if( letter == 'h' )
		outTextureCoordinates = D3DXVECTOR2(7.0f*letterTexture.x, 0.0f);
	else if( letter == 'i' )
		outTextureCoordinates = D3DXVECTOR2(0.0f, letterTexture.y);
	else if( letter == 'j' )
		outTextureCoordinates = D3DXVECTOR2(letterTexture.x, letterTexture.y);
	else if( letter == 'k' )
		outTextureCoordinates = D3DXVECTOR2(2.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'l' )
		outTextureCoordinates = D3DXVECTOR2(3.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'm' )
		outTextureCoordinates = D3DXVECTOR2(4.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'n' )
		outTextureCoordinates = D3DXVECTOR2(5.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'o' )
		outTextureCoordinates = D3DXVECTOR2(6.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'p' )
		outTextureCoordinates = D3DXVECTOR2(7.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'q' )
		outTextureCoordinates = D3DXVECTOR2(0.0f, 2.0f*letterTexture.y);
	else if( letter == 'r' )
		outTextureCoordinates = D3DXVECTOR2(letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == 's' )
		outTextureCoordinates = D3DXVECTOR2(2.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == 't' )
		outTextureCoordinates = D3DXVECTOR2(3.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == 'u' )
		outTextureCoordinates = D3DXVECTOR2(4.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == 'v' )
		outTextureCoordinates = D3DXVECTOR2(5.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == 'w' )
		outTextureCoordinates = D3DXVECTOR2(6.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == 'x' )
		outTextureCoordinates = D3DXVECTOR2(7.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == 'y' )
		outTextureCoordinates = D3DXVECTOR2(0.0f, 3.0f*letterTexture.y);
	else if( letter == 'z' )
		outTextureCoordinates = D3DXVECTOR2(letterTexture.x, 3.0f*letterTexture.y);

	else if( letter == '+' )
		outTextureCoordinates = D3DXVECTOR2(4.0f*letterTexture.x, 4.0f*letterTexture.y);
	else if( letter == '-' )
		outTextureCoordinates = D3DXVECTOR2(5.0f*letterTexture.x, 4.0f*letterTexture.y);
	else if( letter == '!' )
		outTextureCoordinates = D3DXVECTOR2(6.0f*letterTexture.x, 4.0f*letterTexture.y);
	else if( letter == '?' )
		outTextureCoordinates = D3DXVECTOR2(7.0f*letterTexture.x, 4.0f*letterTexture.y);
	else if( letter == ',' )
		outTextureCoordinates = D3DXVECTOR2(letterTexture.x, 5.0f*letterTexture.y);
	else if( letter == '.' )
		outTextureCoordinates = D3DXVECTOR2(2.0f*letterTexture.x, 5.0f*letterTexture.y);
	else if( letter == '(' )
		outTextureCoordinates = D3DXVECTOR2(3.0f*letterTexture.x, 5.0f*letterTexture.y);
	else if( letter == ')' )
		outTextureCoordinates = D3DXVECTOR2(4.0f*letterTexture.x, 5.0f*letterTexture.y);
	else if( letter == '=' )
		outTextureCoordinates = D3DXVECTOR2(5.0f*letterTexture.x, 5.0f*letterTexture.y);
	else if( letter == ':' )
		outTextureCoordinates = D3DXVECTOR2(6.0f*letterTexture.x, 5.0f*letterTexture.y);
	else if( letter == '%' )
		outTextureCoordinates = D3DXVECTOR2(7.0f*letterTexture.x, 5.0f*letterTexture.y);

	else if( letter == '[' )
		outTextureCoordinates = D3DXVECTOR2(0.0f, 6.0f*letterTexture.y); // CTRL
	else if( letter == ']' )
		outTextureCoordinates = D3DXVECTOR2(letterTexture.x, 6.0f*letterTexture.y); // SHIFT
	else if( letter == '{' )
		outTextureCoordinates = D3DXVECTOR2(2.0f*letterTexture.x, 6.0f*letterTexture.y); // ALT
	else if( letter == '}' )
		outTextureCoordinates = D3DXVECTOR2(3.0f*letterTexture.x, 6.0f*letterTexture.y); // SPACE
	else if( letter == '*' )
		outTextureCoordinates = D3DXVECTOR2(4.0f*letterTexture.x, 6.0f*letterTexture.y); // ENTER

	else
		return false;

	return true;
}

// --------------------------------------------------------------------------------------------------
const D3DXVECTOR2 CatalanAlphabetRender::letterTexture(32.0f/512.0f, 64.0f/512.0f);

CatalanAlphabetRender::~CatalanAlphabetRender( ) {
	printDebugString("Destroying CatalanAlphabetRender...\n");
}

bool CatalanAlphabetRender::obtainTextureCoodinatesOf( const char letter, D3DXVECTOR2 &outTextureCoordinates ) const {
	
	if( letter == 'a' )
		outTextureCoordinates = D3DXVECTOR2(0.0f, 0.0f);
	else if( letter == 'b' )
		outTextureCoordinates = D3DXVECTOR2(letterTexture.x, 0.0f);
	else if( letter == 'c' )
		outTextureCoordinates = D3DXVECTOR2(2.0f*letterTexture.x, 0.0f);
	else if( letter == 'd' )
		outTextureCoordinates = D3DXVECTOR2(3.0f*letterTexture.x, 0.0f);
	else if( letter == 'e' )
		outTextureCoordinates = D3DXVECTOR2(4.0f*letterTexture.x, 0.0f);
	else if( letter == 'f' )
		outTextureCoordinates = D3DXVECTOR2(5.0f*letterTexture.x, 0.0f);
	else if( letter == 'g' )
		outTextureCoordinates = D3DXVECTOR2(6.0f*letterTexture.x, 0.0f);
	else if( letter == 'h' )
		outTextureCoordinates = D3DXVECTOR2(7.0f*letterTexture.x, 0.0f);
	else if( letter == 'i' )
		outTextureCoordinates = D3DXVECTOR2(8.0f*letterTexture.x, 0.0f);
	else if( letter == 'j' )
		outTextureCoordinates = D3DXVECTOR2(9.0f*letterTexture.x, 0.0f);
	else if( letter == 'k' )
		outTextureCoordinates = D3DXVECTOR2(10.0f*letterTexture.x, 0.0f);
	else if( letter == 'l' )
		outTextureCoordinates = D3DXVECTOR2(11.0f*letterTexture.x, 0.0f);
	else if( letter == 'm' )
		outTextureCoordinates = D3DXVECTOR2(12.0f*letterTexture.x, 0.0f);
	else if( letter == 'n' )
		outTextureCoordinates = D3DXVECTOR2(13.0f*letterTexture.x, 0.0f);
	else if( letter == 'o' )
		outTextureCoordinates = D3DXVECTOR2(14.0f*letterTexture.x, 0.0f);
	else if( letter == 'p' )
		outTextureCoordinates = D3DXVECTOR2(15.0f*letterTexture.x, 0.0f);

	else if( letter == 'q' )
		outTextureCoordinates = D3DXVECTOR2(0.0f, letterTexture.y);
	else if( letter == 'r' )
		outTextureCoordinates = D3DXVECTOR2(letterTexture.x, letterTexture.y);
	else if( letter == 's' )
		outTextureCoordinates = D3DXVECTOR2(2.0f*letterTexture.x, letterTexture.y);
	else if( letter == 't' )
		outTextureCoordinates = D3DXVECTOR2(3.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'u' )
		outTextureCoordinates = D3DXVECTOR2(4.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'v' )
		outTextureCoordinates = D3DXVECTOR2(5.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'w' )
		outTextureCoordinates = D3DXVECTOR2(6.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'x' )
		outTextureCoordinates = D3DXVECTOR2(7.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'y' )
		outTextureCoordinates = D3DXVECTOR2(8.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'z' )
		outTextureCoordinates = D3DXVECTOR2(9.0f*letterTexture.x, letterTexture.y);

	else if( letter == ' ' )
		outTextureCoordinates = D3DXVECTOR2(10.0f*letterTexture.x, letterTexture.y);
	else if( letter == '1' )
		outTextureCoordinates = D3DXVECTOR2(11.0f*letterTexture.x, letterTexture.y);
	else if( letter == '2' )
		outTextureCoordinates = D3DXVECTOR2(12.0f*letterTexture.x, letterTexture.y);
	else if( letter == '3' )
		outTextureCoordinates = D3DXVECTOR2(13.0f*letterTexture.x, letterTexture.y);
	else if( letter == '4' )
		outTextureCoordinates = D3DXVECTOR2(14.0f*letterTexture.x, letterTexture.y);
	else if( letter == '5' )
		outTextureCoordinates = D3DXVECTOR2(15.0f*letterTexture.x, letterTexture.y);

	else if( letter == '6' )
		outTextureCoordinates = D3DXVECTOR2(0.0f, 2.0f*letterTexture.y);
	else if( letter == '7' )
		outTextureCoordinates = D3DXVECTOR2(letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == '8' )
		outTextureCoordinates = D3DXVECTOR2(2.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == '9' )
		outTextureCoordinates = D3DXVECTOR2(3.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == '0' )
		outTextureCoordinates = D3DXVECTOR2(4.0f*letterTexture.x, 2.0f*letterTexture.y);
	
	else if( letter == '=' )
		outTextureCoordinates = D3DXVECTOR2(5.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == '+' )
		outTextureCoordinates = D3DXVECTOR2(6.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == '-' )
		outTextureCoordinates = D3DXVECTOR2(7.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == '!' )
		outTextureCoordinates = D3DXVECTOR2(8.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == '?' )
		outTextureCoordinates = D3DXVECTOR2(9.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == ',' )
		outTextureCoordinates = D3DXVECTOR2(10.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == '.' )
		outTextureCoordinates = D3DXVECTOR2(11.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == ':' )
		outTextureCoordinates = D3DXVECTOR2(12.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == '(' )
		outTextureCoordinates = D3DXVECTOR2(13.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == ')' )
		outTextureCoordinates = D3DXVECTOR2(14.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == '%' )
		outTextureCoordinates = D3DXVECTOR2(15.0f*letterTexture.x, 2.0f*letterTexture.y);

	// нтуъзаий'
	else if( letter == 'н' )
		outTextureCoordinates = D3DXVECTOR2(letterTexture.x, 3.0f*letterTexture.y); 
	else if( letter == 'т' )
		outTextureCoordinates = D3DXVECTOR2(2.0f*letterTexture.x, 3.0f*letterTexture.y); 
	else if( letter == 'у' )
		outTextureCoordinates = D3DXVECTOR2(3.0f*letterTexture.x, 3.0f*letterTexture.y); 
	else if( letter == 'ъ' )
		outTextureCoordinates = D3DXVECTOR2(4.0f*letterTexture.x, 3.0f*letterTexture.y); 
	else if( letter == 'з' )
		outTextureCoordinates = D3DXVECTOR2(5.0f*letterTexture.x, 3.0f*letterTexture.y); 
	else if( letter == 'а' )
		outTextureCoordinates = D3DXVECTOR2(6.0f*letterTexture.x, 3.0f*letterTexture.y); 
	else if( letter == 'и' )
		outTextureCoordinates = D3DXVECTOR2(7.0f*letterTexture.x, 3.0f*letterTexture.y); 
	else if( letter == 'й' )
		outTextureCoordinates = D3DXVECTOR2(8.0f*letterTexture.x, 3.0f*letterTexture.y); 
	else if( letter == '\'' )
		outTextureCoordinates = D3DXVECTOR2(9.0f*letterTexture.x, 3.0f*letterTexture.y);

	else if( letter == '[' )
		outTextureCoordinates = D3DXVECTOR2(0.0f, 4.0f*letterTexture.y); // CTRL
	else if( letter == ']' )
		outTextureCoordinates = D3DXVECTOR2(2.0f*letterTexture.x, 4.0f*letterTexture.y); // SHIFT
	else if( letter == '{' )
		outTextureCoordinates = D3DXVECTOR2(4.0f*letterTexture.x, 4.0f*letterTexture.y); // ALT
	else if( letter == '}' )
		outTextureCoordinates = D3DXVECTOR2(6.0f*letterTexture.x, 4.0f*letterTexture.y); // SPACE
	else if( letter == '*' )
		outTextureCoordinates = D3DXVECTOR2(8.0f*letterTexture.x, 4.0f*letterTexture.y); // ENTER

	else
		return false;

	return true;
}
