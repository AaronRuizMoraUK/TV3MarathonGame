//--------------------//
//     VARIABLES      //
//--------------------//

struct VS_OUTPUT {
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

uniform float2 texture_coords = float2( 0.0f, 0.0f );
uniform float2 letter_texture = float2( 0.0f, 0.0f );
uniform float letter_screen_width = 0.0f;
uniform float letter_screen_height = 0.0f;
uniform float time = 0.0f;

//-------------------------//
//     VERTEX SHADERS      //
//-------------------------//

VS_OUTPUT Render_VS( float4 pPos: POSITION
	, float2 pTex0 : TEXCOORD0 )
{
	VS_OUTPUT output;
	
	pPos.x *= letter_screen_width;
	pPos.y *= letter_screen_height;
	
	// pTex0 is between 0.0 and 1.0
	float2 text_coords = ( ( texture_coords+letter_texture ) - texture_coords ) * pTex0 + texture_coords;

	output.position = mul(pPos, world_view_proj);
	output.tex0 = text_coords;

	return output;
}

//-------------------------//
//      PIXEL SHADERS      //
//-------------------------//

float4 Render_PS( float2 pTex0: TEXCOORD0 ) : COLOR
{
	// Obtain texture color from sampler
	float4 diffuse_tex = tex2D(diffuse_sampler, pTex0);

	return diffuse_tex;
}

float4 Render_Timeout_PS( float2 pTex0: TEXCOORD0 ) : COLOR
{
	// Obtain texture color from sampler
	float4 diffuse_tex = tex2D(diffuse_sampler, pTex0);
	//diffuse_tex.y += 0.5f*sin( 10.0f*time - 7.0f*pTex0.y );
	diffuse_tex.w *= 0.7f;

	return diffuse_tex;
}

//-------------------------//
//       TECHNIQUES        //
//-------------------------//

technique basic_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Render_VS();
		PixelShader = compile ps_1_1 Render_PS();
	}
}

technique timeout_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Render_VS();
		PixelShader = compile ps_1_1 Render_Timeout_PS();
	}
}
