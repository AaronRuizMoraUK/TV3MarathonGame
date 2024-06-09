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

struct VS_OUTPUT {
	float4 position : POSITION;
	float2 tex0 : TEXCOORD0;
	float3 posPS : TEXCOORD1;
	float3 norPS : TEXCOORD2;
};

struct VS_OUTPUT_NORMAL {
	float4 position : POSITION;
	float2 tex0 : TEXCOORD0;
	float3 light_tan_vec : TEXCOORD1; // Light vector in tangencial coordinates
	float3 half_tan_vec : TEXCOORD2; // Half vector in tangencial coordinates
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

// Create a sampler with sampler_state (reserved word) and set its Texture param for diffse map.
sampler diffuse_sampler_rt = sampler_state
{
	Texture = (diffuse_texture); // Texture is a parameter of sampler
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
	// CLAMP texture tiling
	ADDRESSU = CLAMP;
	ADDRESSV = CLAMP;
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
#define MAX_PARTICLES	100 // also used for shadows
uniform float3x4 Bones[MAX_BONES];
uniform float4 particle_data[MAX_PARTICLES];
uniform float3 camera_up;
uniform float3 camera_left;

uniform float3 planet_center = float3(0,0,0);
uniform float4 godray_color = float4(0.6,0.3,0.1,1);

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

VS_OUTPUT_DUMMY Render_Scene_Shadow_VS( float4 pPos: POSITION
	, float2 pTex0 : TEXCOORD0)
{
	VS_OUTPUT_DUMMY output;

	float idx = pPos.z * 2;

	float4 data1 = particle_data[ idx ];
	float4 data2 = particle_data[ idx + 1 ];
	
	float3 object_front = data1.xyz;
	float3 object_left = data2.xyz;
	float3 object_up = cross( object_front, object_left );

	float planet_distance = data1.w;
	float size = data2.w;

	float3 loc = planet_center + object_up * planet_distance
	           + object_front * size * pPos.y
	           - object_left * size * pPos.x;
 	output.position = mul(float4(loc,1), world_view_proj );
	output.tex0 = pTex0;
	return output;
	
}

VS_OUTPUT Render_Scene_VS( float4 pPos: POSITION
	, float3 pNor: NORMAL
	, float2 pTex0 : TEXCOORD0)
{
	VS_OUTPUT output;

	output.position = mul(pPos, world_view_proj);
	output.tex0 = pTex0;
	output.posPS = mul(pPos, world);
	output.norPS = mul(pNor, (float3x3)world);

	return output;
}

VS_OUTPUT Render_Godray_VS( float4 pPos: POSITION
	, float3 pNor: NORMAL)
{
	VS_OUTPUT output;

	output.position = mul(pPos, world_view_proj);
	output.posPS = mul(pPos, world);
	output.norPS = mul(pNor, (float3x3)world);
	output.tex0 = float2(pPos.x,pPos.z);

	return output;
}

VS_OUTPUT_NORMAL Render_Scene_Normal_VS( float4 pPos: POSITION
	, float3 pNor: NORMAL
	, float4 pTan4: TEXCOORD1
	, float2 pTex0 : TEXCOORD0)
{
	float3 pTan  = pTan4.xyz;
	float  pTanW = pTan4.w;
	
	// Position in world space
	float4 world_pos = mul( pPos, world );
	
	// Matrix to express a vector to tangencial space
	float3 biNor = pTanW*cross(pNor, pTan);
	float3x3 matrixTan= float3x3(pTan, biNor, pNor); // Save in rows, be careful!!
	
	// Tangencial matrix in world coordinates
	matrixTan = mul(matrixTan, world);
	
	// Light vector
	float3 light_vector = normalize( light_pos - world_pos.xyz );

	// Eye Front
	float3 eye_front = normalize( camera_pos - world_pos.xyz );
	
	// Half Vector between light_vector y eye_front
	float3 half_vector = normalize( light_vector + eye_front );	

	VS_OUTPUT_NORMAL output;

	output.position = mul( pPos, world_view_proj );
	output.tex0 = pTex0;
	output.light_tan_vec = mul(matrixTan, light_vector); // Light vector in tangencial space
	output.half_tan_vec = mul(matrixTan, half_vector); // Half vector in tangencial space

	return output;
}

VS_OUTPUT_DUMMY Render_Scene_Quad_VS( float4 pPos: POSITION
	, float2 pTex0 : TEXCOORD0)
{
	VS_OUTPUT_DUMMY output;
	
	pTex0 += amplitudeMovQuad*sin( 2.0f*time + 15.0f*pTex0.x*pTex0.y );
	
	output.position = mul(pPos, world_view_proj);
	output.tex0 = pTex0;

	return output;
}

VS_OUTPUT_NORMAL Render_Scene_CalModel_Normal_VS(
     float4 inPos:  POSITION
   , float3 pNor: NORMAL
   , float2 pTex0 : TEXCOORD0 
   , float4 inWeights: TEXCOORD1
   , float4 inIndexes: TEXCOORD2
   , float4 pTan4: TEXCOORD3
) {
	float3 pTan  = pTan4.xyz;
	float  pTanW = pTan4.w;

	VS_OUTPUT_NORMAL output;

	float4 tempPos;
	tempPos.xyz = inPos.xyz;
	tempPos.w = 1.0f;

	float3 pos = mul(Bones[ inIndexes.x ], tempPos) * inWeights.x;
	pos += mul(Bones[ inIndexes.y ], tempPos) * inWeights.y;
	//pos += mul(Bones[ inIndexes.z ], tempPos) * inWeights.z;
	//pos += mul(Bones[ inIndexes.w ], tempPos) * inWeights.w;
	
	float3 nor = mul((float3x3)Bones[ inIndexes.x ], pNor) * inWeights.x;
	nor += mul((float3x3)Bones[ inIndexes.y ], pNor) * inWeights.y;
	//nor += mul((float3x3)Bones[ inIndexes.z ], pNor) * inWeights.z;
	//nor += mul((float3x3)Bones[ inIndexes.w ], pNor) * inWeights.w;
	nor = normalize(nor);

	float3 tan = mul((float3x3)Bones[ inIndexes.x ], pTan) * inWeights.x;
	tan += mul((float3x3)Bones[ inIndexes.y ], pTan) * inWeights.y;
	//tan += mul((float3x3)Bones[ inIndexes.z ], pTan) * inWeights.z;
	//tan += mul((float3x3)Bones[ inIndexes.w ], pTan) * inWeights.w;
	tan = normalize(tan);

	// Position in world space
	float4 world_pos = mul( pos, world );
	
	// Matrix to express a vector to tangencial space
	float3 biNor = pTanW*cross(pNor, pTan);
	float3x3 matrixTan= float3x3(tan, biNor, nor); // Save in rows, be careful!!
	
	// Tangencial matrix in world coordinates
	matrixTan = mul(matrixTan, world);
	
	// Light vector
	float3 light_vector = normalize( light_pos - world_pos.xyz );

	// Eye Front
	float3 eye_front = normalize( camera_pos - world_pos.xyz );
	
	// Half Vector between light_vector y eye_front
	float3 half_vector = normalize( light_vector + eye_front );


	output.position = mul( float4(pos,1.0f), world_view_proj );
	output.tex0 = pTex0;
	output.light_tan_vec = mul(matrixTan, light_vector); // Light vector in tangencial space
	output.half_tan_vec = mul(matrixTan, half_vector); // Half vector in tangencial space

	return output;
}


VS_OUTPUT Render_Scene_CalModel_VS(
     float4 inPos:  POSITION
   , float3 pNor: NORMAL
   , float2 pTex0 : TEXCOORD0 
   , float4 inWeights: TEXCOORD1
   , float4 inIndexes: TEXCOORD2
) {
	VS_OUTPUT output;

	float4 tempPos;
	tempPos.xyz = inPos.xyz;
	tempPos.w = 1.0f;

	float3 pos = mul(Bones[ inIndexes.x ], tempPos) * inWeights.x;
	pos += mul(Bones[ inIndexes.y ], tempPos) * inWeights.y;
	//pos += mul(Bones[ inIndexes.z ], tempPos) * inWeights.z;
	//pos += mul(Bones[ inIndexes.w ], tempPos) * inWeights.w;
	
	float3 nor = mul((float3x3)Bones[ inIndexes.x ], pNor) * inWeights.x;
	nor += mul((float3x3)Bones[ inIndexes.y ], pNor) * inWeights.y;
	//nor += mul((float3x3)Bones[ inIndexes.z ], pNor) * inWeights.z;
	//nor += mul((float3x3)Bones[ inIndexes.w ], pNor) * inWeights.w;
	nor = normalize(nor);

	output.position = mul( float4(pos,1.0f), world_view_proj );
	output.tex0 = pTex0;
	output.posPS = mul(float4(pos,1.0f), world);
	output.norPS = mul(nor, (float3x3)world);


	return output;
}

VS_OUTPUT_TEXTURE Render_Texture_VS( float4 inPos: POSITION ) {
	VS_OUTPUT_TEXTURE output;
	
	float4 inTex0 = mul( inPos, world_view_proj );
	output.pos = inTex0;
	
	inTex0.x += 0.15f * sin( 5.0f * time + 0.5f * inTex0.x );
	inTex0.y += 0.15f * sin( 5.0f * time + 0.5f * inTex0.y );
	
	inTex0.x += inTex0.w;
	inTex0.y = -(inTex0.y-inTex0.w);
	inTex0.w *= 2.0f;
	
	output.tex0 = inTex0;
	
	//output.pos = mul( inPos, world_view_proj );
	//output.tex0 = output.pos;
	
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

float4 Dummy2_PS( float2 pTex0: TEXCOORD0 ) : COLOR
{
	// Obtain texture color from sampler
	//float4 diffuse_tex = float4(0,0,0,1);
	
	// This fixs the first row and column of pixels, these are affected by the last, so, put it black
	//float value = pTex0.x * pTex0.y;
	//if( value != 0.0f )
	//	diffuse_tex = tex2D(diffuse_sampler, pTex0);
	
	float4 diffuse_tex = tex2D(diffuse_sampler_rt, pTex0);
	
	return diffuse_tex;
}

float4 Render_Scene_PS( float2 pTex0: TEXCOORD0
	, float3 pPosPS : TEXCOORD1
	, float3 pNorPS : TEXCOORD2 ) : COLOR
{
	// Normalize normal
	pNorPS = normalize(pNorPS);
	//return abs(float4(pNorPS, 1.0f));

	// Obtain texture color from sampler
	float4 diffuse_tex = tex2D(diffuse_sampler, pTex0);
	
	// Light Vector
	float3 light_vector = normalize( light_pos - pPosPS );
	
	// Eye Front
	float3 eye_front = normalize( camera_pos - pPosPS );
	
	// Half Vector
	float3 half_vector = normalize( light_vector + eye_front );
		
	// Diffuse angle affected by light
	float diffuse_angle = saturate( dot(pNorPS, light_vector) );
	
	// Specular angle affected by light
	float specular_angle = pow( saturate( dot(pNorPS, half_vector) ), specular_factor);

	float4 diffuse_color = (ambient_color + diffuse_angle)*diffuse_tex;
	float specular_color = specular_alfa*specular_angle;

	float4 color = (status_color + diffuse_color + specular_color)*light_color;
	color.w = diffuse_tex.w; // Set color alpha value directly from diffuse texture
	return color;
}

float4 Render_Scene_TS_PS( float2 pTex0: TEXCOORD0
	, float3 pPosPS : TEXCOORD1
	, float3 pNorPS : TEXCOORD2 ) : COLOR
{
	// Normalize normal
	pNorPS = normalize(pNorPS);
	//return abs(float4(pNorPS, 1.0f));

	// Obtain texture color from sampler
	pTex0.x += time * texture_speed;
	float4 diffuse_tex = tex2D(diffuse_sampler, pTex0);
	
	// Light Vector
	float3 light_vector = normalize( light_pos - pPosPS );
	
	// Eye Front
	float3 eye_front = normalize( camera_pos - pPosPS );
	
	// Half Vector
	float3 half_vector = normalize( light_vector + eye_front );
		
	// Diffuse angle affected by light
	float diffuse_angle = saturate( dot(pNorPS, light_vector) );
	
	// Specular angle affected by light
	float specular_angle = pow( saturate( dot(pNorPS, half_vector) ), specular_factor);

	float4 diffuse_color = (ambient_color + diffuse_angle)*diffuse_tex;
	float specular_color = specular_alfa*specular_angle;

	float4 color = (status_color + diffuse_color + specular_color)*light_color;
	color.w = diffuse_tex.w; // Set color alpha value directly from diffuse texture
	return color;
}

float4 Render_Scene_Normal_PS( float2 pTex0: TEXCOORD0
	, float3 inLight_tang_vec: TEXCOORD1
	, float3 inHalf_tang_vec: TEXCOORD2 ) : COLOR
{
	// Normalize light and half vector in tangencial space
	inLight_tang_vec = normalize( inLight_tang_vec );
	inHalf_tang_vec = normalize( inHalf_tang_vec );

	// Obtain texture color from diffuse sampler
	float4 diffuse_tex = tex2D(diffuse_sampler, pTex0);

	// Obtain normal data from normal sampler
	float4 normal_tex = tex2D( normal_sampler, pTex0);
	
	// Set the normal data in the correct range
	float3 normal_tex_norm = normalize( normal_tex.xyz*2 - 1.0f );
	//normal_tex_norm = float3( 0,0,1 );
	
	// Obtain the light angle in tangencial space
	float diffuse_angle_tan = saturate( dot(normal_tex_norm, inLight_tang_vec) );
	
	// Obtain the specular angle in tangencial space
	float specular_angle_tan = diffuse_angle_tan*pow( saturate( dot(normal_tex_norm, inHalf_tang_vec) ), specular_factor );

	float4 diffuse_color = (ambient_color + diffuse_angle_tan)*diffuse_tex;
	float specular_color = specular_alfa*specular_angle_tan;

	float4 color = (status_color + diffuse_color + specular_color)*light_color;
	color.w = diffuse_tex.w; // Set color alpha value directly from diffuse texture
	return color;
}

float4 Render_Scene_Lava_PS( float2 pTex0: TEXCOORD0
	, float3 pPosPS : TEXCOORD1
	, float3 pNorPS : TEXCOORD2 ) : COLOR
{
	// Move texture coordinates
	pTex0.x += time*0.009f;
	pTex0 += 0.002f*sin( 2.0f*time + 15.0f*pTex0.x );
	pTex0 += 0.002f*sin( 2.0f*time + 5.0f*pTex0.y*pTex0.x );

	// Normalize normal
	pNorPS = normalize(pNorPS);
	//return abs(float4(pNorPS, 1.0f));

	// Obtain texture color from sampler
	float4 diffuse_tex = tex2D(diffuse_sampler, pTex0);
	
	// Light Vector
	float3 light_vector = normalize( light_pos - pPosPS );
	
	// Eye Front
	float3 eye_front = normalize( camera_pos - pPosPS );
	
	// Half Vector
	float3 half_vector = normalize( light_vector + eye_front );
		
	// Diffuse angle affected by light
	float diffuse_angle = saturate( dot(pNorPS, light_vector) );
	
	// Specular angle affected by light
	float specular_angle = pow( saturate( dot(pNorPS, half_vector) ), specular_factor);

	float4 diffuse_color = (ambient_color + diffuse_angle)*diffuse_tex;
	float specular_color = specular_alfa*specular_angle;

	float4 color = (status_color + diffuse_color + specular_color)*light_color;
	color.w = diffuse_tex.w; // Set color alpha value directly from diffuse texture
	color.x += 0.1f;
	color.y += 0.1f;
	
	return color;
}

float4 Render_Scene_Lava_Normal_PS( float2 pTex0: TEXCOORD0
	, float3 inLight_tang_vec: TEXCOORD1
	, float3 inHalf_tang_vec: TEXCOORD2 ) : COLOR
{
	// Move texture coordinates
	pTex0.x += time*0.009f;
	pTex0 += 0.002f*sin( 2.0f*time + 15.0f*pTex0.x );
	pTex0 += 0.002f*sin( 2.0f*time + 5.0f*pTex0.y*pTex0.x );

	// Normalize light and half vector in tangencial space
	inLight_tang_vec = normalize( inLight_tang_vec );
	inHalf_tang_vec = normalize( inHalf_tang_vec );

	// Obtain texture color from diffuse sampler
	float4 diffuse_tex = tex2D(diffuse_sampler, pTex0);

	// Obtain normal data from normal sampler
	float4 normal_tex = tex2D( normal_sampler, pTex0);
	
	// Set the normal data in the correct range
	float3 normal_tex_norm = normalize( normal_tex.xyz*2 - 1.0f );
	
	// Obtain the light angle in tangencial space
	float diffuse_angle_tan = clamp( dot(normal_tex_norm, inLight_tang_vec), 0.3f, 1.0f );
	
	// Obtain the specular angle in tangencial space
	float specular_angle_tan = pow( saturate( dot(normal_tex_norm, inHalf_tang_vec) ), specular_factor );

	float4 diffuse_color = (ambient_color + diffuse_angle_tan)*diffuse_tex;
	float specular_color = specular_alfa*specular_angle_tan;

	float4 color = (status_color + diffuse_color + specular_color)*light_color;
	color.w = diffuse_tex.w; // Set color alpha value directly from diffuse texture
	color.x += 0.1f;
	color.y += 0.1f;
	
	return color;
}

float4 Render_Scene_Quad_PS( float2 pTex0: TEXCOORD0 ) : COLOR
{
	// Return texture color from sampler
	return tex2D(diffuse_sampler, pTex0);
}

float4 Render_Scene_Title_PS( float2 pTex0: TEXCOORD0 ) : COLOR
{
	pTex0 += 0.005f*sin( 2.0f*time + 15.0f*pTex0.x*pTex0.y );
	
	float4 color = tex2D(diffuse_sampler, pTex0);
	float w = color.w;
	color += 0.2f*sin( 2.0f*time + 10.0f*pTex0.x*pTex0.y );
	color.w = w; // Set color alpha value directly from diffuse texture
	
	return color;
}

float4 Render_Texture_PS( float4 inTex0: TEXCOORD0 ) : COLOR {	

	/*
	// VS
	float4 inTex0 = mul( inPos, world_view_proj );
	output.pos = inTex0;
	
	inTex0.x += 0.1f * cos( 5.0f * time + 0.5f * inTex0.x );
	inTex0.y += 0.1f * sin( 5.0f * time + 0.5f * inTex0.y );
	
	inTex0.x += inTex0.w;
	inTex0.y = -(inTex0.y-inTex0.w);
	inTex0.w *= 2.0f;
	
	output.tex0 = inTex0;
	*/


	/*
	inTex0.x += 0.1f * sin( 5.0f * time + 0.5f * inTex0.x );
	inTex0.y += 0.1f * sin( 5.0f * time + 0.5f * inTex0.y );
	
	inTex0.x += inTex0.w;
	inTex0.y = -(inTex0.y-inTex0.w);
	inTex0.w *= 2.0f;
	*/
	
	float4 clr = tex2Dproj( diffuse_sampler, inTex0 );
	clr.w = 0.5f;
	return clr;
}

float4 Render_Scene_Particle_PS( float2 pTex0: TEXCOORD0, float4 pColor0: TEXCOORD1 ) : COLOR
{
	// Obtain texture color from sampler
	float4 diffuse_tex = tex2D(diffuse_sampler, pTex0);
	
	float4 color = diffuse_tex * pColor0;
	
	return color;
}

float4 Render_Godray_PS( float2 pPosLocal : TEXCOORD0, float3 pPosPS : TEXCOORD1
	, float3 pNorPS : TEXCOORD2 ) : COLOR
{
	// Normalize normal
	pNorPS = normalize(pNorPS);
	
	// Eye Front
	float3 eye_front = normalize( camera_pos - pPosPS );
	
	/*
	// Angle betwen Diffuse angle affected by light
	float camera_pos_angle = saturate( dot(pNorPS, eye_front) ) - 0.5f;

	// Distance
	float distance = 1.0f - 6.0f/(length(pPosLocal)+1.0f);
	
	float4 color = godray_color;
	color.w = camera_pos_angle*0.3f - distance*0.4f; // Set color alpha value directly from diffuse texture
	*/
	
	// Angle betwen Diffuse angle affected by light
	float camera_pos_angle = saturate( dot(pNorPS, eye_front) );

	// Distance
	float distance = 0.14f*pPosLocal.y;
	
	float4 color = godray_color;
	color.w = camera_pos_angle - distance + 0.5f; // Set color alpha value directly from diffuse texture
	
	return color;
}


//-------------------------//
//       TECHNIQUES        //
//-------------------------//

technique dummy_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Dummy_VS();
		PixelShader = compile ps_2_0 Dummy_PS();
	}
}

technique dummy2_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Dummy_VS();
		PixelShader = compile ps_2_0 Dummy2_PS();
	}
}

