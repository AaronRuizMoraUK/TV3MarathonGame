#include "CalAnimatedObject.h"
#include "Global/Rendering.h"
#include "Camera/ICamera.h"
#include "Mesh/IMesh.h"
#include "Managers/CalAnimCoreModelManager.h"
#include "Factories/MeshFactory.h"

bool CalAnimatedObject::cal_animation_enabled = true;

CalAnimatedObject::CalAnimatedObject()
: calAnimModel( NULL )
, updateSkeleton( true )
{
}

CalAnimatedObject::CalAnimatedObject( const CalAnimatedObject &source )
: AnimatedObject( source )
, calAnimModel( NULL )
, updateSkeleton( source.updateSkeleton )
{
	copy( source );
}

CalAnimatedObject::~CalAnimatedObject() {
	printDebugString("Destroying CalAnimatedObject...\n");
	destroy( );
}

void CalAnimatedObject::operator=(const CalAnimatedObject &source) {
	AnimatedObject::operator=(source);
	copy(source);
}

void CalAnimatedObject::destroy( ) {
	if( calAnimModel )
		delete calAnimModel, calAnimModel = NULL;
}

void CalAnimatedObject::copy( const CalAnimatedObject &source ) {
	destroy( );
	if( source.calAnimModel != NULL && source.calAnimModel->core != NULL ) {
		CalAnimModel *new_calAnimModel = new CalAnimModel( *source.calAnimModel );
		assert( new_calAnimModel );
		setCalAnimModel( new_calAnimModel );
	}
	updateSkeleton = source.updateSkeleton;
}

void CalAnimatedObject::setCalAnimModel( const char *cal_core_model_name ) {
	// Get the Cal Animation Core Model
	const CalAnimCoreModel *core = CalAnimCoreModelManager::get( )->getCalAnimCoreModel( cal_core_model_name );
	assert( core );
	// Get the Cal Mesh
	const CalCoreMesh *core_mesh_cal = core->calCoreModel.getCoreMesh( core->meshId );
	assert( core_mesh_cal );

	// Set Mesh
	const IMesh * core_mesh = MeshFactory::get()->createMesh( core->name, core_mesh_cal );
	assert( core_mesh );
	setMesh( core_mesh );

	// Set the Cal Animation Model
	CalAnimModel *cam = new CalAnimModel( core );
	setCalAnimModel( cam );
}

void CalAnimatedObject::setCalAnimModel( CalAnimModel *pcalAnimModel ) {
	if( calAnimModel )
		delete calAnimModel, calAnimModel = NULL;
	calAnimModel = pcalAnimModel;
	calAnimModel->calModel.update( 0.0f );
}

void CalAnimatedObject::updateAnimation(float deltaTime) {
	AnimatedObject::updateAnimation( deltaTime );
	if( calAnimModel && cal_animation_enabled ) {
		calAnimModel->calModel.update( deltaTime, updateSkeleton );
		updateSkeleton = false; // Despues de actualizar decir que no se ha pintado
	}
}

void CalAnimatedObject::render() {
	assert( mesh );
	if( !calAnimModel ) {
		AnimatedObject::render( );
		return;
	}

	updateSkeleton = true; // Se va a pintar, asi que el skeleton se deberá actualizar en el updateAnimation

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
		materialVector[i]->activateCal( calAnimModel->calModel );
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

void CalAnimatedObject::renderDebug() {
#ifndef NDEBUG
	AnimatedObject::renderDebug( );

	assert( calAnimModel );
	assert( calAnimModel->calModel.getSkeleton() );

	// Render Bones with lines
	float positions[120][6];
	int numBones = calAnimModel->calModel.getSkeleton()->getBoneLines(&positions[0][0]);

	for(int i=0; i<numBones; ++i) {
		D3DXVECTOR3 line1 (&positions[i][0]);
		D3DXVECTOR3 line2 (&positions[i][3]);
		drawLine(line1, line2, 0xFF0000FF);
	}
#endif
}
