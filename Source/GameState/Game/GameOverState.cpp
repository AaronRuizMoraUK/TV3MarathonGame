#include <cassert>
#include "GameOverState.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Camera/ICamera.h"
#include "Source/Game.h"
#include "Factories/QuadFactory.h"
#include "IO/IOStatus.h"
#include "Bass/SoundPlayer.h"

const float GameOverState::print_enter_time = 0.2f;

GameOverState::GameOverState()
: fading_in( true )
, fading_out( false )
, curr_print_enter_time( 0.0f )
, print_enter( true )
, sound( 0 )
{
}

GameOverState::~GameOverState() {
	printDebugString("Destroying GameOverState...\n");
}

void GameOverState::update( float deltaTime ) {
	if ( fading_in ) {
		if( !Global::fade.isFinished( ) )
			Global::fade.update( deltaTime );
		else {
			fading_in = false;
			// Ejecutar sonido
			sound = SoundPlayer::get( )->playStream( "game_over", false );
		}
	}
	else if( fading_out ) {
		if( !Global::fade.isFinished( ) || ioStatus.selectOption.becomesPressed() )
			Global::fade.update( deltaTime );
		else {
			fading_out = false;
			
			// Go to menu
			getState( "menu_state" )->getFocus( );
		}
	}
	else {
		if( ioStatus.start.becomesPressed( ) || ioStatus.selectOption.becomesPressed() ) {
			SoundPlayer::get( )->playSample( "option_selected" );
			SoundPlayer::get( )->stopStream( true ); // Parar sonido
			fading_out = true;
		}
	}

	// Actualiar el parpadeo de ENTER msg
	curr_print_enter_time += deltaTime;
	if( curr_print_enter_time >= print_enter_time ) {
		curr_print_enter_time = print_enter_time - curr_print_enter_time;
		print_enter = !print_enter;
	}
}

void GameOverState::preRender( ) {
	activeCameraOrtho( );
	background.render( );
	activeCamera3D( );
}

void GameOverState::renderWorld3D( ) {
	//renderStateEnableZTest( );
	//renderStateEnableZWriting( );

	//renderStateDisableZWriting( );
	//renderStateDisableZTest( );
}

void GameOverState::renderWorld2D( ) {
	//renderStateEnableAlphaBlending( );
	//renderStateEnableZTest( );

	//renderStateDisableZTest( );
	//renderStateDisableAlphaBlending( );
}

void GameOverState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	//alphabet.print(0.3f, 0.7f, 0.1f, 0.1f, "GAME OVER");
	if( print_enter )
		alphabet.print( 0.63f, 0.1f, 0.03f, 0.03f, "ENTRAR - CONTINUAR" );

	renderStateDisableAlphaBlending( );
}

void GameOverState::renderGameUI( ) {
}

void GameOverState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Game Over Status");
#endif

	// Render fade medium out
	if( fading_in ) {
		renderStateEnableAlphaBlending( );
		Global::fade.renderFadeIn(0.5f);
		renderStateDisableAlphaBlending( );
	}
	// Render fade in
	else if( fading_out ) {
		renderStateEnableAlphaBlending( );
		Global::fade.renderFadeOut(0.5f);
		renderStateDisableAlphaBlending( );
	}
}

void GameOverState::onLoad( ) {
	fading_in = true;
	fading_out = false;
	curr_print_enter_time = 0.0f;
	print_enter = true;
	sound = 0;
}

void GameOverState::onUnload( ) {
	sound = 0;
}

void GameOverState::loadGameState() {
	// Background
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		background.SETMATERIAL("game_over_pan.dds", "shader.fx", "dummy_tech");
	else
		background.SETMATERIAL("game_over.dds", "shader.fx", "dummy_tech");

	const IMesh * mesh = QuadFactory::get()->createQuad( 1.0f, 1.0f );
	assert(mesh);
	background.setMesh(mesh);

	alphabet.load( );
}
