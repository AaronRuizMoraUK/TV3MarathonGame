#include "FourthLevelState.h"
#include "EndFourthLevelState.h"
#include "GameState/Game/ScoreRecountState.h"
#include "Logic/FourthLevelLogic.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "IO/IOStatus.h"
#include "Camera/Camera3D.h"
#include "Source/Game.h"
#include "Particles/PSManager.h"
#include "Managers/BrainManager.h"
#include "Factories/QuadFactory.h"

const float FourthLevelState::max_hurt_time = 1.0f;

FourthLevelState::FourthLevelState()
: hurt_time( 0.0f )
{
}

FourthLevelState::~FourthLevelState() {
	printDebugString("Destroying FourthLevelState...\n");
}

void FourthLevelState::update( float deltaTime ) {
	// GANAR
	if( getFourthLevelLogic( ).isLevelWon( )
#ifndef NDEBUG
		|| isPushed( 'G' )
#endif
		) {
		EndFourthLevelState *end_fourth_level_state = (EndFourthLevelState *)getState( "end_fourth_level_state" );
		end_fourth_level_state->next_instruction = InstructionsState::SCORE_RECOUNT;

		// Prepare that after Score Recount, we go to Game Won
		ScoreRecountState *score_recount_state = (ScoreRecountState *)getState( "score_recount_state" );
		score_recount_state->next_instruction = InstructionsState::GAME_WON;

		// Go to End Fourth Level State
		end_fourth_level_state->getFocus( );
		return;
	}
	// PERDER
	else if( getFourthLevelLogic( ).isLevelLost( ) 
#ifndef NDEBUG
		|| isPushed( 'P' ) 
#endif
		) {
		// Bajar una vida
		Logic::lifeDown( );

		// Hemos acabado el juego
		EndFourthLevelState *end_fourth_level_state = (EndFourthLevelState *)getState( "end_fourth_level_state" );
		if( Logic::isGameOver( ) )
			end_fourth_level_state->next_instruction = InstructionsState::GAME_OVER;
		else
			end_fourth_level_state->next_instruction = InstructionsState::LOSE_FOURTH_LEVEL;
		end_fourth_level_state->getFocus( );
		return;
	}
	// PAUSE
	else if( ioStatus.start.becomesPressed() ) {
		showPause( "end_fourth_level_state" );
		return;
	}

	// -------------------------------------------

	// Update camera
	getCamera3D( ).update(deltaTime);

	// Update frustums
	Global::frustum.update();
	Global::back_side_frustum.update();

	// Update Controllers
	Global::player->updateController(deltaTime);

	// Update AIs
	getFourthLevelLogic( ).dopaminas.updateAI( deltaTime );
	getFourthLevelLogic( ).noradrenalinas.updateAI( deltaTime );
	getFourthLevelLogic( ).twisters.updateAI( deltaTime );
	getFourthLevelLogic( ).expenders.updateAI( deltaTime );
	getFourthLevelLogic( ).power_ups.updateAI( deltaTime );

	// Update Animations
	Global::player->updateAnimation(deltaTime);
	Global::sky.updateAnimation(deltaTime);
	getFourthLevelLogic( ).expenders.updateAnimation( deltaTime );
	getFourthLevelLogic( ).power_ups.updateAnimation( deltaTime );

	// Spatial and Collisions
	Global::spatial.update( deltaTime );
	Global::collisions.update( deltaTime );

	// Update Logic and triggers
	getFourthLevelLogic( ).update( deltaTime );
	Global::triggers->update( deltaTime );

	// Update Particles
	PSManager::get( )->update( deltaTime );

	// Update UI
	fourth_level_ui.update( deltaTime );

	// Update Hurt
	if( hurt_time > 0.0f )
		hurt_time -= deltaTime;
}

void FourthLevelState::preRender( ) {
}

void FourthLevelState::renderWorld3D( ) {
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

void FourthLevelState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );
	
	getFourthLevelLogic( ).dopaminas.render( );
	getFourthLevelLogic( ).noradrenalinas.render( );
	getFourthLevelLogic( ).power_ups.render( );

	// Render Particles
	PSManager::get( )->render( );

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void FourthLevelState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	vignetting.render( );

	if( hurt_time > 0.0f )
		hurt.render( hurt_time/max_hurt_time );

	renderStateDisableAlphaBlending( );
}

void FourthLevelState::renderGameUI( ) {
	renderStateEnableAlphaBlending( );

	fourth_level_ui.render( );

	renderStateDisableAlphaBlending( );
}

void FourthLevelState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 170;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0,  py, 0xffff0000, "Fourth Level Status");
	py += 10;

	py += printf(0, py, 0xffff0000, "Num Dopaminas %d", getFourthLevelLogic( ).dopaminas.size( ) );
	py += printf(0, py, 0xffff0000, "Num Noradrenalinas %d", getFourthLevelLogic( ).noradrenalinas.size( ) );
	py += printf(0, py, 0xffff0000, "Num Twisters %d", getFourthLevelLogic( ).twisters.size( ) );
	py += 10;

	py += printf(0, py, 0xffff0000, "Dopamina Level %0.3f", getFourthLevelLogic( ).getDopaminaLevel( ) * 100.0f );
	py += printf(0, py, 0xffff0000, "Noradrenalina Level %0.3f", getFourthLevelLogic( ).getNoradrenalinaLevel( ) * 100.0f );
	py += 10;

	py += printf(0, py, 0xffff0000, "Right Level %s", getFourthLevelLogic( ).isLevelsRight( ) ? "YES" : "NO" );
	py += 10;
	
	py += printf(0, py, 0xffff0000, "Num PowerUps %d", getFourthLevelLogic( ).power_ups.size( ) );
#endif
}

void FourthLevelState::onLoad( ) {
	fourth_level_ui.reset( );
	if( getFourthLevelLogic( ).must_show_welcome_image ) {
		getFourthLevelLogic( ).must_show_welcome_image = false;
		fourth_level_ui.showWelcomeImage( );
	}

	hurt_time = 0.0f;
}

void FourthLevelState::onUnload( ) {
}

void FourthLevelState::loadGameState( ) {
		// Vignetting
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		vignetting.SETMATERIAL("vignetting_pan.dds", "shader.fx", "dummy_tech");
	else
		vignetting.SETMATERIAL("vignetting.dds", "shader.fx", "dummy_tech");

	const IMesh * mesh = QuadFactory::get()->createQuad( 1.0f, 1.0f );
	assert(mesh);
	vignetting.setMesh(mesh);

	// Hurt
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		hurt.load("hurt_pan.dds");
	else
		hurt.load("hurt.dds");
	D3DXMATRIX hurt_transform;
	D3DXMatrixScaling( &hurt_transform, aspect_ratio, 1.0f, 1.0f );
	hurt.setTransform( hurt_transform );

	fourth_level_ui.load( );
}
