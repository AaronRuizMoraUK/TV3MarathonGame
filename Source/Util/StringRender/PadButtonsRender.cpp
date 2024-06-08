#include <cassert>
#include "PadButtonsRender.h"
#include "Global/Rendering.h"
#include "Global/Geometry.h"
#include "Game/Game.h"
#include "Camera/ICamera.h"
#include "Mesh/IMesh.h"

const D3DXVECTOR2 PadButtonsRender::letterTexture(64.0f/256.0f, 64.0f/256.0f);

PadButtonsRender::~PadButtonsRender( ) {
	printDebugString("Destroying PadButtonsRender...\n");
}

void PadButtonsRender::print( float x, float y, float letter_width, float letter_height, const char *txt ) {
	assert( mesh && shader && texture );
	assert( letter_width>=0 && letter_height>=0 );

	size_t txtLen = strlen( txt );
	if( txtLen == 0 )
		return;
	else if( txtLen > 2 ) {
		assert( !"Wrong print in PadButtonsRender" );
		return;
	}

	D3DXHANDLE handleParam = NULL;
	EffectDX shader_fx = shader->fx;
	assert( shader_fx );

	// Set texture coordinates according to the letter
	int number = atoi(txt);
	D3DXVECTOR2 textureCoordinates;
	bool found = obtainTextureCoodinatesOf( number, textureCoordinates );
	if( !found )
		return;
	handleParam = shader_fx->GetParameterByName( NULL, "texture_coords" );
	if( handleParam )
		shader_fx->SetFloatArray( handleParam, textureCoordinates, 2 );

	normalize( x, x, -1.0f, 1.0f, 0.0f, 1.0f );
	normalize( y, y, -1.0f, 1.0f, 0.0f, 1.0f );
	letter_height *= letterTexture.y/letterTexture.x * Game::configOptions.resolution.aspect_ratio; // For having the aspect of the letter

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

	// Calculate World View Projection matrix
	// We suppose that the orthogonal camera is activated
	const D3DXMATRIX &matView = getCurrentCamera( )->getMatView();
	const D3DXMATRIX &matProj = getCurrentCamera( )->getMatProj();
	D3DXMATRIX matViewProj;
	D3DXMatrixMultiply(&matViewProj, &matView, &matProj);

	// Go to the right to write the next letter
	transformMatrix(3,0) = x;
	transformMatrix(3,1) = y;
	D3DXMATRIX matWorlViewProj;
	D3DXMatrixMultiply(&matWorlViewProj, &transformMatrix, &matViewProj);

	// Update matrix wvp of shader
	handleParam = shader_fx->GetParameterByName( NULL, "world_view_proj" );
	if( handleParam )
		shader_fx->SetMatrix( handleParam, &matWorlViewProj );

	// Activate diffuse texture
	handleParam = shader_fx->GetParameterByName( NULL, "diffuse_texture" );
	if( handleParam )  {
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

		// Do the real rendering of geometry
		mesh->renderStrip(0);

		shader_fx->EndPass( );	// End Pass
	}
	shader_fx->End( );
}

bool PadButtonsRender::obtainTextureCoodinatesOf( int number, D3DXVECTOR2 &outTextureCoordinates ) const {
	
	if( number > 15 || number < 0 )
		return false;

	int row = number/4;
	int column = number%4;
	outTextureCoordinates = D3DXVECTOR2( column*letterTexture.x, row*letterTexture.y );

	return true;
}