technique dummy_tech_wireframe
{
	pass p0 {
		FILLMODE = WIREFRAME;		
		VertexShader = compile vs_1_1 Dummy_VS();
		PixelShader = compile ps_2_0 Dummy_PS();
	}
}

technique basic_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Render_Scene_VS();
		PixelShader = compile ps_2_0 Render_Scene_PS();
	}
}

technique basic_tech_texture_speed
{
	pass p0 {		
		VertexShader = compile vs_1_1 Render_Scene_VS();
		PixelShader = compile ps_2_0 Render_Scene_TS_PS();
	}
}

technique normal_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Render_Scene_Normal_VS();
		PixelShader = compile ps_2_0 Render_Scene_Normal_PS();
	}
}

technique lava_normal_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Render_Scene_Normal_VS();
		PixelShader = compile ps_2_0 Render_Scene_Lava_Normal_PS();
	}
}

technique quad_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Render_Scene_Quad_VS();
		PixelShader = compile ps_2_0 Render_Scene_Quad_PS();
	}
}

technique skinning_tech
{
	pass p0 {
		VertexShader = compile vs_1_1 Render_Scene_CalModel_VS();
		PixelShader = compile ps_2_0 Render_Scene_PS();
	}
}

technique skinning_normal_tech
{
	pass p0 {
		VertexShader = compile vs_1_1 Render_Scene_CalModel_Normal_VS();
		PixelShader = compile ps_2_0 Render_Scene_Normal_PS();
	}
}


