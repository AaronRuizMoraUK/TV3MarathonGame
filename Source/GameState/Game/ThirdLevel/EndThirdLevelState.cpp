#include "EndThirdLevelState.h"
#include "Logic/ThirdLevelLogic.h"
#include "Global/Variables.h"
#include "Global/Rendering.h"
#include "Source/Game.h"
#include "Camera/Camera3D.h"
#include "Bass/SoundPlayer.h"
#include "Particles/PSManager.h"
#include "Managers/BrainManager.h"
#include "Factories/MeshFactory.h"
#include "Factories/QuadFactory.h"

EndThirdLevelState::EndThirdLevelState()
: next_instruction( InstructionsState::NONE )
{
}

EndThirdLevelState::~EndThirdLevelState() {
	printDebugString("Destroying EndThirdLevelState...\n");
}

void EndThirdLevelState::update( float deltaTime ) {
	// Update Camera
	getCamera3D( ).update( deltaTime );

	// Update frustums
	Global::frustum.update();
	Global::back_side_frustum.update();

	// Update AIs
	//getThirdLevelLogic( ).valves.updateAI( deltaTime );
	getThirdLevelLogic( ).power_ups.updateAI( deltaTime );

	// Update Animations
	Global::player->updateAnimation(deltaTime);
	Global::sky.updateAnimation(deltaTime);
	getThirdLevelLogic( ).valves.updateAnimation( deltaTime );
	getThirdLevelLogic( ).river.updateAnimation( deltaTime );
	getThirdLevelLogic( ).power_ups.updateAnimation( deltaTime );

	// Spatial and Collisions
	Global::spatial.update( deltaTime );
	Global::collisions.update( deltaTime );

	// Update Particles
	PSManager::get( )->update( deltaTime );

	if( !Global::fade.isFinished( ) )
		Global::fade.update( deltaTime );
	else {
		InstructionsState *instructions_state = (InstructionsState *) getState("instructions_state");
		instructions_state->instruction = next_instruction;
		instructions_state->getFocus();
		return;
	}
}

void EndThirdLevelState::preRender( ) {
}

void EndThirdLevelState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	// Game Renders
	Global::player->render();
	safe_path.render( );
	BrainManager::get( )->getCurrentBrain( ).render();
	Global::sky.render();

	getThirdLevelLogic( ).valves.render( );

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void EndThirdLevelState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );

	// Render
	getThirdLevelLogic( ).river.render( );
	getThirdLevelLogic( ).power_ups.render( );

	renderStateEnableAdditiveAlphaBlending( );
	renderStateDisableCulling( );
	getThirdLevelLogic( ).valves.renderGodrays( );
	renderStateEnableCulling( );
	renderStateEnableAlphaBlending( );

	// Render Particles
	PSManager::get( )->render( );

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void EndThirdLevelState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	vignetting.render( );

	renderStateDisableAlphaBlending( );
}

void EndThirdLevelState::renderGameUI( ) {
}

void EndThirdLevelState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "End Third Level Status");
#endif

	renderStateEnableAlphaBlending( );
	Global::fade.renderFadeOut( 0.5f );
	renderStateDisableAlphaBlending( );
}

void EndThirdLevelState::onLoad( ) {
	assert( next_instruction != InstructionsState::NONE );
	SoundPlayer::get( )->stopAll( true );
}

void EndThirdLevelState::onUnload( ) {
	next_instruction = InstructionsState::NONE;
	getThirdLevelLogic( ).destroyLevel( );
}

void EndThirdLevelState::loadGameState( ) {
	// Vignetting
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		vignetting.SETMATERIAL("vignetting_pan.dds", "shader.fx", "dummy_tech");
	else
		vignetting.SETMATERIAL("vignetting.dds", "shader.fx", "dummy_tech");

	const IMesh * mesh = QuadFactory::get()->createQuad( 1.0f, 1.0f );
	assert(mesh);
	vignetting.setMesh(mesh);

	// Safe Path
	const IMesh *safe_path_mesh = MeshFactory::get( )->createMesh( "safe_path_level3.mesh" );
	assert( safe_path_mesh );
	safe_path.setMesh( safe_path_mesh );
	safe_path.SETMATERIAL( "safe_path_level3.dds", "shader.fx", "basic_tech" );
}
