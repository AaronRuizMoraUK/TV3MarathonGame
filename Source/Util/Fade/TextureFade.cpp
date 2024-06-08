#include <cassert>
#include "TextureFade.h"
#include "Global/Rendering.h"
#include "Factories/QuadFactory.h"
#include "Factories/EffectFactory.h"
#include "Factories/TextureFactory.h"
#include "Source/Game.h"
#include "Camera/ICamera.h"
#include "Mesh/IMesh.h"

const char * TextureFade::shaderName = "fade.fx";
const char * TextureFade::techniqueName = "texture_fade_tech";

TextureFade::TextureFade()
: mesh( NULL )
, shader( NULL )
, texture( NULL )
{
	D3DXMatrixIdentity(&transform);
}

TextureFade::~TextureFade() {
	printDebugString("Destroying TextureFade...\n");

	// Object does not destroy its Mesh, MeshFactory does!!

	// EffectFactory is who destrois the effect (shader)!!

	// TextureFactory is who destrois the texture!!
};

void TextureFade::load( const std::string &textureName ) {
	mesh = QuadFactory::get()->createQuad(1.0f / Game::configOptions.resolution.aspect_ratio, 1.0f);
	assert( mesh );

	shader = EffectFactory::get()->createEffect(shaderName);
	assert( shader );

	texture = TextureFactory::get()->createTexture( textureName );
	assert( texture );
}

void TextureFade::render( float alpha ) {
	assert( mesh && shader && texture );

	// Calculate World View Projection matrix
	// We suppose that the orthogonal camera is activated
	ICamera *camera = getCurrentCamera( );
	const D3DXMATRIX &matView = camera->getMatView();
	const D3DXMATRIX &matProj = camera->getMatProj();
	D3DXMATRIX matWorldViewProj;
	D3DXMatrixMultiply(&matWorldViewProj, &matView, &matProj);
	D3DXMatrixMultiply(&matWorldViewProj, &transform, &matWorldViewProj);
	
	D3DXHANDLE handleParam = NULL;
	EffectDX shader_fx = shader->fx;
	assert( shader_fx );

	// Update matrix wvp of shader
	handleParam = shader_fx->GetParameterByName( NULL, "world_view_proj" );
	if( handleParam )
		shader_fx->SetMatrix( handleParam, &matWorldViewProj );

	// Set the color and alpha value
	handleParam = shader_fx->GetParameterByName( NULL, "color_alpha" );
	if( handleParam ) {
		D3DXVECTOR4 colorAlpha( D3DXVECTOR3(0,0,0), alpha);
		shader_fx->SetVector( handleParam, &colorAlpha);
	}

	// Activate diffuse texture
	handleParam = shader_fx->GetParameterByName( NULL, "diffuse_texture" );
	if( handleParam ) {
		assert( texture->tex );
		shader_fx->SetTexture( handleParam, texture->tex );
	}

	// Activate technique
	handleParam = shader_fx->GetTechniqueByName( techniqueName );
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
