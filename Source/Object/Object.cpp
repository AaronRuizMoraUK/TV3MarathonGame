#include <cassert>
#include "Object.h"
#include "Mesh/IMesh.h"
#include "Camera/ICamera.h"
#include "Global/Geometry.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Particles/ParticleGroup.h"

Object::Object()
: mesh(NULL)
, particle_group( NULL )
{
}

// Override copy constructor
Object::Object(const Object &source)
: SignalListener(source)
, mesh(source.mesh)
, particle_group( NULL ) // No se copia
{
	copy(source);
}

Object::~Object() {
	printDebugString("Destroying Object...\n");
	destroy( );

	// Object does not destroy its Mesh, MeshFactory does!!.

	// Automaticaly material vector destroies itself
}

// Override Operator '='
void Object::operator=(const Object &source) {
	SignalListener::operator=(source);
	copy(source);
}

void Object::destroy() {
	if( particle_group ) {
		particle_group->setTracker( TTracker( particle_group->getTracker().getTransform( ) ) );
		particle_group->kill( true );
		particle_group = NULL;
	}

	loc = LocationCollisionable( ); // Para eliminar el Collision Object si tiene
	mesh = NULL;
	materialVector.clear( );
}

void Object::copy(const Object &source) {
	destroy( );

	loc = source.loc;
	mesh = source.mesh;
	materialVector = source.materialVector;
	particle_group = NULL; // No se copia
}

void Object::render() {
	assert( mesh );

	// Calculate World View Projection matrix
	ICamera *camera = getCurrentCamera( );
	const D3DXMATRIX &matView = camera->getMatView();
	const D3DXMATRIX &matProj = camera->getMatProj();
	const D3DXMATRIX &matWorld = loc.getTransform( );
	D3DXMATRIX matWorldViewProj;
	D3DXMatrixMultiply(&matWorldViewProj, &matView, &matProj);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matWorldViewProj);

	for(unsigned int i=0; i<materialVector.size(); ++i) {
		materialVector[i]->setMatWorld(matWorld);
		materialVector[i]->setMatWorldViewProj(matWorldViewProj);
		materialVector[i]->activate();

		UINT cPasses, iPass;
		EffectDX shader_fx = materialVector[i]->getShader().fx;
		assert( shader_fx );
		shader_fx->Begin( &cPasses, 0 ); // How many passes has the technique?
		for( iPass = 0; iPass < cPasses; ++iPass ) { // For each pass
			shader_fx->BeginPass( iPass );	// Begin pass

			// Do the real rendering of geometry
			mesh->renderStrip(i);

			shader_fx->EndPass( );	// End Pass
		}
		shader_fx->End( );
	}
}

void Object::renderDebug( ) {
#ifndef NDEBUG
	assert( mesh );

	// DRAW BOUNDING BOX, AXIS BOUNDING SPHERE, AXIS AND DIRECTION
	Global::device->SetTransform( D3DTS_WORLD, &loc.getTransform( ) );
	mesh->getBoundingBox().renderDebug( );
	mesh->getBoundingSphere().renderDebug( );
	drawAxis();
	drawDirection();
#endif
}

// VERY VERY VERY OPTIONAL!!!!
void Object::SETMATERIAL (const std::string &texture, const std::string &shader, const std::string &technique){
	assert( materialVector.empty( ) );
	Material *material = new Material(texture, shader, technique);
	materialVector.push_back(material);
}

// VERY VERY VERY OPTIONAL!!!!
void Object::SETMATERIAL (const Texture *texture, const std::string &shader, const std::string &technique){
	assert( materialVector.empty( ) );
	Material *material = new Material(texture, shader, technique);
	materialVector.push_back(material);
}

void Object::CHANGETEXTURE( const std::string &texture ) {
	assert( materialVector.size( ) == 1 );
	materialVector[ 0 ]->changeTexture( texture );
}

void Object::CHANGETEXTUREANDNORMAL( const std::string &texture, const std::string &normalmap_texture ) {
	assert( materialVector.size( ) == 1 );
	materialVector[ 0 ]->changeTextureAndNormal( texture, normalmap_texture );
}

Material *Object::getMaterial( ) {
	assert( materialVector.size( ) == 1 );
	return materialVector[ 0 ];
}
