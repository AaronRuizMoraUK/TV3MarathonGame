//--------------------//
//     VARIABLES      //
//--------------------//

struct VS_OUTPUT_DUMMY {
	float4 position : POSITION;
	float2 tex0 : TEXCOORD0;
};

struct VS_OUTPUT_PARTICLE {
	float4 position : POSITION;
	float2 tex0 : TEXCOORD0;
	float4 color : TEXCOORD1;
};

struct VS_OUTPUT_TEXTURE {
	float4 pos : POSITION;
	float4 tex0 : TEXCOORD0;
};

uniform float4x4 world;
uniform float4x4 world_view_proj;

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

// Create a texture for normal map
texture normal_texture;

// Create a sampler with sampler_state (reserved word) and set its Texture param for normal map.
sampler normal_sampler = sampler_state
{ 
	Texture = (normal_texture);
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
};

// DEBUG //
uniform float4 status_color = float4(0.0f,0.0f,0.0f,1.0f);
// ----- //

uniform float3 camera_pos;
uniform float3 light_pos = float3(0.0f,40.0f,80.0f);
uniform float4 light_color = float4(1.0f,0.9f,0.8f,1.0f);
uniform float ambient_color = 0.4f;
uniform float specular_alfa = 0.25f;
uniform float specular_factor = 20.0f;

uniform float time = 0.0f;
uniform float texture_speed = 0.0f;
uniform float amplitudeMovQuad = 0.02f;

#define MAX_BONES	40
#define MAX_PARTICLES	100
uniform float3x4 Bones[MAX_BONES];
uniform float4 particle_data[MAX_PARTICLES];
uniform float3 camera_up;
uniform float3 camera_left;

//-------------------------//
//     VERTEX SHADERS      //
//-------------------------//

VS_OUTPUT_DUMMY Dummy_VS( float4 pPos: POSITION
	, float2 pTex0 : TEXCOORD0)
{
	VS_OUTPUT_DUMMY output;

	output.position = mul(pPos, world_view_proj);
	output.tex0 = pTex0;

	return output;
}


VS_OUTPUT_PARTICLE Render_Scene_Particle_VS( float4 pPos: POSITION )
{
	VS_OUTPUT_PARTICLE output;

	float idx = pPos.z;

	float4 data = particle_data[ idx ];

	float4 center = mul(float4(data.xyz,1), world);
	float size = data.w;

	float3 loc = center.xyz 
	           + camera_up * size * pPos.y
	           + camera_left * size * pPos.x;
 	output.position = mul(float4(loc,1), world_view_proj );
	output.tex0 = float2( pPos.x + 0.5f, 0.5f - pPos.y );	
	output.color = particle_data[ idx + 1 ];
	return output;
}

VS_OUTPUT_DUMMY Render_Scene_Quad_VS( float4 pPos: POSITION
	, float2 pTex0 : TEXCOORD0)
{
	VS_OUTPUT_DUMMY output;
	
	//pTex0 += amplitudeMovQuad*sin( 2.0f*time + 15.0f*pTex0.x*pTex0.y );
	
	output.position = mul(pPos, world_view_proj);
	output.tex0 = pTex0;

	return output;
}

VS_OUTPUT_DUMMY Dummy_CalModel_VS(
     float4 inPos:  POSITION
   , float2 pTex0 : TEXCOORD0 
   , float4 inWeights: TEXCOORD1
   , float4 inIndexes: TEXCOORD2
) {
	VS_OUTPUT_DUMMY output;

	float4 tempPos;
	tempPos.xyz = inPos.xyz;
	tempPos.w = 1.0f;

	float3 pos = mul(Bones[ inIndexes.x ], tempPos) * inWeights.x;
	pos += mul(Bones[ inIndexes.y ], tempPos) * inWeights.y;
	//pos += mul(Bones[ inIndexes.z ], tempPos) * inWeights.z;
	//pos += mul(Bones[ inIndexes.w ], tempPos) * inWeights.w;

	output.position = mul( float4(pos,1.0f), world_view_proj );
	output.tex0 = pTex0;

	return output;
}

//-------------------------//
//      PIXEL SHADERS      //
//-------------------------//

float4 Dummy_PS( float2 pTex0: TEXCOORD0 ) : COLOR
{
	// Obtain texture color from sampler
	float4 diffuse_tex = tex2D(diffuse_sampler, pTex0);
	
	float4 color = status_color+diffuse_tex;
	color.w = diffuse_tex.w; // Set color alpha value directly from diffuse texture
	
	return color;
}

