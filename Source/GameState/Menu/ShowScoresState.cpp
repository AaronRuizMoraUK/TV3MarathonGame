#include "ShowScoresState.h"
#include "MenuState.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "IO/IOStatus.h"
#include "Camera/ICamera.h"
#include "Source/Game.h"
#include "Logic/Logic.h"
#include "Factories/QuadFactory.h"

ShowScoresState::ShowScoresState()
: enter_scores(true)
, exit_scores(false)
{
}

ShowScoresState::~ShowScoresState() {
	printDebugString("Destroying ShowScoresState...\n");
}

void ShowScoresState::update( float deltaTime ) {

	if ( enter_scores ) {
		if( !Global::fade.isFinished( ) )
			Global::fade.update( deltaTime );
		else
			enter_scores = false;
	}
	else if( exit_scores ) {
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
		if( ioStatus.start.becomesPressed( ) || ioStatus.selectOption.becomesPressed() )
			exit_scores = true;
	}
}

void ShowScoresState::preRender( ) {
	activeCameraOrtho( );
	MenuState *menuState = (MenuState *) getState( "menu_state" );
	menuState->background.render();
	activeCamera3D( );
}

void ShowScoresState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	// Menu State Renders
	MenuState *menuState = (MenuState *) getState( "menu_state" );
	menuState->menuPlanet.render();

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void ShowScoresState::renderWorld2D( ) {
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

void ShowScoresState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	MenuState *menuState = (MenuState *) getState( "menu_state" );
	menuState->title.render();

	renderStateDisableAlphaBlending( );
}

void ShowScoresState::renderGameUI( ) {
	renderStateEnableAlphaBlending();

	// Render fade in
	if( enter_scores )
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Render fade out
	else if( exit_scores )
		Global::fade.renderFadeIn(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Render credits
	else {
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));
		
		flowerUp.render( );
		flowerDown.render( );

		size_t num_score_infos = Global::score_recorder.getNumScoreInfos( );
		if( num_score_infos > 0 ) {
			alphabet.printf( 0.33f, 0.8f, 0.07f, 0.07f, "puntuacions" );

			float py = 0.7f;
			for( size_t i=0; i<num_score_infos; ++i ) {
				const ScoreRecorder::ScoreInfo &score_info = Global::score_recorder.getScoreInfo( i );
				int day, month, year;
				score_info.getDate( day, month, year );

				std::string printf_char = "%2d) %s ";
				printf_char += PRINTF_SCORE_CHAR;
				printf_char += " (%02d-%02d-%04d)";
				alphabet.printf( 0.18f, py, 0.05f, 0.05f, printf_char.c_str(), i+1, score_info.initials, score_info.score, day, month, year );
				py -= 0.06f;
			}
		}
		else {
			alphabet.printf( 0.24f, 0.7f, 0.06f, 0.06f, "no hi ha puntuacions" );
		}
	}

	renderStateDisableAlphaBlending();
}

void ShowScoresState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Show Scores Status");
#endif
}

void ShowScoresState::onLoad( ) {
	enter_scores = true;
	exit_scores = false;
}

void ShowScoresState::onUnload( ) {
}

void ShowScoresState::loadGameState( ) {
	alphabet.load( );

	// Flower up
	const IMesh * flowerUpMesh = QuadFactory::get()->createQuad(0.6f, 0.15f * Game::configOptions.resolution.aspect_ratio );
	assert(flowerUpMesh);
	flowerUp.setMesh(flowerUpMesh);
	flowerUp.SETMATERIAL("flower_options.dds", "shader.fx", "dummy_tech");
	flowerUp.loc.setPosition( D3DXVECTOR3(-0.3f, 0.65f, 0.0f) );

	// Flower down
	flowerDown = flowerUp;
	D3DXMATRIX flowerDowntransformMatrix;
	D3DXMatrixRotationZ( &flowerDowntransformMatrix, D3DX_PI );
	flowerDown.loc.setTransform( flowerDowntransformMatrix );
	flowerDown.loc.setPosition( D3DXVECTOR3(0.3f, -0.65f, 0.0f) );
}
