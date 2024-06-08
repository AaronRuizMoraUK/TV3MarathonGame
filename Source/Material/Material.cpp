#include <cassert>
#include "Material.h"
#include "Factories/TextureFactory.h"
#include "Factories/EffectFactory.h"
#include "Game/Game.h"
#include "Camera/ICamera.h"
#include "Global/Geometry.h"
#include "Global/Rendering.h"
#include "Factories/MeshFactory.h"
#include "cal3d/cal3d.h"
//#include "Particles/OwnParticles/ParticleEmitter.h"

// Destructor
TextureMapVector::~TextureMapVector() {
	printDebugString("Destroying TextureMapVector...\n");
	clear();
}

Material::~Material(){
	printDebugString("Destroying Material...\n");

	// Automaticaly texture map vector destroies itself

	// EffectFactory is who destrois the effect (shader)!!
}

// Destructor
MaterialVector::~MaterialVector() {
	printDebugString("Destroying MaterialVector...\n");
	clear();
}

// --------------------------------------------------------------------------------------------

D3DXVECTOR3 lightPosition( 60.0f, 85.0f, 50.0f );

// VERY VERY VERY OPTIONAL!!!
Material::Material(const std::string &filename, const std::string &pShaderName, const std::string &pTechniqueName)
: shader( NULL )
, shaderName(pShaderName)
, techniqueName(pTechniqueName)
, texture_speed( 0.0f )
{
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matWorldViewProj);

	// Texture 1
	TextureMap * textureMap = new TextureMap();
	textureMap->mapFilename=filename;

	// Create the texture
	textureMap->texture = TextureFactory::get()->createTexture(filename);
	if(textureMap->texture->tex == NULL)
		return;

	if( techniqueName == "normal_tech" || techniqueName == "skinning_normal_tech" || techniqueName == "lava_normal_tech" ) {
		std::string filenameNormal = filename;
		std::string::size_type pos = filenameNormal.find(".dds");
		filenameNormal.insert(pos, "_normalmap");

		textureMap->normalMap = TextureFactory::get()->createTexture(filenameNormal);
		if(textureMap->normalMap->tex == NULL)
			return;
	}

	// Include textures in vector
	textureMapVector.push_back(textureMap);

	// Create the Shader TODO OPTIONAL
	shader = EffectFactory::get()->createEffect(shaderName);
	assert( shader );
}

// VERY VERY VERY OPTIONAL!!!
Material::Material(const Texture *texture, const std::string &pShaderName, const std::string &pTechniqueName)
: shader( NULL )
, shaderName(pShaderName)
, techniqueName(pTechniqueName)
, texture_speed( 0.0f )
{
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matWorldViewProj);

	// Texture 1
	TextureMap * textureMap = new TextureMap();

	// Create the texture
	assert( texture );
	textureMap->texture = texture;
	if(textureMap->texture->tex == NULL)
		return;

	assert( techniqueName != "normal_tech" && techniqueName != "skinning_normal_tech" && techniqueName != "lava_normal_tech" );

	// Include textures in vector
	textureMapVector.push_back(textureMap);

	// Create the Shader TODO OPTIONAL
	shader = EffectFactory::get()->createEffect(shaderName);
	assert( shader );
}

