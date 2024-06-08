#include "FinalCreditsState.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "IO/IOStatus.h"
#include "Source/Game.h"
#include "Factories/QuadFactory.h"

FinalCreditsState::FinalCreditsState()
: enter_credits(true)
, exit_credits(false)
{
}

FinalCreditsState::~FinalCreditsState() {
	printDebugString("Destroying FinalCreditsState...\n");
}

void FinalCreditsState::update( float deltaTime ) {

	if ( enter_credits ) {
		if( !Global::fade.isFinished( ) )
			Global::fade.update( deltaTime );
		else
			enter_credits = false;
	}
	else if( exit_credits ) {
		if( !Global::fade.isFinished( ) ) {
			Global::fade.update( deltaTime );
		}
		else {
			getState( "menu_state" )->getFocus( );
		}
	}
	else {
		bool finished = texture_sequencer.update( deltaTime );

		if( ioStatus.start.becomesPressed( ) 
			|| ioStatus.selectOption.becomesPressed() 
			|| finished )
				exit_credits = true;
	}
}

void FinalCreditsState::preRender( ) {
}

void FinalCreditsState::renderWorld3D( ) {
}

void FinalCreditsState::renderWorld2D( ) {
}

void FinalCreditsState::renderPostProcessed2D( ) {
	background.render();
}

void FinalCreditsState::renderGameUI( ) {
	renderStateEnableAlphaBlending();

	// Render fade medium out
	if( enter_credits )
		Global::fade.renderFadeIn(0.5f);
	// Render fade in
	else if( exit_credits )
		Global::fade.renderFadeOut(0.5f);
	// Render credits
	else {
		texture_sequencer.render( );
	}

	renderStateDisableAlphaBlending();
}

void FinalCreditsState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Final Credits Status");
#endif
}

void FinalCreditsState::onLoad( ) {
	enter_credits = true;
	exit_credits = false;

	texture_sequencer.initiate( );
}

void FinalCreditsState::onUnload( ) {
}

void FinalCreditsState::loadGameState( ) {
	texture_sequencer.load( "credits.xml" );

	// Background
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		background.SETMATERIAL("end_credits_background_pan.dds", "shader.fx", "dummy_tech");
	else
		background.SETMATERIAL("end_credits_background.dds", "shader.fx", "dummy_tech");

	const IMesh * mesh = QuadFactory::get()->createQuad( 1.0f, 1.0f );
	assert(mesh);
	background.setMesh(mesh);
}
