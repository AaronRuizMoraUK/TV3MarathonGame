#include <cassert>
#include "Fade.h"
#include "Global/Rendering.h"
#include "Factories/QuadFactory.h"
#include "Factories/EffectFactory.h"
#include "Global/Geometry.h"
#include "Camera/ICamera.h"
#include "Mesh/IMesh.h"

const char * Fade::shaderName = "fade.fx";
const char * Fade::techniqueName = "fade_tech";

Fade::Fade()
: alpha( -1.0f )
, mesh( NULL )
, shader( NULL )
, duration( 0.0f )
, time( 0.0f )
, initedFadeIn( false )
, initedFadeOut( false )
, initedMediumFadeOut( false )
, initAlphaFadeIn( 0.0f )
, initAlphaFadeOut( 0.0f )
{
}

Fade::~Fade() {
	printDebugString("Destroying Fade...\n");

	// Object does not destroy its Mesh, MeshFactory does!!

	// EffectFactory is who destrois the effect (shader)!!
};

void Fade::load( ) {
	initVariables( );

	mesh = QuadFactory::get()->createQuad(1.0f, 1.0f);
	assert(mesh);

	shader = EffectFactory::get()->createEffect(shaderName);
	assert(shader);
}

void Fade::initVariables( ) {
	alpha = -1.0f;
	duration = 0.0f;
	time = 0.0f;
	initedFadeIn = false;
	initedFadeOut = false;
	initedMediumFadeOut = false;
	initAlphaFadeIn = 0.0f;
	initAlphaFadeOut = 0.0f;
}

void Fade::update( float deltaTime ) { 
	time += deltaTime;
	if( time>duration )
		time = duration;
}

void Fade::renderFadeIn( float pDuration, const D3DXVECTOR3 &color ) {
	if( !initedFadeIn ) {
		initedFadeIn = true;
		initedFadeOut = initedMediumFadeOut = false;

		time = 0.0f;
		duration = pDuration;

		if( alpha == 0.0f ) {
			time = duration;
			return;
		}
		if( alpha == -1.0f )
			alpha = 1.0f;

		initAlphaFadeIn = alpha;
	}

	// Update alpha from initAlphaFadeIn to 0 in duration seconds
	assert( duration!=0.0f );
	alpha = clamp( initAlphaFadeIn * ((duration-time)/duration), 0.0f, 1.0f );

	if( alpha > 0.0f )
		render( color );
}

void Fade::renderFadeOut( float pDuration, const D3DXVECTOR3 &color ) {
	if( !initedFadeOut ) {
		initedFadeOut = true;
		initedFadeIn = initedMediumFadeOut = false;

		time = 0.0f;
		duration = pDuration;

		if( alpha == 1.0f ) {
			time = duration;
			return;
		}
		else if( alpha == -1.0f )
			alpha = 0.0f;

		initAlphaFadeOut = alpha;
	}

	// Update alpha from initAlphaFadeOut to 1 in duration seconds
	assert( duration!=0.0f );
	alpha = clamp( initAlphaFadeOut + (1.0f-initAlphaFadeOut) * (time/duration), 0.0f, 1.0f );

	if( alpha > 0.0f )
		render( color );
}

void Fade::renderMediumFadeOut( float pDuration, const D3DXVECTOR3 &color ) {
	if( !initedMediumFadeOut ) {
		initedMediumFadeOut = true;
		initedFadeIn = initedFadeOut = false;

		time = 0.0f;
		duration = pDuration;

		if( alpha == -1.0f )
			alpha = 0.0f;

		assert( alpha == 0.0f );
	}

	// Update alpha from 0 to 0.5 in duration seconds
	assert( duration!=0.0f );
	alpha = clamp( 0.5f * (time/duration), 0.0f, 1.0f );

	if( alpha > 0.0f )
		render( color );
}

void Fade::render( const D3DXVECTOR3 &color ) const {
	assert( mesh && shader );

	// Calculate World View Projection matrix
	// We suppose that the orthogonal camera is activated
	ICamera *camera = getCurrentCamera( );
	const D3DXMATRIX &matView = camera->getMatView();
	const D3DXMATRIX &matProj = camera->getMatProj();
	D3DXMATRIX matWorldViewProj;
	D3DXMatrixMultiply(&matWorldViewProj, &matView, &matProj);

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
		D3DXVECTOR4 colorAlpha( color, alpha);
		shader_fx->SetVector( handleParam, &colorAlpha);
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