/*
void Material::activateShadow( const ShadowInformation *shadowInfs, int numShadowInformations ) const
{
D3DXHANDLE handleParam = NULL;

D3DXHANDLE handle_param = shader->GetParameterByName( NULL, "particle_data" );
if( handle_param )  {
shader->SetVectorArray( handle_param, (D3DXVECTOR4 *)shadowInfs, numShadowInformations * 2);
}

handleParam = shader->GetParameterByName( NULL, "planet_center" );
if( handleParam ) {
const D3DXVECTOR3 &planetCenter = PlanetManager::getPlanetManager()->getCurrentPlanet()->getCenter();
D3DXVECTOR4 planetCenter4( planetCenter.x, planetCenter.y, planetCenter.z, 0.0f );
shader->SetVector( handleParam, &planetCenter4);
}
}

void Material::activateParticle( const ParticleEmitter &particle ) const
{
D3DXHANDLE handleParam = NULL;

struct TMatrix2x4 {
float r00, r01, r02, r03;
float r04, r05, r06, r07;
};

//	size_t cnt = IParticles::particleContext.GetGroupCount();
size_t cnt = particle.getNumQuads();

if(cnt < 1)
return;

TMatrix2x4 matrices[ParticleEmitter::MAX_PARTICLES];
//memset( &matrices[0].r00, 0x00, sizeof(TMatrix2x4)*ParticleEmitter::MAX_PARTICLES );

assert( cnt <= ParticleEmitter::MAX_PARTICLES ); 

for (size_t i = 0; i < cnt; i++)
{
const D3DXVECTOR3 &pos = particle.getParticlePosition(i);

D3DXVECTOR3 realPos;

if (particle.isLocal)
D3DXVec3TransformCoord( &realPos, &pos , &(particle.getParent()->getTransformMatrix()) );
else
realPos = pos;

float size  = particle.getSize(i);
const D3DXVECTOR4 &color = particle.getColor(i);

matrices[i].r00 = realPos.x;
matrices[i].r01 = realPos.y;
matrices[i].r02 = realPos.z;
matrices[i].r03 = size;
matrices[i].r04 = color.x;
matrices[i].r05 = color.y;
matrices[i].r06 = color.z;
matrices[i].r07 = color.w;
}

D3DXHANDLE handle_param = shader->GetParameterByName( NULL, "particle_data" );
if( handle_param ) 
{
shader->SetVectorArray( handle_param, (D3DXVECTOR4 *) matrices, cnt * 2);
}


handleParam = shader->GetParameterByName( NULL, "camera_up" );
if( handleParam ) {
const D3DXVECTOR3 & cameraUp = getCurrentCamera( )->getLocalUpVector();
D3DXVECTOR4 cameraUp4( cameraUp.x, cameraUp.y, cameraUp.z, 0.0f );
shader->SetVector( handleParam, &cameraUp4);
}

handleParam = shader->GetParameterByName( NULL, "camera_left" );
if( handleParam ) {
const D3DXVECTOR3 & cameraLeft = getCurrentCamera( )->getLocalLeftVector();
D3DXVECTOR4 cameraLeft4( -cameraLeft.x, -cameraLeft.y, -cameraLeft.z, 0.0f );
shader->SetVector( handleParam, &cameraLeft4);
}
}
*/

void Material::activateCal(const CalModel &calModel) const
{
	D3DXHANDLE handleParam = NULL;
	EffectDX shader_fx = shader->fx;
	assert( shader_fx );

	struct TMatrix3x4 {
		float r00, r01, r02, tx;
		float r10, r11, r12, ty;
		float r20, r21, r22, tz;
	};

	TMatrix3x4 matrices[ MeshFactory::MAX_BONES ];
	//memset( &matrices[0].r00, 0x00, sizeof(TMatrix3x4)*MeshFactory::MAX_BONES );

	typedef std::vector< CalBone * > VBones;
	assert( calModel.getSkeleton( ) );
	const VBones &bones = calModel.getSkeleton( )->getVectorBone( );
	assert( bones.size( ) <= MeshFactory::MAX_BONES );

	for( size_t i=0; i<bones.size( ); ++i ) {
		const CalMatrix &mtx = bones[ i ]->getTransformMatrix( );
		const CalVector &trans = bones[ i ]->getTranslationBoneSpace( );
		// Convertir la matrix de cal a una de 3x4 traspuesta
		matrices[ i ].r00 = mtx.dxdx;
		matrices[ i ].r01 = mtx.dxdy;
		matrices[ i ].r02 = mtx.dxdz;
		matrices[ i ].tx  = trans.x;

		matrices[ i ].r10 = mtx.dydx;
		matrices[ i ].r11 = mtx.dydy;
		matrices[ i ].r12 = mtx.dydz;
		matrices[ i ].ty  = trans.y;

		matrices[ i ].r20 = mtx.dzdx;
		matrices[ i ].r21 = mtx.dzdy;
		matrices[ i ].r22 = mtx.dzdz;
		matrices[ i ].tz  = trans.z;
	}

	handleParam = shader_fx->GetParameterByName( NULL, "Bones" );
	if( handleParam ) 
		shader_fx->SetFloatArray( handleParam, &matrices[ 0 ].r00, ((int)bones.size()) * 12 );
}

