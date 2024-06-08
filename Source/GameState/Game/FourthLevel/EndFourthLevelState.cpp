#include "EndFourthLevelState.h"
#include "Logic/FourthLevelLogic.h"
#include "Global/Variables.h"
#include "Global/Rendering.h"
#include "Source/Game.h"
#include "Camera/Camera3D.h"
#include "Bass/SoundPlayer.h"
#include "Particles/PSManager.h"
#include "Managers/BrainManager.h"
#include "Factories/QuadFactory.h"

EndFourthLevelState::EndFourthLevelState()
: next_instruction( InstructionsState::NONE )
{
}

EndFourthLevelState::~EndFourthLevelState() {
	printDebugString("Destroying EndFourthLevelState...\n");
}

void EndFourthLevelState::update( float deltaTime ) {
	// Update Camera
	getCamera3D( ).update( deltaTime );

	// Update frustums
	Global::frustum.update();
	Global::back_side_frustum.update();

	// Update AIs
	getFourthLevelLogic( ).dopaminas.updateAI( deltaTime );
	getFourthLevelLogic( ).noradrenalinas.updateAI( deltaTime );
	//getFourthLevelLogic( ).twisters.updateAI( deltaTime );
	//getFourthLevelLogic( ).expenders.updateAI( deltaTime );
	getFourthLevelLogic( ).power_ups.updateAI( deltaTime );

	// Update Animations
	Global::player->updateAnimation(deltaTime);
	Global::sky.updateAnimation(deltaTime);
	getFourthLevelLogic( ).expenders.updateAnimation( deltaTime );
	getFourthLevelLogic( ).power_ups.updateAnimation( deltaTime );

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

void EndFourthLevelState::preRender( ) {
}

void EndFourthLevelState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	// Game Renders
	Global::player->render();
	BrainManager::get( )->getCurrentBrain( ).render();
	Global::sky.render();

	getFourthLevelLogic( ).expenders.render( );

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void EndFourthLevelState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );

	// Render
	getFourthLevelLogic( ).dopaminas.render( );
	getFourthLevelLogic( ).noradrenalinas.render( );
	getFourthLevelLogic( ).power_ups.render( );

	// Render Particles
	PSManager::get( )->render( );

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void EndFourthLevelState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	vignetting.render( );

	renderStateDisableAlphaBlending( );
}

void EndFourthLevelState::renderGameUI( ) {
}

void EndFourthLevelState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "End Fourth Level Status");
#endif

	renderStateEnableAlphaBlending( );
	Global::fade.renderFadeOut( 0.5f );
	renderStateDisableAlphaBlending( );
}

void EndFourthLevelState::onLoad( ) {
	assert( next_instruction != InstructionsState::NONE );
	SoundPlayer::get( )->stopAll( true );
}

void EndFourthLevelState::onUnload( ) {
	next_instruction = InstructionsState::NONE;
	getFourthLevelLogic( ).destroyLevel( );
}

void EndFourthLevelState::loadGameState( ) {
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
