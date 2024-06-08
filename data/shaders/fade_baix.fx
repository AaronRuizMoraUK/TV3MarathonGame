//--------------------//
//     VARIABLES      //
//--------------------//

struct VS_OUTPUT {
	float4 position : POSITION;
};

struct VS_OUTPUT_TEXTURE {
	float4 position : POSITION;
	float2 tex0 : TEXCOORD0;
};

// Create a texture for diffuse map
texture diffuse_texture;

// Create a sampler with sampler_state (reserved word) and set its Texture param for diffse map.
sampler diffuse_sampler = sampler_state
{
	Texture = (diffuse_texture); // Texture is a parameter of sampler
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
};

uniform float4x4 world_view_proj;

uniform float4 color_alpha = float4( 0.0f, 0.0f, 0.0f, 1.0f );

//-------------------------//
//     VERTEX SHADERS      //
//-------------------------//

VS_OUTPUT Fade_VS( float4 pPos: POSITION )
{
	VS_OUTPUT output;

	output.position = mul(pPos, world_view_proj);

	return output;
}

VS_OUTPUT_TEXTURE Texture_Fade_VS( float4 pPos: POSITION
	, float2 pTex0 : TEXCOORD0 )
{
	VS_OUTPUT_TEXTURE output;

	output.position = mul(pPos, world_view_proj);
	output.tex0 = pTex0;

	return output;
}

//-------------------------//
//      PIXEL SHADERS      //
//-------------------------//

float4 Fade_PS( ) : COLOR
{		
	return color_alpha;
}

float4 Texture_Fade_PS( float2 pTex0: TEXCOORD0 ) : COLOR
{
	// Obtain texture color from sampler
	float4 diffuse_tex = tex2D(diffuse_sampler, pTex0);
	diffuse_tex.w *= color_alpha.w;
	
	return diffuse_tex;
}

//-------------------------//
//       TECHNIQUES        //
//-------------------------//

technique fade_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Fade_VS();
		PixelShader = compile ps_1_1 Fade_PS();
	}
}

technique texture_fade_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Texture_Fade_VS();
		PixelShader = compile ps_1_1 Texture_Fade_PS();
	}
}