void Material::activate() const {

    static float var = 0.0f;
    var += 1.0f/60.0f;
    lightPosition = D3DXVECTOR3( 5.0f*sinf(var), 5, 2.5f*cosf(var) );

	D3DXHANDLE handleParam = NULL;
	EffectDX shader_fx = shader->fx;
	assert( shader_fx );

#ifndef NDEBUG
	// Status Color DEBUG
	handleParam = shader_fx->GetParameterByName( NULL, "status_color" );
	if( handleParam ) {
		if( additionalColor.enabled ) {
			D3DXVECTOR4 statusColor;
			formatColorDWordToVector4(statusColor, additionalColor.color);
			shader_fx->SetVector( handleParam, &statusColor );
		}
		else {
			const D3DXVECTOR4 black(0, 0, 0, 1);
			shader_fx->SetVector(handleParam, &black);
		}
	}
#endif

	// Update matrix wvp of shader
	handleParam = shader_fx->GetParameterByName( NULL, "world_view_proj" );
	if( handleParam )
		shader_fx->SetMatrix( handleParam, &matWorldViewProj );

	handleParam = shader_fx->GetParameterByName( NULL, "world" );
	if( handleParam )
		shader_fx->SetMatrix( handleParam, &matWorld );

	// Activate diffuse texture
	handleParam = shader_fx->GetParameterByName( NULL, "diffuse_texture" );
	if( handleParam ) 
		shader_fx->SetTexture( handleParam, textureMapVector[0]->texture->tex );

	//if(  techniqueName == "quad_tech"
	//	|| techniqueName == "basic_tech_texture_speed"
	//	|| techniqueName == "title_tech"
	//	|| techniqueName == "lava_normal_tech"
	//	|| techniqueName == "render_texture_tech"
	//	|| techniqueName == "godray_tech") {
			// Set the time
			handleParam = shader_fx->GetParameterByName( NULL, "time" );
			if( handleParam ) {
				shader_fx->SetFloat( handleParam, Game::shader_time );
			}
	//}

	// Set texture speed
	handleParam = shader_fx->GetParameterByName( NULL, "texture_speed" );
	if( handleParam ) {
		shader_fx->SetFloat( handleParam, texture_speed );
	}

	//if ( techniqueName == "basic_tech" 
	//	|| techniqueName == "basic_tech_texture_speed" 
	//	|| techniqueName == "normal_tech" 
	//	|| techniqueName == "skinning_normal_tech" 
	//	|| techniqueName == "lava_normal_tech" 
	//	|| techniqueName == "skinning_tech"
	//	|| techniqueName == "godray_tech" ) {
			// Set the light position
			handleParam = shader_fx->GetParameterByName( NULL, "light_pos" );
			if( handleParam ) {
				D3DXVECTOR4 lightPos4( lightPosition.x, lightPosition.y, lightPosition.z, 1.0f );
				shader_fx->SetVector( handleParam, &lightPos4 );
			}

			// Set the camera position
			handleParam = shader_fx->GetParameterByName( NULL, "camera_pos" );
			if( handleParam ) {
				const D3DXVECTOR3 & cameraPosition = getCurrentCamera( )->getPosition();
				D3DXVECTOR4 cameraPos4( cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f );
				shader_fx->SetVector( handleParam, &cameraPos4);
			}

			if(techniqueName == "normal_tech" || techniqueName == "skinning_normal_tech" || techniqueName == "lava_normal_tech") {
				// Activate normal map texture
				// TODO Look for the normal map in the Factory
				//std::string normalTextureFilename = "iuranus_surface_normalmap.dds";
				//Texture normalTexture = TextureFactory::getTextureFactory()->createTexture(normalTextureFilename);
				handleParam = shader_fx->GetParameterByName( NULL, "normal_texture" );
				if( handleParam ) 
					shader_fx->SetTexture( handleParam, textureMapVector[0]->normalMap->tex );
			}
	//}

	// Activate technique
	handleParam = shader_fx->GetTechniqueByName( techniqueName.c_str() );
	if( handleParam ) {
		HRESULT hr = shader_fx->SetTechnique( handleParam );
		assert( hr == D3D_OK );
	}
}

const Effect & Material::getShader() const {
	assert( shader );
	return *shader;
}

void Material::changeTexture( const std::string &texture_name ) {
	assert( textureMapVector.size( ) == 1 );
	textureMapVector[ 0 ]->mapFilename = texture_name;
	textureMapVector[ 0 ]->texture = TextureFactory::get()->createTexture(texture_name);
	if( textureMapVector[ 0 ]->normalMap != NULL ) {
		std::string filenameNormal = texture_name;
		std::string::size_type pos = filenameNormal.find(".dds");
		filenameNormal.insert(pos, "_normalmap");

		textureMapVector[ 0 ]->normalMap = TextureFactory::get()->createTexture(filenameNormal);
	}
}

void Material::changeTextureAndNormal( const std::string &texture_name, const std::string &normalmap_texture ) {
	assert( textureMapVector.size( ) == 1 );
	textureMapVector[ 0 ]->mapFilename = texture_name;
	textureMapVector[ 0 ]->texture = TextureFactory::get()->createTexture(texture_name);
	if( textureMapVector[ 0 ]->normalMap != NULL )
		textureMapVector[ 0 ]->normalMap = TextureFactory::get()->createTexture(normalmap_texture);
}

/*
void Material::setTechnique(const std::string &technique) {
assert( technique == "basic_tech" || technique == "normal_tech" );
techniqueName = technique;

if( techniqueName == "normal_tech" ) {
if( textureMapVector[0]->normalMap != NULL )
return;

std::string filenameNormal = textureMapVector[0]->mapFilename;
assert( filenameNormal != "" );
std::string::size_type pos = filenameNormal.find(".dds");
filenameNormal.insert(pos, "_normalmap");

textureMapVector[0]->normalMap = TextureFactory::getTextureFactory()->createTexture(filenameNormal);
assert( textureMapVector[0]->normalMap != NULL );
}
}
*/