float4 Dummy_TS_PS( float2 pTex0: TEXCOORD0 ) : COLOR
{
	// Obtain texture color from sampler
	//pTex0 += time * texture_speed;
	float4 diffuse_tex = tex2D(diffuse_sampler, pTex0);
	
	float4 color = status_color+diffuse_tex;
	color.w = diffuse_tex.w; // Set color alpha value directly from diffuse texture
	
	return color;
}

float4 Dummy_Lava_PS( float2 pTex0: TEXCOORD0 ) : COLOR
{
	// Move texture coordinates
	//pTex0.x += time*0.009f;
	//pTex0 += 0.002f*sin( 2.0f*time + 15.0f*pTex0.x );
	//pTex0 += 0.002f*sin( 2.0f*time + 5.0f*pTex0.y*pTex0.x );
	
	// Obtain texture color from sampler
	float4 diffuse_tex = tex2D(diffuse_sampler, pTex0);
	diffuse_tex.x += 0.1f;
	diffuse_tex.y += 0.1f;
	
	return diffuse_tex;
}

float4 Render_Scene_Quad_PS( float2 pTex0: TEXCOORD0 ) : COLOR
{
	// Return texture color from sampler
	return tex2D(diffuse_sampler, pTex0);
}

float4 Render_Scene_Title_PS( float2 pTex0: TEXCOORD0 ) : COLOR
{
	//pTex0 += 0.005f*sin( 2.0f*time + 15.0f*pTex0.x*pTex0.y );
	
	float4 color = tex2D(diffuse_sampler, pTex0);
	//float w = color.w;
	//color += 0.2f*sin( 2.0f*time + 10.0f*pTex0.x*pTex0.y );
	//color.w = w; // Set color alpha value directly from diffuse texture
	
	return color;
}

float4 Render_Scene_Particle_PS( float2 pTex0: TEXCOORD0, float4 pColor0: TEXCOORD1 ) : COLOR
{
	// Obtain texture color from sampler
	float4 diffuse_tex = tex2D(diffuse_sampler, pTex0);
	
	// In pixel shader 1_1 this is not allowed because pColor0.w can not be access
	//float4 color = diffuse_tex * pColor0;
	diffuse_tex.xyz *= pColor0.xyz;
	
	return diffuse_tex;
}

//-------------------------//
//       TECHNIQUES        //
//-------------------------//

technique dummy_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Dummy_VS();
		PixelShader = compile ps_1_1 Dummy_PS();
	}
}

technique dummy_tech_wireframe
{
	pass p0 {
		FILLMODE = WIREFRAME;		
		VertexShader = compile vs_1_1 Dummy_VS();
		PixelShader = compile ps_1_1 Dummy_PS();
	}
}

technique basic_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Dummy_VS();
		PixelShader = compile ps_1_1 Dummy_PS();
	}
}

technique basic_tech_texture_speed
{
	pass p0 {		
		VertexShader = compile vs_1_1 Dummy_VS();
		PixelShader = compile ps_1_1 Dummy_TS_PS();
	}
}

technique normal_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Dummy_VS();
		PixelShader = compile ps_1_1 Dummy_PS();
	}
}

technique lava_normal_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Dummy_VS();
		PixelShader = compile ps_1_1 Dummy_Lava_PS();
	}
}

technique quad_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Render_Scene_Quad_VS();
		PixelShader = compile ps_1_1 Render_Scene_Quad_PS();
	}
}

technique skinning_tech
{
	pass p0 {
		VertexShader = compile vs_1_1 Dummy_CalModel_VS();
		PixelShader = compile ps_1_1 Dummy_PS();
	}
}

technique skinning_normal_tech
{
	pass p0 {
		VertexShader = compile vs_1_1 Dummy_CalModel_VS();
		PixelShader = compile ps_1_1 Dummy_PS();
	}
}

technique add_particle_tech
{
	pass p0 {
		ALPHABLENDENABLE = 1;
		SRCBLEND = SRCALPHA;
		DESTBLEND = ONE;
		
		VertexShader = compile vs_1_1 Render_Scene_Particle_VS();
		PixelShader = compile ps_1_1 Render_Scene_Particle_PS();
	}
}

technique modulate_particle_tech
{
	pass p0 {
		ALPHABLENDENABLE = 1;
		SRCBLEND = SRCALPHA;
		DESTBLEND = INVSRCALPHA;
		
		VertexShader = compile vs_1_1 Render_Scene_Particle_VS();
		PixelShader = compile ps_1_1 Render_Scene_Particle_PS();
	}
}

technique title_tech
{
	pass p0 {
		VertexShader = compile vs_1_1 Dummy_VS();
		PixelShader = compile ps_1_1 Render_Scene_Title_PS();
	}
}
