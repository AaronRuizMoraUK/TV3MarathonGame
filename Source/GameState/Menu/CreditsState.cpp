#include "CreditsState.h"
#include "MenuState.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "IO/IOStatus.h"
#include "Camera/ICamera.h"
#include "Source/Game.h"

CreditsState::CreditsState()
: enter_credits(true)
, exit_credits(false)
{
}

CreditsState::~CreditsState() {
	printDebugString("Destroying CreditsState...\n");
}

void CreditsState::update( float deltaTime ) {

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
			// Go to menu state, but telling it that not make its onLoad
			MenuState * menuState = (MenuState *) getState( "menu_state" );
			menuState->doOnLoad = false;
			menuState->getFocus( );
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

void CreditsState::preRender( ) {
	activeCameraOrtho( );
	MenuState *menuState = (MenuState *) getState( "menu_state" );
	menuState->background.render();
	activeCamera3D( );
}

void CreditsState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	// Menu State Renders
	MenuState *menuState = (MenuState *) getState( "menu_state" );
	menuState->menuPlanet.render();

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void CreditsState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );

	MenuState *menuState = (MenuState *) getState( "menu_state" );
	menuState->startAction.render();
	menuState->optionsAction.render();
	menuState->creditsAction.render();
	menuState->scoresAction.render();
	menuState->exitAction.render();

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void CreditsState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	MenuState *menuState = (MenuState *) getState( "menu_state" );
	menuState->title.render();

	renderStateDisableAlphaBlending( );
}

void CreditsState::renderGameUI( ) {
	renderStateEnableAlphaBlending();

	// Render fade medium out
	if( enter_credits )
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Render fade in
	else if( exit_credits )
		Global::fade.renderFadeIn(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Render credits
	else {
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));
		texture_sequencer.render( );
	}

	renderStateDisableAlphaBlending();
}

void CreditsState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Credits Status");
#endif
}

void CreditsState::onLoad( ) {
	enter_credits = true;
	exit_credits = false;

	texture_sequencer.initiate( );
}

void CreditsState::onUnload( ) {
}

void CreditsState::loadGameState( ) {
	texture_sequencer.load( "credits.xml" );
}
