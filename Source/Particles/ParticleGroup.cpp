#include <cassert>
#include "ParticleGroup.h"
#include "PSManager.h"
#include "Global/Rendering.h"
#include "Factories/ParticleFactory.h"
#include "Factories/EffectFactory.h"
#include "Camera/ICamera.h"
#include "Mesh/IMesh.h"

CoreParticleGroup::CoreParticleGroup( )
: name( "" )
{
}

CoreParticleGroup::~CoreParticleGroup( ) {
	printDebugString( "Destroying CoreParticleGroup...\n" );
}

// ----------------------------------------------------------------

#define PARTICLES_FADE_TIME 0.5f
#define MAX_PARTICLES 50

ParticleGroup::ParticleGroup( )
: particle_handle( 0 )
, core( NULL )
, life( 0.0f )
, particle_group_finished( false )
, fade_to_kill( false )
, fade_alpha( 1.0f )
, curr_time_fade( 0.0f )
, mesh( NULL )
, shader( NULL )
{
	mesh = ParticleFactory::get( )->createParticle( MAX_PARTICLES );
	assert( mesh );
	shader = EffectFactory::get( )->createEffect( "shader.fx" );
	assert( shader && shader->fx );
}

ParticleGroup::~ParticleGroup( ) {
	printDebugString( "Destroying ParticleGroup...\n" );

	assert( core );
	const CoreParticleGroup::CorePSubsystems &subsystems = core->core_psubsystems;
	int first = getFirstHandleGroup();
	PSManager::particleEngine.DeleteParticleGroups( first, (int)subsystems.size() );
}

bool ParticleGroup::update( float deltaTime ) {
	// Update life of group
	life += deltaTime;

	// Hacer el fade mientras la particula no acabe por otro lado( por tiempo por ejemplo )
	if( !particle_group_finished && fade_to_kill ) {
		// Hacer fade
		curr_time_fade += deltaTime;
		if( curr_time_fade >= PARTICLES_FADE_TIME )
			curr_time_fade = PARTICLES_FADE_TIME;
		assert( PARTICLES_FADE_TIME != 0.0f );
		fade_alpha = 1.0f - curr_time_fade / PARTICLES_FADE_TIME;

		// El fade ha terminado
		if( fade_alpha == 0.0f )
			particle_group_finished = true;
	}

	// Update particle
	return updateParticle( deltaTime );
}

bool ParticleGroup::updateParticle( float deltaTime ) {
	// This particle group has finished?
	if( particle_group_finished )
		return true;

	// Update delta time in particle engine
	PSManager::particleEngine.TimeStep(deltaTime); 

	// Update subsystems
	const CoreParticleGroup::CorePSubsystems &subsystems = core->core_psubsystems;
	int first = getFirstHandleGroup();
	int last = first + (int)subsystems.size();
	size_t death_groups = 0;
	for (int index = first; index < last; ++index) {
		int i = index - first;

		int actionListHandle = subsystems[i].getUpdateHandle( life );
		if (actionListHandle != -1) {
			PSManager::particleEngine.CurrentGroup(index);
			PSManager::particleEngine.CallActionList(actionListHandle);

			//PSManager::particleEngine.setDomainTransform(actionListHandle, "SourceDomain", tracker.getTransform( ) );
			//PSManager::particleEngine.setDomainOffset(actionListHandle, "", PAPI::pVec(offset.x, offset.y, offset.z));
		}
		else
			++death_groups;
	}

	// Si todos los subsistemas de particulas han muerto, hemos muerto
	if( death_groups == subsystems.size() )
		return true;

	return false;
}

void ParticleGroup::render() {
	assert( mesh );
	assert( shader && shader->fx );

	// Calculate World View Projection matrix
	ICamera *camera = getCurrentCamera( );
	const D3DXMATRIX &matView = camera->getMatView();
	const D3DXMATRIX &matProj = camera->getMatProj();
	D3DXMATRIX matWorldViewProj;
	D3DXMatrixMultiply(&matWorldViewProj, &matView, &matProj);

	// Update matrix wvp of shader
	D3DXHANDLE handleParam = shader->fx->GetParameterByName( NULL, "world" );
	if( handleParam )
		shader->fx->SetMatrix( handleParam, &tracker.getTransform( ) );

	// Update matrix wvp of shader
	handleParam = shader->fx->GetParameterByName( NULL, "world_view_proj" );
	if( handleParam )
		shader->fx->SetMatrix( handleParam, &matWorldViewProj );

	handleParam = shader->fx->GetParameterByName( NULL, "camera_up" );
	if( handleParam ) {
		const D3DXVECTOR3 & cameraUp = camera->getLocalUpVector();
		D3DXVECTOR4 cameraUp4( cameraUp.x, cameraUp.y, cameraUp.z, 0.0f );
		shader->fx->SetVector( handleParam, &cameraUp4);
	}

	handleParam = shader->fx->GetParameterByName( NULL, "camera_left" );
	if( handleParam ) {
		const D3DXVECTOR3 & cameraLeft = camera->getLocalLeftVector();
		D3DXVECTOR4 cameraLeft4( -cameraLeft.x, -cameraLeft.y, -cameraLeft.z, 0.0f );
		shader->fx->SetVector( handleParam, &cameraLeft4);
	}

	const CoreParticleGroup::CorePSubsystems &subsystems = core->core_psubsystems;
	int first = getFirstHandleGroup();
	int last = first + (int)subsystems.size();
	for (int index = first; index < last; ++index) {
		PSManager::particleEngine.CurrentGroup(index);

		// Set texture and color properties
		int i = index - first;
		renderSubsystem( subsystems[i] );
	}
}

