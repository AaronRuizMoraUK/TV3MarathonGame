#include "EndFirstLevelState.h"
#include "Global/Variables.h"
#include "Global/Rendering.h"
#include "Source/Game.h"
#include "Bass/SoundPlayer.h"
#include "Camera/Camera3D.h"
#include "Logic/FirstLevelLogic.h"
#include "Particles/PSManager.h"
#include "Managers/BrainManager.h"
#include "Factories/QuadFactory.h"

EndFirstLevelState::EndFirstLevelState()
: next_instruction( InstructionsState::NONE )
{
}

EndFirstLevelState::~EndFirstLevelState() {
	printDebugString("Destroying EndFirstLevelState...\n");
}

void EndFirstLevelState::update( float deltaTime ) {
	// Update Camera
	getCamera3D( ).update( deltaTime );

	// Update frustums
	Global::frustum.update();
	Global::back_side_frustum.update();

	// Update AIs
	getFirstLevelLogic( ).dopaminas.updateAI( deltaTime );
	//getFirstLevelLogic( ).alucinaciones.updateAI( deltaTime ); // Que no hagan el update en el fade out si ya hemos ganado
	getFirstLevelLogic( ).power_ups.updateAI( deltaTime );

	// Update Animations
	Global::player->updateAnimation(deltaTime);
	Global::sky.updateAnimation(deltaTime);
	getFirstLevelLogic( ).alucinaciones.updateAnimation( deltaTime );
	getFirstLevelLogic( ).power_ups.updateAnimation( deltaTime );

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

void EndFirstLevelState::preRender( ) {
}

void EndFirstLevelState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	// Game Renders
	Global::player->render();
	BrainManager::get( )->getCurrentBrain( ).render();
	Global::sky.render();
	getFirstLevelLogic( ).alucinaciones.render( );

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void EndFirstLevelState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );

	getFirstLevelLogic( ).dopaminas.render( );
	getFirstLevelLogic( ).power_ups.render( );

	// Render Particles
	PSManager::get( )->render( );

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void EndFirstLevelState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	vignetting.render( );

	renderStateDisableAlphaBlending( );
}

void EndFirstLevelState::renderGameUI( ) {
}

void EndFirstLevelState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "End First Level Status");
#endif

	renderStateEnableAlphaBlending( );
	Global::fade.renderFadeOut( 0.5f );
	renderStateDisableAlphaBlending( );
}

void EndFirstLevelState::onLoad( ) {
	assert( next_instruction != InstructionsState::NONE );
	SoundPlayer::get( )->stopAll( true );
}

void EndFirstLevelState::onUnload( ) {
	next_instruction = InstructionsState::NONE;
	getFirstLevelLogic( ).destroyLevel( );
}

void EndFirstLevelState::loadGameState( ) {
	// Vignetting
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		vignetting.SETMATERIAL("vignetting_pan.dds", "shader.fx", "dummy_tech");
	else
		vignetting.SETMATERIAL("vignetting.dds", "shader.fx", "dummy_tech");

	const IMesh * mesh = QuadFactory::get()->createQuad( 1.0f, 1.0f );
	assert(mesh);
	vignetting.setMesh(mesh);
}