technique add_particle_tech
{
	pass p0 {
		ALPHABLENDENABLE = 1;
		SRCBLEND = SRCALPHA;
		DESTBLEND = ONE;
		
		VertexShader = compile vs_1_1 Render_Scene_Particle_VS();
		PixelShader = compile ps_2_0 Render_Scene_Particle_PS();
	}
}

technique modulate_particle_tech
{
	pass p0 {
		ALPHABLENDENABLE = 1;
		SRCBLEND = SRCALPHA;
		DESTBLEND = INVSRCALPHA;
		
		VertexShader = compile vs_1_1 Render_Scene_Particle_VS();
		PixelShader = compile ps_2_0 Render_Scene_Particle_PS();
	}
}

technique shadow_tech
{
	pass p0 {
		VertexShader = compile vs_1_1 Render_Scene_Shadow_VS();
		PixelShader = compile ps_2_0 Dummy_PS();
	}
}

technique title_tech
{
	pass p0 {
		VertexShader = compile vs_1_1 Dummy_VS();
		PixelShader = compile ps_2_0 Render_Scene_Title_PS();
	}
}

technique render_texture_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Render_Texture_VS();
		PixelShader = compile ps_2_0 Render_Texture_PS();
	}
}

technique godray_tech
{
	pass p0 {		
		VertexShader = compile vs_1_1 Render_Godray_VS();
		PixelShader = compile ps_2_0 Render_Godray_PS();
	}
}