void ParticleGroup::renderSubsystem( const CorePSubsystem &subsystem ) {

	// Activate diffuse texture
	D3DXHANDLE handleParam = shader->fx->GetParameterByName( NULL, "diffuse_texture" );
	if( handleParam ) 
		shader->fx->SetTexture( handleParam, subsystem.getTexture()->tex );

	// Activate technique
	handleParam = shader->fx->GetTechniqueByName( subsystem.getShaderType().c_str() );
	if( handleParam ) {
		HRESULT hr = shader->fx->SetTechnique( handleParam );
		assert( hr == D3D_OK );
	}

	UINT cPasses, iPass;
	shader->fx->Begin( &cPasses, 0 ); // How many passes has the technique?
	for( iPass = 0; iPass < cPasses; ++iPass ) { // For each pass
		shader->fx->BeginPass( iPass );	// Begin pass

		// Call to render
		size_t size = PSManager::particleEngine.GetGroupCount();
		size_t bloques = size / MAX_PARTICLES;
		size_t resto = size % MAX_PARTICLES;
		for (size_t j=0; j<bloques; ++j) {
			// Particle Data to shader
			size_t ini = j * MAX_PARTICLES;
			size_t fin = ini + MAX_PARTICLES;
			setParticleDataToShader(ini, fin);

			// State changes have happened after BeginPass is called
			shader->fx->CommitChanges();
			// Render
			mesh->renderStripTriangles( 0, MAX_PARTICLES * 2 );
		}

		// PASAR LOS DATOS AL SHADER
		if (resto > 0) {
			setParticleDataToShader(bloques * MAX_PARTICLES, size);

			// State changes have happened after BeginPass is called
			shader->fx->CommitChanges();
			// Render
			mesh->renderStripTriangles( 0, (int)resto * 2 );
		}

		shader->fx->EndPass( );	// End Pass
	}
	shader->fx->End( );
}

void ParticleGroup::setParticleDataToShader( size_t ini, size_t fin ) {
	// Billboards a nivel logico
	struct TBillboard {
		float x,y,z,sz;		// Paquetes de 4 floats
		float r,g,b,a;
	};
	TBillboard bill_boards[ MAX_PARTICLES ];
	memset( bill_boards, 0x00, sizeof( bill_boards ) );

	float *ptr;
	size_t flstride, pos3Ofs, posB3Ofs, size3Ofs,
		vel3Ofs, velB3Ofs, color3Ofs, alpha1Ofs, 
		age1Ofs, up3Ofs, rvel3Ofs, upB3Ofs, mass1Ofs, data1Ofs;

	size_t cnt = PSManager::particleEngine.GetParticlePointer(ptr, flstride, pos3Ofs, posB3Ofs,
		size3Ofs, vel3Ofs, velB3Ofs, color3Ofs, alpha1Ofs, age1Ofs,
		up3Ofs, rvel3Ofs, upB3Ofs, mass1Ofs, data1Ofs);
	assert(cnt > 0);

	size_t start = ini * flstride;
	size_t end = fin * flstride;
	unsigned index = 0;
	for (size_t offset = start; offset < end; offset += flstride) {
		assert( index < MAX_PARTICLES );

		size_t base = pos3Ofs + offset;

		// Position
		bill_boards[index].x = ptr[base++];
		bill_boards[index].y = ptr[base++];
		bill_boards[index].z = ptr[base];
		//D3DXVECTOR3 center( bill_boards[index].x, bill_boards[index].y, bill_boards[index].z );
		//D3DXVec3TransformCoord( &center, &center, &tracker.getTransform( ) );
		//bill_boards[index].x = center.x;
		//bill_boards[index].y = center.y;
		//bill_boards[index].z = center.z;

		// Size
		base = offset + size3Ofs;
		bill_boards[index].sz = ptr[base];

		// Color
		base = offset + color3Ofs;
		bill_boards[index].r = ptr[base++];
		bill_boards[index].g = ptr[base++];
		bill_boards[index].b = ptr[base];

		// Alpha
		base = offset + alpha1Ofs;
		bill_boards[index].a = ptr[base] * fade_alpha;

		++index;
	}

	// To shader
	D3DXHANDLE handle_param = shader->fx->GetParameterByName( NULL, "particle_data" );
	if( handle_param ) 
		shader->fx->SetVectorArray( handle_param, (D3DXVECTOR4 *) bill_boards, index * 2);
}

void ParticleGroup::kill( bool fading ) {
	if( !fading )
		particle_group_finished = true;
	else
		fade_to_kill = true;
}
